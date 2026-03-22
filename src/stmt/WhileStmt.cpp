#include "stmt/WhileStmt.h"
#include "Visitor.h"
#include "expressions/Expr.h"

WhileStmt::WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> statement)
    : mCondition(std::move(condition)), mStatement(std::move(statement)) {}

void WhileStmt::print(std::ostream& builder) const
{
  builder << "WhileStmt(";
  mCondition->print(builder);
  builder << ") {";
  mStatement->print(builder);
  builder << "}";
}

void WhileStmt::accept(Visitor& visitor) const { visitor.visitWhileStmt(this); }

const Expr* WhileStmt::getCondition() const { return mCondition.get(); }

const Stmt* WhileStmt::getStatement() const { return mStatement.get(); }

