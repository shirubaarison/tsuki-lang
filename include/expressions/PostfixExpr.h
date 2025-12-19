#ifndef POSTFIX_EXPR_H
#define POSTFIX_EXPR_H

#include "Lexer.h"
#include "TokenType.h"
#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class PostfixExpr : public Expr {
public:
  PostfixExpr(std::unique_ptr<Expr> left, TokenType operatorType)
      : mleft(std::move(left)), mOperator(operatorType) {};

  virtual void print(std::ostream &builder) const override {
    builder << "(";
    mleft->print(builder);
    builder << tokenTypeToString(mOperator) << ")";
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitPostfixExpr(this);
  }

private:
  const std::unique_ptr<Expr> mleft;
  const TokenType mOperator;
};

#endif // !POSTFIX_EXPR_H
