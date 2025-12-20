#include "expressions/GroupingExpr.h"
#include "Visitor.h"
#include <ostream>

GroupingExpr::GroupingExpr(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}

void GroupingExpr::print(std::ostream &builder) const {
  builder << "(";
  mExpr->print(builder);
  builder << ")";
}

void GroupingExpr::accept(Visitor &visitor) const {
  visitor.visitGroupingExpr(this);
}

const Expr *GroupingExpr::getExpr() const { return mExpr.get(); }
