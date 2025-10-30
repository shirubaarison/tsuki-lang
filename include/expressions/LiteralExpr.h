#ifndef LITERAL_EXPR_H
#define LITERAL_EXPR_H

#include "expressions/Expr.h"
#include <ostream>

class LiteralExpr : public Expr {
public:
  LiteralExpr(std::string value) : mValue(value) {}
  virtual void print(std::ostream &builder) const override {
    builder << mValue;
  }

private:
  const std::string mValue;
};

#endif // !LITERAL_EXPR_H
