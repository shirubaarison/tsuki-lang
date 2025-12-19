#ifndef GROUPING_EXPR_H
#define GROUPING_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class GroupingExpr : public Expr {
public:
  GroupingExpr(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}
  virtual void print(std::ostream &builder) const override {
    builder << "(";
    mExpr->print(builder);
    builder << ")";
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitGroupingExpr(this);
  }


private:
  const std::unique_ptr<Expr> mExpr;
};

#endif // !GROUPING_EXPR_H
