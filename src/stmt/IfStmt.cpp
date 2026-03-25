#include "stmt/IfStmt.h"
#include "Visitor.h"
#include "frontend/ast/Expr.h"

IfStmt::IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
               std::unique_ptr<Stmt> elseBranch)
    : mCondition(std::move(condition)), mThenBranch(std::move(thenBranch)),
      mElseBranch(std::move(elseBranch)) {}

void IfStmt::print(std::ostream& builder) const
{
  builder << "ifStmt(";
  mCondition->print(builder);
  builder << ")\n";
  builder << "{";
  mThenBranch->print(builder);
  builder << "}";
  if (mElseBranch) {
    builder << "elseBranch(\n";
    mElseBranch->print(builder);
    builder << ")";
  }
}

void IfStmt::accept(Visitor& visitor) const { visitor.visitIfStmt(this); }

const Expr* IfStmt::getCondition() const { return mCondition.get(); }

const Stmt* IfStmt::getThen() const { return mThenBranch.get(); }

const Stmt* IfStmt::getElse() const { return mElseBranch.get(); }

