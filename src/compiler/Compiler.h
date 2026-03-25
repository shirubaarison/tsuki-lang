#ifndef COMPILER_H
#define COMPILER_H

#include "Visitor.h"
#include "frontend/ast/Stmt.h"
#include "ir/Instruction.h"

#include <memory>

#include <vector>

struct Local {
  std::string name;
  int depth;
};

class Compiler : public Visitor {
private:
  std::vector<Instruction> m_chunk;
  std::vector<std::unique_ptr<Stmt>> m_syntaxTree;
  std::vector<Local> m_locals;
  std::vector<std::string> m_globals;

  int m_localCount = 0;
  int m_scopeDepth = 0;

  size_t emit(OpCode op);
  size_t emit(OpCode op, const Value& value);

  void emitConstant(const Value& value);
  void patchJump(int jumpPos);

  int resolveLocal(const std::string& name);
  int resolveGlobal(const std::string& name);

  void addLocal(const std::string& name);
  void addGlobal(const std::string& name);
  void error(const std::string& name);
  void beginScope();
  void endScope();
public:
  Compiler();
  std::vector<Instruction> compile(std::vector<std::unique_ptr<Stmt>> syntaxTree);

  void visitLiteralExpr(const LiteralExpr* expr) override;
  void visitBinaryExpr(const BinaryExpr* expr) override;
  void visitAssignExpr(const AssignExpr* expr) override;
  void visitBooleanExpr(const BooleanExpr* expr) override;
  void visitGroupingExpr(const GroupingExpr* expr) override;
  void visitNameExpr(const NameExpr* expr) override;
  void visitPrefixExpr(const PrefixExpr* expr) override;
  void visitVarExpr(const VarExpr* expr) override;

  void visitPrintStmt(const PrintStmt* stmt) override;
  void visitBlockStmt(const BlockStmt* stmt)override;
  void visitExprStmt(const ExprStmt* stmt)override;
  void visitIfStmt(const IfStmt* stmt)override;
  void visitWhileStmt(const WhileStmt* stmt)override;
};

class CompilerError : public std::exception {
public:
  CompilerError(std::string m);
  const char* what() const noexcept override;

private:
  std::string m_msg;
};

#endif // !COMPILER_H
