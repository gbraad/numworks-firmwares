#include <poincare/complex_argument.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ComplexArgument::type() const {
  return Type::ComplexArgument;
}

Expression * ComplexArgument::clone() const {
  ComplexArgument * a = new ComplexArgument(m_operands, true);
  return a;
}

Expression ComplexArgument::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
  return this;
}

template<typename T>
std::complex<T> ComplexArgument::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::arg(c));
}

}

