#ifndef BOOLEAN_EXPR_H
#define BOOLEAN_EXPR_H

#include "expressions/Expr.h"
#include <ostream>
class BooleanExpr : public Expr {
public:
  BooleanExpr(bool value) : mValue(value) {}
  virtual void print(std::ostream &builder) const override {
    std::string val = mValue ? "true" : "false";
    builder << val;
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitBooleanExpr(this);
  }

private:
  bool mValue;
};

#endif // !BOOLEAN_EXPR_H
