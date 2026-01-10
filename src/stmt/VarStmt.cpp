#include "stmt/VarStmt.h"
#include "Visitor.h"
#include <string>

VarStmt::VarStmt(const std::string& name, std::unique_ptr<VarExpr> varExpr)
    : mName(name), mExpr(std::move(varExpr)) {}

void VarStmt::print(std::ostream& builder) const
{
  builder << "var " << mName;
  if (mExpr) {
    builder << " = ";
    mExpr->print(builder);
  }
  builder << ";\n";
}

void VarStmt::accept(Visitor& visitor) const { visitor.visitVarStmt(this); }

const std::string& VarStmt::getName() const { return mName; }

const VarExpr* VarStmt::getExpr() const { return mExpr.get(); }
