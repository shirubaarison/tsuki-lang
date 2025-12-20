#include "expressions/BooleanExpr.h"
#include "Visitor.h"

BooleanExpr::BooleanExpr(bool value) : mValue(value) {}
void BooleanExpr::print(std::ostream &builder) const {
  builder << (mValue ? "true" : "false");
}

void BooleanExpr::accept(Visitor &visitor) const {
  visitor.visitBooleanExpr(this);
}

bool BooleanExpr::getValue() const { return mValue; }
