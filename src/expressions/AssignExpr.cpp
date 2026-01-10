#include "expressions/AssignExpr.h"
#include "Visitor.h"
#include <ostream>

AssignExpr::AssignExpr(std::unique_ptr<Expr> name, std::unique_ptr<Expr> expr)
    : mName(std::move(name)), mExpr(std::move(expr)) {}

void AssignExpr::print(std::ostream &builder) const {
  mExpr->print(builder);
}

void AssignExpr::accept(Visitor &visitor) const {
  visitor.visitAssignExpr(this);
}

const Expr *AssignExpr::getName() const { return mName.get(); }

const Expr *AssignExpr::getExpression() const { return mExpr.get(); }
