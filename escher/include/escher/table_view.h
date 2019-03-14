#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/palette.h>
#include <escher/scroll_view.h>
#include <escher/highlight_cell.h>
#include <escher/table_view_data_source.h>

class TableView : public ScrollView {
public:
  TableView(TableViewDataSource * dataSource, ScrollViewDataSource * scrollDataSource);

  void setHorizontalCellOverlap(KDCoordinate o) { m_contentView.setHorizontalCellOverlap(o); }
  void setVerticalCellOverlap(KDCoordinate o) { m_contentView.setVerticalCellOverlap(o); }
  int firstDisplayedRowIndex() const { return m_contentView.rowsScrollingOffset(); }
  int firstDisplayedColumnIndex() const { return m_contentView.columnsScrollingOffset(); }
  int numberOfDisplayableRows() const { return m_contentView.numberOfDisplayableRows(); }
  int numberOfDisplayableColumns() const { return m_contentView.numberOfDisplayableColumns(); }

  virtual void scrollToCell(int i, int j);
  HighlightCell * cellAtLocation(int i, int j);
  void reloadCellAtLocation(int i, int j);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  TableViewDataSource * dataSource();
  void layoutSubviews() override;
  class ContentView : public View {
  public:
    ContentView(TableView * tableView, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlap, KDCoordinate verticalCellOverlap);
    KDSize minimalSizeForOptimalDisplay() const override;

    void setHorizontalCellOverlap(KDCoordinate o) { m_horizontalCellOverlap = o; }
    void setVerticalCellOverlap(KDCoordinate o) { m_verticalCellOverlap = o; }

    void scrollToCell(int i, int j) const;
    void reloadCellAtLocation(int i, int j);
    HighlightCell * cellAtLocation(int i, int j);
    TableViewDataSource * dataSource();
    int rowsScrollingOffset() const;
    int columnsScrollingOffset() const;
    int numberOfDisplayableRows() const;
    int numberOfDisplayableColumns() const;
    void layoutSubviews() override;
  protected:
#if ESCHER_VIEW_LOGGING
    const char * className() const override;
#endif
  private:
    KDCoordinate height() const;
    KDCoordinate width() const;

    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;

    /* realCellWidth enables to handle list view for which
     * TableViewDataSource->cellWidht = 0 */
    KDRect cellFrame(int i, int j) const;
    /* These two methods transform an index (of subview for instance) into
     * coordinates that refer to the data source entire table */
    int absoluteColumnNumberFromSubviewIndex(int index) const;
    int absoluteRowNumberFromSubviewIndex(int index) const;
    int numberOfFullyDisplayableRows() const;
    int numberOfFullyDisplayableColumns() const;
    int typeOfSubviewAtIndex(int index) const;
    /* This method transform a index (of subview for instance) into an index
     * refering to the set of cells of type "type". */
    int typeIndexFromSubviewIndex(int index, int type) const;
    TableView * m_tableView;
    TableViewDataSource * m_dataSource;
    KDCoordinate m_horizontalCellOverlap;
    KDCoordinate m_verticalCellOverlap;
  };
  ContentView m_contentView;
};

#endif
