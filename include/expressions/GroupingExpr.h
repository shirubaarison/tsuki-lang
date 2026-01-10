#ifndef GROUPING_EXPR_H
#define GROUPING_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class GroupingExpr : public Expr {
public:
  GroupingExpr(std::unique_ptr<Expr> expr);
  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Expr* getExpr() const;

private:
  std::unique_ptr<Expr> mExpr;
};

#endif // !GROUPING_EXPR_H
