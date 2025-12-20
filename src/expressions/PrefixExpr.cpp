#include "expressions/PrefixExpr.h"
#include "TokenType.h"
#include "Visitor.h"

PrefixExpr::PrefixExpr(TokenType op, std::unique_ptr<Expr> expr)
    : mOp(op), mExpr(std::move(expr)) {}
void PrefixExpr::print(std::ostream &builder) const {
  builder << tokenToOperator(mOp);
  mExpr->print(builder);
}

void PrefixExpr::accept(Visitor &visitor) const {
  visitor.visitPrefixExpr(this);
}

const Expr *PrefixExpr::getExpr() const { return mExpr.get(); }

TokenType PrefixExpr::getOp() const { return mOp; }
