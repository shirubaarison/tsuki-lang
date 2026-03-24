#include "expressions/NameExpr.h"
#include "Visitor.h"
#include <string>

NameExpr::NameExpr(std::string name) : m_name(std::move(name)) {}

void NameExpr::print(std::ostream& builder) const
{
  builder << "NameExpr(";
  builder << m_name;
  builder << ")";
}

void NameExpr::accept(Visitor& visitor) const { visitor.visitNameExpr(this); }

const std::string& NameExpr::getName() const { return m_name; }
