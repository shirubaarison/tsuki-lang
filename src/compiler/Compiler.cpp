#include "compiler/Compiler.h"

#include "frontend/ast/Ast.h"
#include "frontend/token/TokenType.h"
#include "ir/OpCode.h"
#include "runtime/value/Value.h"

#include <cstddef>
#include <variant>
#include <vector>

Compiler::Compiler() {}

CompilerError::CompilerError(std::string m) : m_msg(std::move(m)) {}

const char* CompilerError::what() const noexcept { return m_msg.c_str(); }

Chunk Compiler::compile(std::vector<Stmt> syntaxTree)
{
  m_chunk = Chunk{};
  m_syntaxTree = std::move(syntaxTree);

  for (const auto &stmt : m_syntaxTree)
    compileStmt(stmt);

  m_chunk.writeOp(OpCode::RET, 0);

  return m_chunk;
}

size_t Compiler::emit(OpCode op)
{
  size_t pos = m_chunk.code.size();
  m_chunk.writeOp(op, 0);
  return pos;
}

size_t Compiler::emit(OpCode op, const Value& value)
{
  size_t pos = m_chunk.code.size();
  m_chunk.writeOp(op, 0);
  int idx = m_chunk.addConstant(value);
  m_chunk.write(static_cast<Byte>(idx), 0);

  return pos;
}

void Compiler::emitConstant(const Value& value)
{
  emit(OpCode::LOAD_CONSTANT, value);
}

size_t Compiler::emitJump(OpCode op)
{
  size_t pos = m_chunk.code.size();
  m_chunk.writeOp(op, 0);
  m_chunk.write(0xFF, 0); // placeholder

  return pos;
}

void Compiler::compileExpr(const Expr& expr)
{
  std::visit([this](auto&& node) { compileExpr(*node); }, expr);
}

void Compiler::compileExpr(const LiteralExpr& e)
{
  emitConstant(e.value);
}

void Compiler::compileExpr(const BinaryExpr& e)
{
  if (e.op == TokenType::TOKEN_AND)
  {
    compileExpr(e.left);

    int endJump = emitJump(OpCode::JMP_IF_FALSE);
    emit(OpCode::POP);

    compileExpr(e.right);

    patchJump(endJump);

    return;
  }

  if (e.op == TokenType::TOKEN_OR)
  {
    compileExpr(e.left);

    int elseJump = emitJump(OpCode::JMP_IF_FALSE);
    int endJump = emitJump(OpCode::JMP);

    patchJump(elseJump);
    emit(OpCode::POP);

    compileExpr(e.right);
    patchJump(endJump);

    return;
  }

  compileExpr(e.left);
  compileExpr(e.right);

  switch (e.op)
  {
    case TokenType::TOKEN_PLUS:
      emit(OpCode::ADD);
      break;
    case TokenType::TOKEN_MINUS:
      emit(OpCode::SUB);
      break;
    case TokenType::TOKEN_STAR:
      emit(OpCode::MUL);
      break;
    case TokenType::TOKEN_SLASH:
      emit(OpCode::DIV);
      break;
    case TokenType::TOKEN_GREATER:
      emit(OpCode::GREATER);
      break;
    case TokenType::TOKEN_GREATER_EQUAL:
      emit(OpCode::GREATER_EQUAL);
      break;
    case TokenType::TOKEN_EQUAL_EQUAL:
      emit(OpCode::EQUAL);
      break;
    case TokenType::TOKEN_BANG_EQUAL:
      emit(OpCode::NOT_EQUAL);
      break;
    case TokenType::TOKEN_LESS:
      emit(OpCode::LESS);
      break;
    case TokenType::TOKEN_LESS_EQUAL:
      emit(OpCode::LESS_EQUAL);
      break;
    default:
      break;
  }
}

