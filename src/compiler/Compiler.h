#ifndef COMPILER_H
#define COMPILER_H

#include "frontend/ast/Ast.h"
#include "ir/Instruction.h"

#include <vector>

struct Local {
  std::string name;
  int depth;
};

class Compiler {
private:
  std::vector<Instruction> m_chunk;
  std::vector<Stmt> m_syntaxTree;
  std::vector<Local> m_locals;
  std::vector<std::string> m_globals;

  int m_localCount = 0;
  int m_scopeDepth = 0;

  size_t emit(OpCode op);
  size_t emit(OpCode op, const Value &value);

  void emitConstant(const Value &value);
  void patchJump(int jumpPos);

  int resolveLocal(const std::string &name);
  int resolveGlobal(const std::string &name);

  void addLocal(const std::string &name);
  void addGlobal(const std::string &name);
  void error(const std::string &name);
  void beginScope();
  void endScope();

  void compileExpr(const Expr &expr);
  void compileStmt(const Stmt &stmt);

public:
  Compiler();
  std::vector<Instruction> compile(std::vector<Stmt> syntaxTree);
};

class CompilerError : public std::exception {
public:
  CompilerError(std::string m);
  const char *what() const noexcept override;

private:
  std::string m_msg;
};

#endif // !COMPILER_H
