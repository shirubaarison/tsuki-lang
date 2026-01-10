#include "stmt/ExprStmt.h"
#include "Visitor.h"
#include <ostream>

ExprStmt::ExprStmt(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}

void ExprStmt::print(std::ostream& builder) const 
{
  if (mExpr) {
    mExpr->print(builder);
  }
}

void ExprStmt::accept(Visitor& visitor) const { visitor.visitExprStmt(this); }

const Expr* ExprStmt::getExpr() const { return mExpr.get(); }
