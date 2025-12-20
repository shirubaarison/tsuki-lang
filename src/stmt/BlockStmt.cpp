#include "stmt/BlockStmt.h"
#include "Visitor.h"
#include <vector>

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
    : mStatements(std::move(statements)) {}

void BlockStmt::print(std::ostream &builder) const {
  builder << "{\n";
  for (const auto &stmt : mStatements) {
    stmt->print(builder);
  }
  builder << "}\n";
}

void BlockStmt::accept(Visitor &visitor) const { visitor.visitBlockStmt(this); }

const std::vector<std::unique_ptr<Stmt>> &BlockStmt::getStatements() const {
  return mStatements;
}
