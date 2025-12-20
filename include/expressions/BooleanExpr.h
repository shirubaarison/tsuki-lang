#ifndef BOOLEAN_EXPR_H
#define BOOLEAN_EXPR_H

#include "expressions/Expr.h"
#include <ostream>
class BooleanExpr : public Expr {
public:
  BooleanExpr(bool value);
  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor& visitor) const override;

  bool getValue() const;

private:
  const bool mValue;
};

#endif // !BOOLEAN_EXPR_H
