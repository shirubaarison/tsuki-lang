#ifndef PREFIX_EXPR_H
#define PREFIX_EXPR_H

#include "Lexer.h"
#include "TokenType.h"
#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class PrefixExpr : public Expr {
public:
  PrefixExpr(TokenType operatorType, std::unique_ptr<Expr> right)
      : mOperator(operatorType), mRight(std::move(right)) {};

  virtual void print(std::ostream &builder) const override {
    builder << "(" << tokenToOperator(mOperator);
    mRight->print(builder);
    builder << ")";
  }

private:
  const TokenType mOperator;
  const std::unique_ptr<Expr> mRight;
};

#endif // !PREFIX_EXPR_H
