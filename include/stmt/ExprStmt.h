#ifndef EXPR_STMT_H
#define EXPR_STMT_H

#include "expressions/Expr.h"
#include "stmt/Stmt.h"
#include <memory>
class ExprStmt : public Stmt {
public:
  ExprStmt(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}

  virtual void print(std::ostream &builder) const override {
    if (mExpr) {
      mExpr->print(builder);
    }
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitExprStmt(this);
  }

  const Expr& expression() const {
    return *mExpr;
  }

private:
  const std::unique_ptr<Expr> mExpr;
};

#endif // !EXPR_STMT_H
