#ifndef ASSIGN_EXPR_H
#define ASSIGN_EXPR_H

#include "expressions/Expr.h"
#include <memory>
#include <ostream>
#include <string>

class AssignExpr : public Expr {
public:
  AssignExpr(const std::string &name, std::unique_ptr<Expr> right)
      : mName(name), mRight(std::move(right)) {};

  virtual void print(std::ostream &builder) const override {
    builder << "(" << mName << " = ";
    mRight->print(builder);
    builder << ")";
  }

private:
  const std::string mName;
  const std::unique_ptr<Expr> mRight;
};

#endif // !ASSIGN_EXPR_H
