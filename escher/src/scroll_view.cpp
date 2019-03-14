#include <escher/scroll_view.h>
#include <escher/palette.h>

#include <new>

extern "C" {
#include <assert.h>
}

ScrollView::ScrollView(View * contentView, ScrollViewDataSource * dataSource) :
  View(),
  m_contentView(contentView),
  m_dataSource(dataSource),
  m_topMargin(0),
  m_rightMargin(0),
  m_bottomMargin(0),
  m_leftMargin(0),
  m_innerView(this),
  m_decorators(),
  m_backgroundColor(Palette::WallScreen)
{
  assert(m_dataSource != nullptr);
  setDecoratorType(Decorator::Type::Bars);
}

ScrollView::ScrollView(ScrollView&& other) :
  m_contentView(other.m_contentView),
  m_dataSource(other.m_dataSource),
  m_topMargin(other.m_topMargin),
  m_rightMargin(other.m_rightMargin),
  m_bottomMargin(other.m_bottomMargin),
  m_leftMargin(other.m_leftMargin),
  m_innerView(this),
  m_backgroundColor(other.m_backgroundColor)
{
  setDecoratorType(other.m_decoratorType);
}

KDSize ScrollView::minimalSizeForOptimalDisplay() const {
  KDSize contentSize = m_contentView->minimalSizeForOptimalDisplay();
  return KDSize(
    contentSize.width() + m_leftMargin + m_rightMargin,
    contentSize.height() + m_topMargin + m_bottomMargin
  );
}


void ScrollView::scrollToContentPoint(KDPoint p, bool allowOverscroll) {
  if (!allowOverscroll && !m_contentView->bounds().contains(p)) {
    return;
  }
  KDCoordinate offsetX = 0;
  KDCoordinate offsetY = 0;
  KDRect visibleRect = visibleContentRect();
  if (visibleRect.left() > p.x()) {
    offsetX = p.x() - visibleRect.left();
  }
  if (visibleRect.right() < p.x()) {
    offsetX = p.x() - visibleRect.right();
  }
  if (visibleRect.top() > p.y()) {
    offsetY = p.y() - visibleRect.top();
  }
  if (visibleRect.bottom() < p.y()) {
    offsetY = p.y() - visibleRect.bottom();
  }
  if (offsetX != 0 || offsetY != 0) {
    setContentOffset(contentOffset().translatedBy(KDPoint(offsetX, offsetY)));
  }

  /* Handle cases when the size of the view has decreased. */
  setContentOffset(KDPoint(
    min(contentOffset().x(), max(minimalSizeForOptimalDisplay().width() - bounds().width(), 0)),
    min(contentOffset().y(), max(minimalSizeForOptimalDisplay().height() - bounds().height(), 0))
  ));
}

void ScrollView::scrollToContentRect(KDRect rect, bool allowOverscroll) {
  scrollToContentPoint(rect.topLeft(), allowOverscroll);
  scrollToContentPoint(rect.bottomRight(), allowOverscroll);
}

KDRect ScrollView::visibleContentRect() {
  return KDRect(
    contentOffset().x(),
    contentOffset().y(),
    m_frame.width() - m_leftMargin - m_rightMargin,
    m_frame.height() - m_topMargin - m_bottomMargin
  );
}

void ScrollView::layoutSubviews() {
  KDRect innerFrame = decorator()->layoutIndicators(minimalSizeForOptimalDisplay(), contentOffset(), bounds());
  m_innerView.setFrame(innerFrame);
  KDPoint absoluteOffset = contentOffset().opposite().translatedBy(KDPoint(m_leftMargin - innerFrame.x(), m_topMargin - innerFrame.y()));
  KDRect contentFrame = KDRect(absoluteOffset, contentSize());
  m_contentView->setFrame(contentFrame);
}

void ScrollView::setContentOffset(KDPoint offset, bool forceRelayout) {
  if (m_dataSource->setOffset(offset) || forceRelayout) {
    layoutSubviews();
  }
}

void ScrollView::InnerView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate offsetX = m_scrollView->contentOffset().x() + m_frame.x();
  KDCoordinate offsetY = m_scrollView->contentOffset().y() + m_frame.y();
  KDCoordinate contentHeight = m_scrollView->m_contentView->bounds().height();
  KDCoordinate contentWidth = m_scrollView->m_contentView->bounds().width();
  ctx->fillRect(KDRect(0, 0, width, m_scrollView->m_topMargin-offsetY), m_scrollView->m_backgroundColor);
  ctx->fillRect(KDRect(0, contentHeight+m_scrollView->m_topMargin-offsetY, width, height - contentHeight - m_scrollView->m_topMargin + offsetY), m_scrollView->m_backgroundColor);
  ctx->fillRect(KDRect(0, 0, m_scrollView->m_leftMargin-offsetX, height), m_scrollView->m_backgroundColor);
  ctx->fillRect(KDRect(contentWidth + m_scrollView->m_leftMargin - offsetX, 0, width - contentWidth - m_scrollView->m_leftMargin + offsetX, height), m_scrollView->m_backgroundColor);
}

