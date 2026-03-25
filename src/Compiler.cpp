#include "Compiler.h"

#include "TokenType.h"
#include "VM/VM.h"
#include "Value.h"
#include "Visitor.h"

#include "expressions/BinaryExpr.h"
#include "expressions/BooleanExpr.h"
#include "expressions/GroupingExpr.h"
#include "expressions/LiteralExpr.h"
#include "expressions/PrefixExpr.h"
#include "expressions/AssignExpr.h"
#include "expressions/VarExpr.h"
#include "expressions/NameExpr.h"

#include "stmt/ExprStmt.h"
#include "stmt/PrintStmt.h"
#include "stmt/BlockStmt.h"
#include "stmt/IfStmt.h"
#include "stmt/WhileStmt.h"

#include <cstddef>
#include <memory>
#include <vector>

Compiler::Compiler() {}

CompilerError::CompilerError(std::string m) : m_msg(std::move(m)) {}

const char *CompilerError::what() const noexcept { return m_msg.c_str(); }

void Compiler::error(const std::string& msg)
{
  std::cerr << "CompilerError: " << msg << std::endl;
}

std::vector<Instruction> Compiler::compile(std::vector<std::unique_ptr<Stmt>> syntaxTree)
{
  m_chunk.clear();
  m_syntaxTree = std::move(syntaxTree);

  try
  {
    for (const auto& stmt : m_syntaxTree)
      if (stmt) stmt->accept(*this);
  }
  catch (const CompilerError& compilerError)
  {
    error(compilerError.what());
  }

  m_chunk.push_back({OpCode::RET, std::monostate{}});

  return m_chunk;
}

size_t Compiler::emit(OpCode op)
{
  m_chunk.push_back(Instruction{op, Value{}});
  return m_chunk.size() - 1;
}

size_t Compiler::emit(OpCode op, const Value& value)
{
  m_chunk.push_back(Instruction{op, value});
  return m_chunk.size() - 1;
}

void Compiler::emitConstant(const Value& value)
{
  m_chunk.push_back(Instruction{OpCode::LOAD_CONSTANT, value});
}

void Compiler::visitLiteralExpr(const LiteralExpr* expr)
{
  emitConstant(expr->getValue());
}

void Compiler::visitBinaryExpr(const BinaryExpr* expr)
{
  if (expr->getOperatorType() == TokenType::TOKEN_AND) {
    expr->getLeft()->accept(*this);

    int endJump = emit(OpCode::JMP_IF_FALSE);
    emit(OpCode::POP);

    expr->getRight()->accept(*this);

    patchJump(endJump);
    return;
  }

  if (expr->getOperatorType() == TokenType::TOKEN_OR) {
    expr->getLeft()->accept(*this);

    int elseJump = emit(OpCode::JMP_IF_FALSE);
    int endJump  = emit(OpCode::JMP);

    patchJump(elseJump);
    emit(OpCode::POP);

    expr->getRight()->accept(*this);

    patchJump(endJump);
    return;
  }

  expr->getLeft()->accept(*this);
  expr->getRight()->accept(*this);

  switch (expr->getOperatorType()) {
    case TokenType::TOKEN_PLUS:          emit(OpCode::ADD); break;
    case TokenType::TOKEN_MINUS:         emit(OpCode::SUB); break;
    case TokenType::TOKEN_STAR:          emit(OpCode::MUL); break;
    case TokenType::TOKEN_SLASH:         emit(OpCode::DIV); break;
    case TokenType::TOKEN_GREATER:       emit(OpCode::GREATER); break;
    case TokenType::TOKEN_GREATER_EQUAL: emit(OpCode::GREATER_EQUAL); break;
    case TokenType::TOKEN_EQUAL_EQUAL:   emit(OpCode::EQUAL); break;
    case TokenType::TOKEN_BANG_EQUAL:    emit(OpCode::NOT_EQUAL); break;
    case TokenType::TOKEN_LESS:          emit(OpCode::LESS); break;
    case TokenType::TOKEN_LESS_EQUAL:    emit(OpCode::LESS_EQUAL); break;
    default: break;
  }
}

