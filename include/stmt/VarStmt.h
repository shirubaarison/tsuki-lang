#ifndef VAR_STMT_H
#define VAR_STMT_H

#include "expressions/VarExpr.h"
#include "stmt/Stmt.h"
#include <memory>

class VarStmt : public Stmt {
public:
  VarStmt(const std::string &name, std::unique_ptr<VarExpr> varExpr);

  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor &visitor) const override;

  const std::string &getName() const;

  const VarExpr *getExpr() const;

private:
  const std::string mName;
  std::unique_ptr<VarExpr> mExpr;
};

#endif // !VAR_STMT_H
