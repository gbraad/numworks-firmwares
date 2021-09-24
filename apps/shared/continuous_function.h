#ifndef SHARED_CONTINUOUS_FUNCTION_H
#define SHARED_CONTINUOUS_FUNCTION_H

#include "function.h"
#include "range_1D.h"
#include <apps/i18n.h>
#include <poincare/symbol_abstract.h>
#include <poincare/conic.h>
#include <poincare/preferences.h>

// TODO Hugo : Emscripten things

namespace Shared {

class ContinuousFunction : public Function {
public:
  /* ContinuousFunction */

  // Create a record with baseName
  static ContinuousFunction NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName = nullptr);
  // Builder
  ContinuousFunction(Ion::Storage::Record record = Record()) : Function(record) {}

  /* Symbol and Plot types */

  static constexpr size_t k_numberOfSymbolTypes = 3;
  enum class SymbolType : uint8_t {
    Theta = 0,
    T,
    X,
  };

  static constexpr size_t k_numberOfPlotTypes = 14;
  enum class PlotType : uint8_t {
    Polar = 0,
    Parametric,
    // All following types use x as a symbol
    Cartesian,
    Line,
    HorizontalLine,
    // All following types shall never be active in values table
    VerticalLine,
    Inequation,
    Circle,
    Ellipse,
    Parabola,
    Hyperbola,
    Other,
    // All following types shall never be active
    Undefined,
    Unhandled
  };

  static_assert(
    (static_cast<SymbolType>(PlotType::Polar) == SymbolType::Theta &&
      static_cast<SymbolType>(PlotType::Parametric) == SymbolType::T &&
      static_cast<SymbolType>(PlotType::Cartesian) == SymbolType::X),
    "First PlotTypes should match SymbolTypes");
  // Return Message corresponding to SymbolType
  static I18n::Message MessageForSymbolType(SymbolType symbolType);
  // Return ContinuousFunction's PlotType
  PlotType plotType() const { return recordData()->plotType(); }
  // Return ContinuousFunction's SymbolType
  SymbolType symbolType() const;
  // Return ContinuousFunction's PlotType message
  I18n::Message plotTypeMessage() const;

  /* Function */

  // Return Message corresponding to ContinuousFunction's SymbolType
  I18n::Message parameterMessageName() const override { return MessageForSymbolType(symbolType()); }
  // Return CodePoint corresponding to ContinuousFunction's SymbolType
  CodePoint symbol() const override;
  // Insert ContinuousFunction's name and argument in buffer ("f(x)" or "y")
  int nameWithArgument(char * buffer, size_t bufferSize) override;
  // Insert x or y (depending on index) value in buffer
  int printValue(int index, double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) override;

  /* Properties */

  // Wether to draw a dotted or solid line (Strict inequalities).
  bool drawDottedCurve() const;
  // Wether to draw the area "below" the curve or not
  bool drawInferiorArea() const;
  // Wether to draw the area "above" the curve or not
  bool drawSuperiorArea() const;
  // If the ContinuousFunction should be plot with two curves
  bool hasTwoCurves() const override { return m_model.hasTwoCurves(); }
  // If the ContinuousFunction should be considered active in table
  bool isActiveInTable() const { return plotType() < PlotType::VerticalLine && isActive(); }
  // If the ContinuousFunction has x for unknown symbol
  bool isAlongX() const { return symbol() == 'x'; }
  // If the ContinuousFunction is named ("f(x)=...")
  bool isNamed() const;

  /* Detail view */

  // Number of sections to display in the ContinuousFunction's detail menu
  int detailsNumberOfSections() const;
  // Title of given section in ContinuousFunction's detail menu
  I18n::Message detailsTitle(int i) const;
  // Description of given section in ContinuousFunction's detail menu
  I18n::Message detailsDescription(int i) const;
  // Value of given section in ContinuousFunction's detail menu
  double detailsValue(int i) const;

  /* Expression */

