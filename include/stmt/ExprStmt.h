#ifndef EXPR_STMT_H
#define EXPR_STMT_H

#include "expressions/Expr.h"
#include "stmt/Stmt.h"
#include <memory>

class ExprStmt : public Stmt {
public:
  ExprStmt(std::unique_ptr<Expr> expr);

  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Expr* getExpr() const;

private:
  std::unique_ptr<Expr> mExpr;
};

#endif // !EXPR_STMT_H
