#ifndef LITERAL_EXPR_H
#define LITERAL_EXPR_H

#include "Value.h"
#include "expressions/Expr.h"
#include <ostream>

class LiteralExpr : public Expr {
public:
  LiteralExpr(Value value);
  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Value& getValue() const;

private:
  const Value mValue;
};

#endif // !LITERAL_EXPR_H
