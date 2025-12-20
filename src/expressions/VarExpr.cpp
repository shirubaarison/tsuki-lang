#include "expressions/VarExpr.h"
#include "Visitor.h"
#include <string>

VarExpr::VarExpr(const std::string &name, std::unique_ptr<Expr> rhs)
    : mName(name), rhs(std::move(rhs)) {}

void VarExpr::print(std::ostream &builder) const { builder << mName; }

void VarExpr::accept(Visitor &visitor) const { visitor.visitVarExpr(this); }

const std::string &VarExpr::getName() const { return mName; }
