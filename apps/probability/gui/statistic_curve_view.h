#ifndef APPS_PROBABILITY_GUI_STATISTIC_CURVE_VIEW_H
#define APPS_PROBABILITY_GUI_STATISTIC_CURVE_VIEW_H

#include <apps/shared/curve_view.h>
#include <escher/palette.h>
#include <poincare/coordinate_2D.h>

#include "probability/models/statistic/statistic.h"

namespace Probability {

class StatisticCurveView : public Shared::CurveView {
public:
  StatisticCurveView(Shared::CurveViewRange * range) : Shared::CurveView(range) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setStatistic(Statistic * statistic) { m_statistic = statistic; }

protected:
  char * label(Axis axis, int index) const override;

private:
  static Poincare::Coordinate2D<float> evaluateAtAbsissa(float x, void * model, void * context) {
    Statistic * statistic = static_cast<Statistic *>(model);
    return Poincare::Coordinate2D<float>(x, statistic->normedDensityFunction(x));
  }
  static constexpr KDColor k_backgroundColor = Escher::Palette::WallScreen;
  char m_labels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
  Statistic * m_statistic;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_STATISTIC_CURVE_VIEW_H */