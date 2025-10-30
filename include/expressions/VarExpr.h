#ifndef VAR_EXPR_H
#define VAR_EXPR_H

#include "expressions/Expr.h"
#include <ostream>

class VarExpr : public Expr {
public:
  VarExpr(const std::string name) : mName(name) {}
  virtual void print(std::ostream &builder) const override { builder << mName; }

private:
  const std::string mName;
};

#endif // !VAR_EXPR_H
