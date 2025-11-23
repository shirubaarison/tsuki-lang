#ifndef BLOCK_STMT_H
#define BLOCK_STMT_H

#include "stmt/Stmt.h"
#include <memory>
class BlockStmt : public Stmt {
public: 
  BlockStmt(std::unique_ptr<Stmt> statement) : mStatement(std::move(statement)) {}

  virtual void print(std::ostream &builder) const override {
    if (mStatement) {
      mStatement->print(builder);
    }
  }


private:
  const std::unique_ptr<Stmt> mStatement;
};

#endif // !BLOCK_STMT_H
