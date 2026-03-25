#ifndef BOOLEAN_EXPR_H
#define BOOLEAN_EXPR_H

#include <ostream>

#include "runtime/value/Value.h"
#include "frontend/ast/Expr.h"

class BooleanExpr : public Expr {
public:
  BooleanExpr(Value value);
  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  bool getValue() const;

private:
  const Value mValue;
};

#endif // !BOOLEAN_EXPR_H
