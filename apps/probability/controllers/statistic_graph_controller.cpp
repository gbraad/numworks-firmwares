#include "statistic_graph_controller.h"

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

StatisticGraphController::StatisticGraphController(StackViewController * stack,
                                                   Statistic * statistic) :
    Page(stack),
    m_rangeLeft(m_graphView.curveViewLeft(), true),
    m_rangeRight(m_graphView.curveViewRight(), false),
    m_graphView(&m_rangeLeft, &m_rangeRight),
    m_statistic(statistic) {
}

const char * StatisticGraphController::title() {
  char zBuffer[10];
  char pBuffer[10];
  defaultParseFloat(m_statistic->testCriticalValue(), zBuffer, sizeof(zBuffer));
  defaultParseFloat(m_statistic->pValue(), pBuffer, sizeof(pBuffer));
  sprintf(m_titleBuffer, "z=%s p-value=%s", zBuffer, pBuffer);
  return m_titleBuffer;
}

void StatisticGraphController::didBecomeFirstResponder() {
  TestConclusionView::Type t = m_statistic->testPassed() ? TestConclusionView::Type::Success
                                                         : TestConclusionView::Type::Failure;
  GraphDisplayMode m = m_statistic->hypothesisParams()->op() ==
                                   HypothesisParams::ComparisonOperator::Different &&
                               App::app()->subapp() == Data::SubApp::Tests
                           ? GraphDisplayMode::TwoCurveViews
                           : GraphDisplayMode::OneCurveView;
  GraphView::LegendPosition pos = m_statistic->hypothesisParams()->op() ==
                                          HypothesisParams::ComparisonOperator::Lower
                                      ? GraphView::LegendPosition::Left
                                      : GraphView::LegendPosition::Right;
  m_graphView.setMode(m);
  m_graphView.setLegendPosition(pos);
  m_graphView.setType(t);
  m_graphView.setStatistic(m_statistic);
  m_rangeLeft.setMode(m);
  m_rangeRight.setMode(m);
  m_rangeLeft.setStatistic(m_statistic);
  m_rangeRight.setStatistic(m_statistic);
  if (App::app()->subapp() == Data::SubApp::Intervals) {
    m_graphView.intervalConclusionView()->setInterval(m_statistic->estimate(),
                                                      m_statistic->marginOfError());
  }
  m_graphView.reload();
}

}  // namespace Probability