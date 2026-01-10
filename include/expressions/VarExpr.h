#ifndef VAR_EXPR_H
#define VAR_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class VarExpr : public Expr {
public:
  VarExpr(const std::string& name, std::unique_ptr<Expr> rhs);
  void print(std::ostream& builder) const override;

  void accept(Visitor& visitor) const override;

  const std::string& getName() const;
  const Expr* getExpression() const;

private:
  const std::string mName;
  std::unique_ptr<Expr> rhs;
};

#endif // !VAR_EXPR_H
