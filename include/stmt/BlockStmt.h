#ifndef BLOCK_STMT_H
#define BLOCK_STMT_H

#include "stmt/Stmt.h"
#include <memory>
#include <vector>

class BlockStmt : public Stmt {
public:
  BlockStmt(std::vector<std::unique_ptr<Stmt>> statements);

  virtual void print(std::ostream &builder) const override;

  virtual void accept(Visitor &visitor) const override;

  const std::vector<std::unique_ptr<Stmt>> &getStatements() const;

private:
  std::vector<std::unique_ptr<Stmt>> mStatements;
};

#endif // !BLOCK_STMT_H
