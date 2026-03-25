#ifndef LITERAL_EXPR_H
#define LITERAL_EXPR_H

#include <ostream>

#include "frontend/ast/Expr.h"
#include "runtime/value/Value.h"

class LiteralExpr : public Expr {
public:
  LiteralExpr(Value value);
  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Value& getValue() const;

private:
  const Value mValue;
};

#endif // !LITERAL_EXPR_H
