#ifndef ASSIGN_EXPR_H
#define ASSIGN_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class AssignExpr : public Expr {
public:
  AssignExpr(std::string name, std::unique_ptr<Expr> expr);

  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const std::string& getName() const;

  const Expr *getExpression() const;

private:
  std::string m_name;
  std::unique_ptr<Expr> m_expr;
};

#endif // !ASSIGN_EXPR_H
