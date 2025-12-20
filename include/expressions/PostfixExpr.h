#ifndef POSTFIX_EXPR_H
#define POSTFIX_EXPR_H

#include "TokenType.h"
#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class PostfixExpr : public Expr {
public:
  PostfixExpr(std::unique_ptr<Expr> expr, TokenType op);

  void print(std::ostream &builder) const override;

  void accept(Visitor &visitor) const override;

  const Expr *getExpr() const;

  TokenType getOp() const;

private:
  std::unique_ptr<Expr> mExpr;
  const TokenType mOp;
};

#endif // !POSTFIX_EXPR_H
