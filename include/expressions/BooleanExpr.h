#ifndef BOOLEAN_EXPR_H
#define BOOLEAN_EXPR_H

#include "Value.h"
#include "expressions/Expr.h"
#include <ostream>
class BooleanExpr : public Expr {
public:
  BooleanExpr(Value value);
  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor& visitor) const override;

  bool getValue() const;

private:
  const Value mValue;
};

#endif // !BOOLEAN_EXPR_H
