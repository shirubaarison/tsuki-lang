#ifndef VAR_STMT_H
#define VAR_STMT_H

#include <memory>

#include "expressions/VarExpr.h"
#include "stmt/Stmt.h"

class VarStmt : public Stmt {
public:
  VarStmt(std::unique_ptr<VarExpr> varExpr) : mVarExpr(std::move(varExpr)) {}
  virtual void print(std::ostream &builder) const override {
    if (mVarExpr) {
      mVarExpr->print(builder);
    }
  }

  virtual void accept(Visitor& visitor) const override {
    visitor.visitVarStmt(this);
  }

private:
  const std::unique_ptr<VarExpr> mVarExpr;
};

#endif // !VAR_STMT_H
