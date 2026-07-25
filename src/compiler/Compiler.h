#ifndef COMPILER_H
#define COMPILER_H

#include "compiler/SymbolTable.h"
#include "frontend/ast/Ast.h"
#include "ir/Chunk.h"

#include <vector>

class Compiler {
public:
  Compiler();
  Chunk compile(std::vector<Stmt> syntaxTree);

private:
  Chunk m_chunk;
  SymbolTable symbolTable;
  std::vector<Stmt> m_syntaxTree;

  size_t emit(OpCode op);
  size_t emit(OpCode op, const Value& value);

  void emitConstant(const Value& value);
  size_t emitJump(OpCode op);
  void patchJump(int jumpPos);

  void compileExpr(const Expr& expr);
  void compileExpr(const BinaryExpr& expr);
  void compileExpr(const AssignExpr& expr);
  void compileExpr(const GroupingExpr& expr);
  void compileExpr(const PrefixExpr& expr);
  void compileExpr(const LiteralExpr& expr);
  void compileExpr(const BooleanExpr& expr);
  void compileExpr(const NameExpr& expr);
  void compileExpr(const VarExpr& expr);

  void compileStmt(const Stmt& stmt);
  void compileStmt(const PrintStmt& stmt);
  void compileStmt(const BlockStmt& stmt);
  void compileStmt(const IfStmt& stmt);
  void compileStmt(const ExprStmt& stmt);
  void compileStmt(const WhileStmt& stmt);
};

class CompilerError : public std::exception {
public:
  CompilerError(std::string m);
  const char *what() const noexcept override;

private:
  std::string m_msg;
};

#endif // !COMPILER_H
