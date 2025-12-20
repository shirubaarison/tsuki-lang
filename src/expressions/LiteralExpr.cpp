#include "expressions/LiteralExpr.h"
#include "Value.h"
#include "Visitor.h"

LiteralExpr::LiteralExpr(Value value) : mValue(value) {}
void LiteralExpr::print(std::ostream &builder) const {
  std::visit(ValuePrinter{}, mValue);
}

void LiteralExpr::accept(Visitor &visitor) const {
  visitor.visitLiteralExpr(this);
}

const Value &LiteralExpr::getValue() const { return mValue; }
