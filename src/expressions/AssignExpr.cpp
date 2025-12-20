#include "expressions/AssignExpr.h"
#include "Visitor.h"
#include <ostream>
#include <string>

AssignExpr::AssignExpr(const std::string &name, std::unique_ptr<Expr> expr)
    : mName(name), mExpr(std::move(expr)) {}

void AssignExpr::print(std::ostream &builder) const {
  builder << mName << " = ";
  mExpr->print(builder);
}

void AssignExpr::accept(Visitor &visitor) const {
  visitor.visitAssignExpr(this);
}

const std::string &AssignExpr::name() const { return mName; }

const Expr *AssignExpr::getExpression() const { return mExpr.get(); }
