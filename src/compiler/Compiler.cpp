#include "compiler/Compiler.h"

#include "frontend/ast/Ast.h"
#include "frontend/token/TokenType.h"
#include "runtime/value/Value.h"

#include <cstddef>
#include <memory>
#include <vector>

Compiler::Compiler() {}

CompilerError::CompilerError(std::string m) : m_msg(std::move(m)) {}

const char *CompilerError::what() const noexcept { return m_msg.c_str(); }

void Compiler::error(const std::string &msg) {
  std::cerr << "CompilerError: " << msg << std::endl;
}

std::vector<Instruction> Compiler::compile(std::vector<Stmt> syntaxTree) {
  m_chunk.clear();
  m_syntaxTree = std::move(syntaxTree);

  try {
    for (const auto &stmt : m_syntaxTree)
    compileStmt(stmt);
  } catch (const CompilerError &compilerError) {
    error(compilerError.what());
  }

  m_chunk.push_back({OpCode::RET, std::monostate{}});

  return m_chunk;
}

size_t Compiler::emit(OpCode op) {
  m_chunk.push_back(Instruction{op, Value{}});
  return m_chunk.size() - 1;
}

size_t Compiler::emit(OpCode op, const Value &value) {
  m_chunk.push_back(Instruction{op, value});
  return m_chunk.size() - 1;
}

void Compiler::emitConstant(const Value &value) {
  m_chunk.push_back(Instruction{OpCode::LOAD_CONSTANT, value});
}

void Compiler::compileExpr(const Expr &expr) {
  std::visit(
    overload{
      [&](const std::unique_ptr<LiteralExpr> &e) {
        emitConstant(e->value);
      },
      [&](const std::unique_ptr<BinaryExpr> &e) {
        if (e->op == TokenType::TOKEN_AND) {
          compileExpr(e->left);

          int endJump = emit(OpCode::JMP_IF_FALSE);
          emit(OpCode::POP);

          compileExpr(e->right);

          patchJump(endJump);
          return;
        }

        if (e->op == TokenType::TOKEN_OR) {
          compileExpr(e->left);

          int elseJump = emit(OpCode::JMP_IF_FALSE);
          int endJump = emit(OpCode::JMP);

          patchJump(elseJump);
          emit(OpCode::POP);

          compileExpr(e->right);

          patchJump(endJump);
          return;
        }

        compileExpr(e->left);
        compileExpr(e->right);

        switch (e->op) {
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
      },
      [&](const std::unique_ptr<AssignExpr> &e) {
        const auto &name = e->name;
        compileExpr(e->expr);

        if (m_scopeDepth == 0) {
          if (resolveGlobal(name) == -1) {
            addGlobal(name);
            emit(OpCode::DEFINE_GLOBAL, name);
          } else {
            emit(OpCode::SET_GLOBAL, name);
          }
        } else {
          int localSlot = resolveLocal(name);
          if (localSlot == -1) {
            if (resolveGlobal(name) == -1) {
              addLocal(name);
              emit(OpCode::DEFINE_LOCAL);
            } else {
              emit(OpCode::SET_GLOBAL, name);
            }
          } else {
            emit(OpCode::SET_LOCAL, localSlot);
          }
        }
      },
      [&](const std::unique_ptr<BooleanExpr> &e) {
        if (e->value) {
          emit(OpCode::TRUE);
        } else {
          emit(OpCode::FALSE);
        }
      },
      [&](const std::unique_ptr<GroupingExpr> &e) { compileExpr(e->expr); },
      [&](const std::unique_ptr<NameExpr> &e) {
        auto name = e->name;
        int localSlot = resolveLocal(name);
        if (localSlot != -1) {
          emit(OpCode::GET_LOCAL, localSlot);
          return;
        }
        if (resolveGlobal(name) == -1)
          throw CompilerError("Undefined variable '" + name + "'");

        emit(OpCode::GET_GLOBAL, name);
      },
      [&](const std::unique_ptr<PrefixExpr> &) {
        // Not yet implemented
      },
      [&](const std::unique_ptr<VarExpr> &e) { compileExpr(e->rhs); }},
    expr);
}

void Compiler::compileStmt(const Stmt &stmt) {
  std::visit(overload{[&](const std::unique_ptr<PrintStmt> &s) {
    compileExpr(s->expr);
    emit(OpCode::PRINT);
  },
    [&](const std::unique_ptr<BlockStmt> &s) {
      beginScope();

      for (const auto &st : s->statements) {
        compileStmt(st);
      }

      endScope();
    },
    [&](const std::unique_ptr<ExprStmt> &s) {
      compileExpr(s->expr);
      // emit(OpCode::POP);
    },
    [&](const std::unique_ptr<IfStmt> &s) {
      compileExpr(s->condition);
      emit(OpCode::POP);

      std::size_t jumpToElse = emit(OpCode::JMP_IF_FALSE);
      compileStmt(s->thenBranch);

      bool hasElse = std::visit(
        [](const auto &p) -> bool { return p != nullptr; },
        s->elseBranch);

      size_t jumpOverElse = 0;
      if (hasElse)
        jumpOverElse = emit(OpCode::JMP);

      patchJump(jumpToElse);
      emit(OpCode::POP);

      if (hasElse) {
        compileStmt(s->elseBranch);
        patchJump(jumpOverElse);
      }
    },
    [&](const std::unique_ptr<WhileStmt> &s) {
      int loopStart = m_chunk.size();
      compileExpr(s->condition);

      std::size_t exitJump = emit(OpCode::JMP_IF_FALSE);
      emit(OpCode::POP);

      compileStmt(s->body);

      int offset = m_chunk.size() - loopStart + 1;
      emit(OpCode::LOOP, offset);

      patchJump(exitJump);

      emit(OpCode::POP);
    }},
             stmt);
}

int Compiler::resolveLocal(const std::string &name) {
  for (int i = m_locals.size() - 1; i >= 0; --i) {
    if (m_locals[i].name == name)
      return i;
  }

  return -1;
}

int Compiler::resolveGlobal(const std::string &name) {
  for (size_t i = 0; i < m_globals.size(); ++i) {
    if (m_globals[i] == name)
      return i;
  }

  return -1;
}

void Compiler::addLocal(const std::string &name) {
  Local local = {name, m_scopeDepth};
  m_locals.push_back(local);
}

void Compiler::addGlobal(const std::string &name) { m_globals.push_back(name); }

void Compiler::patchJump(int jumpPos) {
  int offset = static_cast<int>(m_chunk.size() - 1 - jumpPos);
  m_chunk[jumpPos].operand = offset;
}

void Compiler::beginScope() { m_scopeDepth++; }

void Compiler::endScope() {
  m_scopeDepth--;

  while (!m_locals.empty() && m_locals.back().depth > m_scopeDepth) {
    emit(OpCode::POP);
    m_locals.pop_back();
  }
}
