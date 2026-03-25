#include "expressions/LiteralExpr.h"
#include "runtime/value/Value.h"
#include "Visitor.h"

LiteralExpr::LiteralExpr(Value value) : mValue(value) {}

void LiteralExpr::print(std::ostream& builder) const
{
  builder << "LiteralExpr(";
  std::visit(ValuePrinter{}, mValue);
  builder << ")";
}

void LiteralExpr::accept(Visitor& visitor) const { visitor.visitLiteralExpr(this); }

const Value& LiteralExpr::getValue() const { return mValue; }
