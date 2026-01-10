#include "stmt/IfStmt.h"
#include "Visitor.h"
#include "expressions/Expr.h"

IfStmt::IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
               std::unique_ptr<Stmt> elseBranch)
    : mCondition(std::move(condition)), mThenBranch(std::move(thenBranch)),
      mElseBranch(std::move(elseBranch)) {}

void IfStmt::print(std::ostream& builder) const
{
  builder << "if (";
  mCondition->print(builder);
  builder << ")\n";
  mThenBranch->print(builder);
  if (mElseBranch) {
    builder << "else\n";
    mElseBranch->print(builder);
  }
}

void IfStmt::accept(Visitor& visitor) const { visitor.visitIfStmt(this); }

const Expr* IfStmt::getCondition() const { return mCondition.get(); }

const Stmt* IfStmt::getThen() const { return mThenBranch.get(); }

const Stmt* IfStmt::getElse() const { return mElseBranch.get(); }

