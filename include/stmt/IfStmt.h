#ifndef IF_STMT_H
#define IF_STMT_H

#include <memory>

#include "stmt/Stmt.h"
#include "expressions/Expr.h"

class IfStmt : public Stmt {
public:
  IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
         std::unique_ptr<Stmt> elseBranch)
      : mCondition(std::move(condition)), mThenBranch(std::move(thenBranch)),
        mElseBranch(std::move(elseBranch)) {}
  virtual void print(std::ostream &builder) const override {
    builder << "if (";
    if (mCondition) {
      mCondition->print(builder);
    }
    builder << ") then { ";

    if (mThenBranch) {
      mThenBranch->print(builder);
    }

    builder << " }";

    if (mElseBranch) {
      builder << " else {";
      mElseBranch->print(builder);
      builder << " }";
    }
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitIfStmt(this);
  }

private:
  const std::unique_ptr<Expr> mCondition;
  const std::unique_ptr<Stmt> mThenBranch;
  const std::unique_ptr<Stmt> mElseBranch;
};

#endif // !IF_STMT_H
