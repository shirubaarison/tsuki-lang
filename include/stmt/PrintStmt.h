#ifndef PRINT_STMT_H
#define PRINT_STMT_H

#include "expressions/Expr.h"
#include "stmt/Stmt.h"
#include <memory>

class PrintStmt : public Stmt {
public:
  PrintStmt(std::unique_ptr<Expr> expr);
  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor &visitor) const override;

  const Expr* getExpr() const;

private:
  std::unique_ptr<Expr> mExpr;
};

#endif // !PRINT_STMT_H
