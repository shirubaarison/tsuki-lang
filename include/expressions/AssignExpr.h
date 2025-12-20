#ifndef ASSIGN_EXPR_H
#define ASSIGN_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>
#include <string>

class AssignExpr : public Expr {
public:
  AssignExpr(const std::string &name, std::unique_ptr<Expr> expr);

  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor &visitor) const override;

  const std::string &name() const;

  const Expr *getExpression() const;

private:
  const std::string mName;
  std::unique_ptr<Expr> mExpr;
};

#endif // !ASSIGN_EXPR_H
