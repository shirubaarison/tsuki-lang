#include "expressions/NameExpr.h"
#include "Visitor.h"
#include <string>

NameExpr::NameExpr(const std::string& name) : mName(name) {}

void NameExpr::print(std::ostream& builder) const { builder << mName; }

void NameExpr::accept(Visitor& visitor) const { visitor.visitNameExpr(this); }

const std::string& NameExpr::getName() const { return mName; }
