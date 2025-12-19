#ifndef BINARY_EXPR_H
#define BINARY_EXPR_H

#include <memory>
#include <ostream>
#include <utility>

#include "Lexer.h"
#include "TokenType.h"
#include "expressions/Expr.h"

class BinaryExpr : public Expr {
public:
  BinaryExpr(std::unique_ptr<Expr> left, TokenType operatorType,
             std::unique_ptr<Expr> right)
      : mLeft(std::move(left)), mOperator(operatorType),
        mRight(std::move(right)) {};

  virtual void print(std::ostream &builder) const override {
    builder << "(";
    mLeft->print(builder);
    builder << " " << tokenToOperator(mOperator) << " ";
    mRight->print(builder);
    builder << ")";
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitBinaryExpr(this);
  }

  const Expr& left() const { return *mLeft; }
  const Expr& right() const { return *mRight; }
  TokenType operatorType() const { return mOperator; }

private:
  const std::unique_ptr<Expr> mLeft;
  const TokenType mOperator;
  const std::unique_ptr<Expr> mRight;
};

#endif // !BINARY_EXPR_H
