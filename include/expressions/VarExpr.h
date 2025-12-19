#ifndef VAR_EXPR_H
#define VAR_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>

class VarExpr : public Expr {
public:
  VarExpr(const std::string name, std::unique_ptr<Expr> value)
      : mName(name), mValue(std::move(value)) {}
  virtual void print(std::ostream &builder) const override {
    builder << "var:" << mName << " = ";
    if (mValue) {
      mValue->print(builder);
    } else {
      builder << "nil";
    }
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitVarExpr(this);
  }

private:
  const std::string mName;
  const std::unique_ptr<Expr> mValue;
};

#endif // !VAR_EXPR_H