  // Expression clone is of the form (exp1) = (exp2)
  // expressionEquation returns (exp1) - (exp2) or (exp2) if isNamed() (reduced ?)
  Poincare::Expression expressionEquation(Poincare::Context * context) const  { return m_model.expressionEquation(this, context); }
  // expressionReduced returns expressionReduced derivative(s)
  Poincare::Expression expressionDerivateReduced(Poincare::Context * context) const { return m_model.expressionDerivateReduced(this, context); }
  Poincare::ExpressionNode::Type equationSymbol() const { return recordData()->equationSymbol(); }
  Poincare::Expression equationExpression(const Ion::Storage::Record * record) const {
    return m_model.originalEquation(record, symbol());
  }
  Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) override;
  void udpateModel(Poincare::Context * context);

  /* Evaluation */

  Poincare::Coordinate2D<double> evaluate2DAtParameter(double t, Poincare::Context * context, int i = 0) const {
    return templatedApproximateAtParameter(t, context, i);
  }
  Poincare::Coordinate2D<float> evaluateXYAtParameter(float t, Poincare::Context * context, int i = 0) const override {
    // TODO Hugo : cache ?
    return privateEvaluateXYAtParameter<float>(t, context, i);
  }
  Poincare::Coordinate2D<double> evaluateXYAtParameter(double t, Poincare::Context * context, int i = 0) const override {
    return privateEvaluateXYAtParameter<double>(t, context, i);
  }

  /* Derivative */

  bool displayDerivative() const;
  void setDisplayDerivative(bool display);
  int derivativeNameWithArgument(char * buffer, size_t bufferSize);
  double approximateDerivative(double x, Poincare::Context * context, int i = 0) const;

  /* tMin and tMax */

  bool shouldClipTRangeToXRange() const override;  // Returns true if the function will not be displayed if t is outside x range.
  float tMin() const override;
  float tMax() const override;
  void setTMin(float tMin);
  void setTMax(float tMax);
  float rangeStep() const override;

  /* Range */

  bool basedOnCostlyAlgorithms(Poincare::Context * context) const override;
  void xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context) const override;
  void yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Poincare::Context * context, bool optimizeRange) const override;

  /* Extremum */

  Poincare::Coordinate2D<double> nextMinimumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextMaximumFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;

  /* Roots */

  Poincare::Coordinate2D<double> nextRootFrom(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) const;
  Poincare::Coordinate2D<double> nextIntersectionFrom(double start, double max, Poincare::Context * context, Poincare::Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin = -INFINITY, double eDomainMax = INFINITY) const;

  /* Integral */

  Poincare::Expression sumBetweenBounds(double start, double end, Poincare::Context * context) const override;

  /* Cache */
  // TODO Hugo : Consider cache
