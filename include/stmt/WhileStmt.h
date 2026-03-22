#ifndef WHILE_STMT_H
#define WHILE_STMT_H

#include <memory>

#include "stmt/Stmt.h"
#include "expressions/Expr.h"

class WhileStmt : public Stmt {
public:
  WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> statement);

  virtual void print(std::ostream& builder) const override;

  virtual void accept(Visitor& visitor) const override;

  const Expr* getCondition() const;

  const Stmt* getStatement() const;

private:
  std::unique_ptr<Expr> mCondition;
  std::unique_ptr<Stmt> mStatement;
};

#endif // !WHILE_STMT_H