ScrollView::BarDecorator::BarDecorator() :
  m_verticalBar(),
  m_horizontalBar()
{
}

View * ScrollView::BarDecorator::indicatorAtIndex(int index) {
  switch(index) {
    case 1:
      return &m_verticalBar;
    default:
      assert(index == 2);
      return &m_horizontalBar;
  }
}

KDRect ScrollView::BarDecorator::layoutIndicators(KDSize content, KDPoint offset, KDRect frame) {
  KDCoordinate hBarFrameBreadth = k_barsFrameBreadth * m_horizontalBar.update(
    content.width(),
    offset.x(),
    frame.width()
  );
  KDCoordinate vBarFrameBreadth = k_barsFrameBreadth * m_verticalBar.update(
    content.height(),
    offset.y(),
    frame.height()
  );
  /* If the two indicators are visible, we leave an empty rectangle in the right
   * bottom corner. Otherwise, the only indicator uses all the height/width. */
  m_verticalBar.setFrame(KDRect(
    frame.width() - vBarFrameBreadth, 0,
    vBarFrameBreadth, frame.height() - hBarFrameBreadth
  ));
  m_horizontalBar.setFrame(KDRect(
    0, frame.height() - hBarFrameBreadth,
    frame.width() - vBarFrameBreadth, hBarFrameBreadth
  ));
  return frame;
}

ScrollView::ArrowDecorator::ArrowDecorator() :
  m_topArrow(ScrollViewArrow::Side::Top),
  m_rightArrow(ScrollViewArrow::Side::Right),
  m_bottomArrow(ScrollViewArrow::Side::Bottom),
  m_leftArrow(ScrollViewArrow::Side::Left)
{
}

View * ScrollView::ArrowDecorator::indicatorAtIndex(int index) {
  switch(index) {
    case 1:
      return &m_topArrow;
    case 2:
      return &m_rightArrow;
    case 3:
      return &m_bottomArrow;
    default:
      assert(index == 4);
      return &m_leftArrow;
  }
}

KDRect ScrollView::ArrowDecorator::layoutIndicators(KDSize content, KDPoint offset, KDRect frame) {
  KDSize arrowSize = KDFont::LargeFont->glyphSize();
  KDCoordinate topArrowFrameBreadth = arrowSize.height() * m_topArrow.update(0 < offset.y());
  KDCoordinate rightArrowFrameBreadth = arrowSize.width() * m_rightArrow.update(offset.x() + frame.width() < content.width());
  KDCoordinate bottomArrowFrameBreadth = arrowSize.height() * m_bottomArrow.update(offset.y() + frame.height() < content.height());
  KDCoordinate leftArrowFrameBreadth = arrowSize.width() * m_leftArrow.update(0 < offset.x());
  m_topArrow.setFrame(KDRect(
    0, 0,
    frame.width(), topArrowFrameBreadth
  ));
  m_rightArrow.setFrame(KDRect(
    frame.width() - rightArrowFrameBreadth, 0,
    rightArrowFrameBreadth, frame.height()
  ));
  m_bottomArrow.setFrame(KDRect(
    0, frame.height() - bottomArrowFrameBreadth,
    frame.width(), bottomArrowFrameBreadth
  ));
  m_leftArrow.setFrame(KDRect(
    0, 0,
    leftArrowFrameBreadth, frame.height()
  ));
  return KDRect(
    frame.x() + leftArrowFrameBreadth,
    frame.y() + topArrowFrameBreadth,
    frame.width() - leftArrowFrameBreadth - rightArrowFrameBreadth,
    frame.height() - topArrowFrameBreadth - bottomArrowFrameBreadth
  );
}

void ScrollView::ArrowDecorator::setBackgroundColor(KDColor c) {
  for (int index = 1; index <= numberOfIndicators(); index++) {
    static_cast<ScrollViewArrow *>(indicatorAtIndex(index))->setBackgroundColor(c);
  }
}

ScrollView::Decorators::Decorators() {
  /* We need to initiate the Union at construction to avoid destructing an
   * uninitialized object when changing the decorator type. */
  new (this) Decorator();
}

ScrollView::Decorators::~Decorators() {
  activeDecorator()->~Decorator();
}

void ScrollView::Decorators::setActiveDecorator(Decorator::Type t) {
  /* Decorator destructor is virtual so calling ~Decorator() on a Decorator
   * pointer will call the appropriate destructor. */
  activeDecorator()->~Decorator();
  switch (t) {
    case Decorator::Type::Bars:
      new (&m_bars) BarDecorator();
      break;
    case Decorator::Type::Arrows:
      new (&m_arrows) ArrowDecorator();
      break;
    default:
      assert(t == Decorator::Type::None);
      new (&m_none) Decorator();
  }
}

#if ESCHER_VIEW_LOGGING
const char * ScrollView::className() const {
  return "ScrollView";
}

void ScrollView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << (int)contentOffset().x << "," << (int)contentOffset().y << "\"";
}
#endif