private:
  static constexpr char k_unknownName[2] = {UCodePointUnknown, 0};
  static constexpr char k_ordinateName[2] = "y";
  static constexpr float k_polarParamRangeSearchNumberOfPoints = 100.0f; // This is ad hoc, no special justification
  static constexpr Poincare::Preferences::UnitFormat k_defaultUnitFormat = Poincare::Preferences::UnitFormat::Metric;
  static constexpr Poincare::ExpressionNode::SymbolicComputation k_defaultSymbolicComputation = Poincare::ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol;
  static constexpr size_t k_lineDetailsSections = 3;
  static constexpr size_t k_circleDetailsSections = 3;
  static constexpr size_t k_ellipseDetailsSections = 6;
  static constexpr size_t k_parabolaDetailsSections = 3;
  static constexpr size_t k_hyperbolaDetailsSections = 6;


  // Update ContinuousFunction's PlotType
  void updatePlotType(Poincare::Preferences::AngleUnit angleUnit, Poincare::Context * context);

  // If y coefficient at yDeg in equation is NonNull. Can also compute its sign.
  bool isYCoefficientNonNull(Poincare::Expression equation, int yDeg, Poincare::Context * context, Poincare::ExpressionNode::Sign * YSign = nullptr) const;


  typedef Poincare::Coordinate2D<double> (*ComputePointOfInterest)(Poincare::Expression e, char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep);
  Poincare::Coordinate2D<double> nextPointOfInterestFrom(double start, double max, Poincare::Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const;

  template<typename T> Poincare::Coordinate2D<T> privateEvaluateXYAtParameter(T t, Poincare::Context * context, int i = 0) const;
  template<typename T> Poincare::Coordinate2D<T> templatedApproximateAtParameter(T t, Poincare::Context * context, int i = 0) const;


  // TODO Hugo : Restore cache or remove it
  void didBecomeInactive() override {} // m_cache = nullptr; }

  /* Record */
  // TODO Hugo : Padding
  class __attribute__((packed)) RecordDataBuffer : public Shared::Function::RecordDataBuffer {
  public:
    RecordDataBuffer(KDColor color) : Shared::Function::RecordDataBuffer(color), m_domain(-INFINITY, INFINITY), m_displayDerivative(false), m_plotType(PlotType::Undefined), m_equationSymbol(Poincare::ExpressionNode::Type::Equal)  {}
    PlotType plotType() const { return m_plotType; }
    void setPlotType(PlotType plotType) { m_plotType = plotType; }
    bool displayDerivative() const { return m_displayDerivative; }
    void setDisplayDerivative(bool display) { m_displayDerivative = display; }
    Poincare::ExpressionNode::Type equationSymbol() const { return m_equationSymbol; }
    void setEquationSymbol(Poincare::ExpressionNode::Type equationSymbol) { m_equationSymbol = equationSymbol; }
    // TODO Hugo : Fix Turn on/off menu
    bool isActive() const override { return Shared::Function::RecordDataBuffer::isActive() && m_plotType != PlotType::Unhandled && m_plotType != PlotType::Undefined; }
    float tMin() const { return m_domain.min(); }
    float tMax() const { return m_domain.max(); }
    void setTMin(float tMin) { m_domain.setMin(tMin); }
    void setTMax(float tMax) { m_domain.setMax(tMax); }
    void setConic(Poincare::Conic conic) { m_conic = conic; }
    Poincare::Conic getConic() const { return m_conic; }
    void setLine(double a, double b) { m_lineA = a; m_lineB = b; }
    double getLine(size_t paramIndex) const { assert(paramIndex < 2); return paramIndex == 0 ? m_lineA : m_lineB; }
  private:
    // sizeof - align
    // TODO Hugo : Use Emscripten types for packed alignment
    Poincare::Conic m_conic; // 80 - 8
    double m_lineA; // 8 - 8
    double m_lineB; // 8 - 8
    Range1D m_domain; // 8 - 4
    bool m_displayDerivative; // 1 - 1
    PlotType m_plotType; // 1 - 1
    Poincare::ExpressionNode::Type m_equationSymbol; // 1 - 1
  };
  size_t metaDataSize() const override { return sizeof(RecordDataBuffer); }
  RecordDataBuffer * recordData() const {
    assert(!isNull());
    Ion::Storage::Record::Data d = value();
    return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
  }

  /* Model */

  class Model : public ExpressionModel {
  public:
    Model() : ExpressionModel(), m_hasTwoCurves(false), m_equationSymbol(Poincare::ExpressionNode::Type::Equal), m_plotType(PlotType::Undefined), m_expressionDerivate() {}
    // TODO Hugo : Properly rename these functions
    Poincare::Expression originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const;
    Poincare::Expression expressionEquation(const Ion::Storage::Record * record, Poincare::Context * context) const;
    Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const override;
    Poincare::Expression expressionClone(const Ion::Storage::Record * record) const override;
    Poincare::Expression expressionDerivateReduced(const Ion::Storage::Record * record, Poincare::Context * context) const;
    void tidy() const override;
    bool hasTwoCurves() const { return m_hasTwoCurves; }
  // private:
    void * expressionAddress(const Ion::Storage::Record * record) const override;
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    mutable bool m_hasTwoCurves;
    mutable Poincare::ExpressionNode::Type m_equationSymbol;
    // TODO Hugo : Avoid this
    mutable PlotType m_plotType;
    mutable Poincare::Expression m_expressionDerivate;
  };
  const ExpressionModel * model() const override { return &m_model; }
  Model m_model;
};

}

#endif
