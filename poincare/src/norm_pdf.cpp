#include <poincare/norm_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormPDF::s_functionHelper;

int NormPDFNode::numberOfChildren() const { return NormPDF::s_functionHelper.numberOfChildren(); }

Expression NormPDFNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == sign(reductionContext.context()));
  return NormPDF(this);
}

Layout NormPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormPDF(this), floatDisplayMode, numberOfSignificantDigits, NormPDF::s_functionHelper.name());
}

int NormPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormPDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> muEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> sigmaEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  T x = xEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T sigma = sigmaEvaluation.toScalar();

  // EvaluateAtAbscissa handles bad mu and var values
  return Complex<T>::Builder(NormalDistribution::EvaluateAtAbscissa(x, mu, sigma));
}

}
