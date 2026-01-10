#ifndef NAME_EXPR_H
#define NAME_EXPR_H

#include "expressions/Expr.h"
#include <ostream>

class NameExpr : public Expr {
public:
  NameExpr(const std::string& name);

  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const std::string& getName() const;

private:
  const std::string mName;
};

#endif // !NAME_EXPR_H
