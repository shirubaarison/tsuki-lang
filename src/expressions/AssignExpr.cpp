#include "expressions/AssignExpr.h"
#include "Visitor.h"
#include <ostream>

AssignExpr::AssignExpr(std::string name, std::unique_ptr<Expr> expr)
    : m_name(name), mExpr(std::move(expr)) {}

void AssignExpr::print(std::ostream& builder) const 
{
  builder << "AssignExpr(";
  builder << m_name << " : ";
  mExpr->print(builder);
  builder << ")";
}

void AssignExpr::accept(Visitor& visitor) const { visitor.visitAssignExpr(this); }

const std::string& AssignExpr::getName() const { return m_name; }

const Expr* AssignExpr::getExpression() const { return mExpr.get(); }