void Compiler::compileExpr(const AssignExpr& e)
{
  const auto &name = e.name;
  compileExpr(e.expr);

  if (symbolTable.getScopeDepth() == 0)
  {
    if (symbolTable.resolveGlobal(name) == -1)
    {
      symbolTable.defineGlobal(name);
      emit(OpCode::DEFINE_GLOBAL, name);
    }
    else
    {
      emit(OpCode::SET_GLOBAL, name);
    }
  }
  else
  {
    int localSlot = symbolTable.resolveLocal(name);
    if (localSlot == -1)
    {
      if (symbolTable.resolveGlobal(name) == -1)
      {
        symbolTable.defineLocal(name);
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


void Compiler::compileExpr(const BooleanExpr& e)
{
  if (e.value)
  {
    emit(OpCode::TRUE);
  }
  else
{
    emit(OpCode::FALSE);
  }
}

void Compiler::compileExpr(const GroupingExpr& e)
{
  compileExpr(e.expr);
}

void Compiler::compileExpr(const NameExpr& e)
{
  auto name = e.name;
  int localSlot = symbolTable.resolveLocal(name);
  if (localSlot != -1)
  {
    emit(OpCode::GET_LOCAL, localSlot);
    return;
  }
  if (symbolTable.resolveGlobal(name) == -1)
    throw CompilerError("Undefined variable '" + name + "'");

  emit(OpCode::GET_GLOBAL, name);
}

void Compiler::compileExpr(const PrefixExpr &e)
{
  compileExpr(e.expr);

  switch (e.op) {
    case TokenType::TOKEN_BANG:
      emit(OpCode::NOT);
      break;

    case TokenType::TOKEN_MINUS:
      emit(OpCode::NEGATE);
      break;

    default:
      throw std::logic_error("Unsupported prefix operator");
  }
}

void Compiler::compileExpr(const VarExpr& e)
{
  compileExpr(e.rhs);
}

void Compiler::compileStmt(const Stmt &stmt)
{
  std::visit([this](auto&& node) { compileStmt(*node); }, stmt);
}

void Compiler::compileStmt(const PrintStmt& s)
{
  compileExpr(s.expr);
  emit(OpCode::PRINT);
}

void Compiler::compileStmt(const BlockStmt& s)
{
  symbolTable.beginScope();

  for (const auto &st : s.statements)
  {
    compileStmt(st);
  }

  int pop = symbolTable.endScope();
  for (int i = 0; i < pop; ++i)
    emit(OpCode::POP);
}

void Compiler::compileStmt(const ExprStmt& s)
{
  compileExpr(s.expr);
  emit(OpCode::POP);
}

void Compiler::compileStmt(const IfStmt& s)
{
  compileExpr(s.condition);

  std::size_t jumpToElse = emitJump(OpCode::JMP_IF_FALSE);
  emit(OpCode::POP);
  compileStmt(s.thenBranch);

  bool hasElse = std::visit(
    [](const auto &p) -> bool { return p != nullptr; },
    s.elseBranch);

  size_t jumpOverElse = 0;
  if (hasElse)
    jumpOverElse = emitJump(OpCode::JMP);

  patchJump(jumpToElse);
  emit(OpCode::POP);

  if (hasElse)
  {
    compileStmt(s.elseBranch);
    patchJump(jumpOverElse);
  }
}

void Compiler::compileStmt(const WhileStmt& s)
{
  int loopStart = m_chunk.code.size();
  compileExpr(s.condition);

  std::size_t exitJump = emitJump(OpCode::JMP_IF_FALSE);
  emit(OpCode::POP);

  compileStmt(s.body);

  // Emit LOOP with raw byte offset back to loopStart
  size_t loopInstrPos = m_chunk.code.size();
  m_chunk.writeOp(OpCode::LOOP, 0);
  int offset = static_cast<int>(loopInstrPos + 2 - loopStart);
  m_chunk.write(static_cast<Byte>(offset), 0);

  patchJump(exitJump);

  emit(OpCode::POP);
}

void Compiler::patchJump(int jumpPos)
{
  // jumpPos points to the opcode byte; jumpPos+1 is the operand byte
  int offset = static_cast<int>(m_chunk.code.size()) - jumpPos - 2;
  m_chunk.code[jumpPos + 1] = static_cast<Byte>(offset);
}
