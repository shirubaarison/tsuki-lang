#ifndef LITERAL_EXPR_H
#define LITERAL_EXPR_H

#include "Value.h"
#include "expressions/Expr.h"
#include <ostream>
#include <variant>

class LiteralExpr : public Expr {
public:
  LiteralExpr(Value value) : mValue(value) {}
  virtual void print(std::ostream &builder) const override {
    std::visit(ValuePrinter{}, mValue);
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitLiteralExpr(this);
  }

  const Value value() const { return mValue; }

private:
  const Value mValue;
};

#endif // !LITERAL_EXPR_H
