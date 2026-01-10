#include "stmt/PrintStmt.h"
#include "Visitor.h"
#include "expressions/Expr.h"

PrintStmt::PrintStmt(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}

void PrintStmt::print(std::ostream& builder) const
{
  builder << "print ";
  mExpr->print(builder);
  builder << ";\n";
}

void PrintStmt::accept(Visitor& visitor) const { visitor.visitPrintStmt(this); }

const Expr* PrintStmt::getExpr() const { return mExpr.get(); }

