#ifndef IF_STMT_H
#define IF_STMT_H

#include <memory>

#include "stmt/Stmt.h"
#include "expressions/Expr.h"

class IfStmt : public Stmt {
public:
  IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
         std::unique_ptr<Stmt> elseBranch);

  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor &visitor) const override;

  const Expr* getCondition() const;

  const Stmt* getThen() const;

  const Stmt* getElse() const;

private:
  std::unique_ptr<Expr> mCondition;
  std::unique_ptr<Stmt> mThenBranch;
  std::unique_ptr<Stmt> mElseBranch;
};

#endif // !IF_STMT_H
