#ifndef PRINT_STMT_H
#define PRINT_STMT_H

#include <memory>

#include "frontend/ast/Expr.h"
#include "frontend/ast/Stmt.h"

class PrintStmt : public Stmt {
public:
  PrintStmt(std::unique_ptr<Expr> expr);
  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Expr* getExpr() const;

private:
  std::unique_ptr<Expr> mExpr;
};

#endif // !PRINT_STMT_H
