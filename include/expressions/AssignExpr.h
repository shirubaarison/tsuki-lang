#ifndef ASSIGN_EXPR_H
#define ASSIGN_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class AssignExpr : public Expr {
public:
  AssignExpr(std::unique_ptr<Expr> name, std::unique_ptr<Expr> expr);

  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor &visitor) const override;

  const Expr *getName() const;

  const Expr *getExpression() const;

private:
  std::unique_ptr<Expr> mName;
  std::unique_ptr<Expr> mExpr;
};

#endif // !ASSIGN_EXPR_H
