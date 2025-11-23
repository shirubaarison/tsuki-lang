#ifndef PRINT_STMT_H
#define PRINT_STMT_H

#include "expressions/Expr.h"
#include "stmt/Stmt.h"
#include <memory>

class PrintStmt : public Stmt {
public:
  PrintStmt(std::unique_ptr<Expr> expr) : mExpr(std::move(expr)) {}

  virtual void print(std::ostream &builder) const override {
    builder << "PRINT ";
    if (mExpr) {
      mExpr->print(builder);
    }
  }

private:
  const std::unique_ptr<Expr> mExpr;
};

#endif // !PRINT_STMT_H
