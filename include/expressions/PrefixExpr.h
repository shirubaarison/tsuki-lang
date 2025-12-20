#ifndef PREFIX_EXPR_H
#define PREFIX_EXPR_H

#include "TokenType.h"
#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class PrefixExpr : public Expr {
public:
  PrefixExpr(TokenType op, std::unique_ptr<Expr> expr);
  void print(std::ostream &builder) const override;

  void accept(Visitor &visitor) const override;

  const Expr *getExpr() const;

  TokenType getOp() const;

private:
  const TokenType mOp;
  std::unique_ptr<Expr> mExpr;
};

#endif // !PREFIX_EXPR_H
