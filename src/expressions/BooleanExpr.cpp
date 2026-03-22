#include "expressions/BooleanExpr.h"
#include "Visitor.h"

BooleanExpr::BooleanExpr(Value value) : mValue(value) {}

void BooleanExpr::print(std::ostream& builder) const
{
  builder << "BooleanExpr(";
  builder << (std::get<bool>(mValue) ? "true" : "false");
  builder << ")";
}

void BooleanExpr::accept(Visitor& visitor) const { visitor.visitBooleanExpr(this); }

bool BooleanExpr::getValue() const
{
  return std::get<bool>(mValue) ? true : false;
}
