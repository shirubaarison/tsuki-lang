#include "stmt/ExprStmt.h"
#include "Visitor.h"
#include <ostream>

ExprStmt::ExprStmt(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}

void ExprStmt::print(std::ostream& builder) const 
{
  if (mExpr) {
    builder << "ExprStmt(";
    mExpr->print(builder);
    builder << ")";
  }
}

void ExprStmt::accept(Visitor& visitor) const { visitor.visitExprStmt(this); }

const Expr* ExprStmt::getExpr() const { return mExpr.get(); }
