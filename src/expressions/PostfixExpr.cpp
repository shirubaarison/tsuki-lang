#include "expressions/PostfixExpr.h"
#include "TokenType.h"
#include "Visitor.h"

PostfixExpr::PostfixExpr(std::unique_ptr<Expr> expr, TokenType op)
    : mExpr(std::move(expr)), mOp(op) {}

void PostfixExpr::print(std::ostream& builder) const
{
  mExpr->print(builder);
  builder << tokenToOperator(mOp);
}

void PostfixExpr::accept(Visitor& visitor) const { visitor.visitPostfixExpr(this); }

const Expr* PostfixExpr::getExpr() const { return mExpr.get(); }

TokenType PostfixExpr::getOp() const { return mOp; }