void Compiler::visitAssignExpr(const AssignExpr* expr)
{
  const auto& name = expr->getName();
  expr->getExpression()->accept(*this);

  if (m_scopeDepth == 0)
  {
    if (resolveGlobal(name) == -1)
    {
      addGlobal(name);
      emit(OpCode::DEFINE_GLOBAL, name);
    }
    else
    {
      emit(OpCode::SET_GLOBAL, name);
    }
  }
  else
  {
    int localSlot = resolveLocal(name);
    if (localSlot == -1)
    {
      if (resolveGlobal(name) == -1)
      {
        addLocal(name);
        emit(OpCode::DEFINE_LOCAL);
      }
      else
      {
        emit(OpCode::SET_GLOBAL, name);
      }
    }
    else
    {
      emit(OpCode::SET_LOCAL, localSlot);
    }
  }
}

void Compiler::visitBooleanExpr(const BooleanExpr* expr)
{
  if (expr->getValue()) {
    emit(OpCode::TRUE);
  } else {
    emit(OpCode::FALSE);
  }
}

int Compiler::resolveLocal(const std::string& name) {
  for (int i = m_locals.size() - 1; i >= 0; --i)
  {
    if (m_locals[i].name == name)
      return i;
  }

  return -1;
}

int Compiler::resolveGlobal(const std::string& name)
{
  for (size_t i = 0; i < m_globals.size(); ++i)
  {
    if (m_globals[i] == name)
      return i;
  }

  return -1;
}

void Compiler::addLocal(const std::string& name)
{
  Local local = {name, m_scopeDepth};
  m_locals.push_back(local);
}

void Compiler::addGlobal(const std::string& name)
{
  m_globals.push_back(name);
}

void Compiler::visitGroupingExpr(const GroupingExpr* expr)
{
  expr->getExpr()->accept(*this);
}

void Compiler::visitNameExpr(const NameExpr* expr)
{
  auto name = expr->getName();
  int localSlot = resolveLocal(name);
  if (localSlot != -1)
  {
    emit(OpCode::GET_LOCAL, localSlot);
    return;
  }
  if (resolveGlobal(name) == -1)
    throw CompilerError("Undefined variable '" + name + "'");

  emit(OpCode::GET_GLOBAL, name);
}

void Compiler::visitPrefixExpr(const PrefixExpr*) {}

void Compiler::visitVarExpr(const VarExpr* expr)
{
  expr->getExpression()->accept(*this);
}

void Compiler::visitPrintStmt(const PrintStmt* stmt)
{
  stmt->getExpr()->accept(*this);
  emit(OpCode::PRINT);
}

void Compiler::beginScope()
{
  m_scopeDepth++;
}

void Compiler::endScope()
{
  m_scopeDepth--;

  while (!m_locals.empty() && m_locals.back().depth > m_scopeDepth)
  {
    emit(OpCode::POP);
    m_locals.pop_back();
  }
}

void Compiler::visitBlockStmt(const BlockStmt* stmt)
{
  beginScope();

  auto& statements = stmt->getStatements();
  for (auto& st : statements) {
    st->accept(*this);
  }

  endScope();
}

void Compiler::visitExprStmt(const ExprStmt* stmt)
{
  stmt->getExpr()->accept(*this);
  // emit(OpCode::POP);
}

void Compiler::patchJump(int jumpPos)
{
  int offset = static_cast<int>(m_chunk.size() - 1 - jumpPos);
  m_chunk[jumpPos].operand = offset;
}

void Compiler::visitIfStmt(const IfStmt* stmt)
{
  stmt->getCondition()->accept(*this);
  emit(OpCode::POP);

  std::size_t jumpToElse = emit(OpCode::JMP_IF_FALSE);
  stmt->getThen()->accept(*this);

  size_t jumpOverElse = 0;
  if (stmt->getElse())
    jumpOverElse = emit(OpCode::JMP);

  patchJump(jumpToElse);
  emit(OpCode::POP);

  if (stmt->getElse()) {
    stmt->getElse()->accept(*this);
    patchJump(jumpOverElse);
  }
}

void Compiler::visitWhileStmt(const WhileStmt* stmt)
{
  int loopStart = m_chunk.size();
  stmt->getCondition()->accept(*this);

  std::size_t exitJump = emit(OpCode::JMP_IF_FALSE);
  emit(OpCode::POP);

  stmt->getStatement()->accept(*this);

  int offset = m_chunk.size() - loopStart + 1;
  emit(OpCode::LOOP, offset);

  patchJump(exitJump);

  emit(OpCode::POP);
}
