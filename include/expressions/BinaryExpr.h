#ifndef BINARY_EXPR_H
#define BINARY_EXPR_H

#include <memory>
#include <ostream>

#include "TokenType.h"
#include "expressions/Expr.h"

class BinaryExpr : public Expr {
public:
  BinaryExpr(std::unique_ptr<Expr> left, TokenType operatorType,
             std::unique_ptr<Expr> right);

  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Expr *getLeft() const;
  const Expr *getRight() const;
  TokenType getOperatorType() const;

private:
  std::unique_ptr<Expr> mLeft;
  const TokenType mOperator;
  std::unique_ptr<Expr> mRight;
};

#endif // !BINARY_EXPR_H
