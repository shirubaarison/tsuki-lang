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
#include "stmt/ExprStmt.h"
#include "stmt/PrintStmt.h"

Compiler::Compiler(std::vector<Instruction>& targetChunk, std::vector<std::unique_ptr<Stmt>> syntaxTree)
  : chunk(targetChunk), syntaxTree(std::move(syntaxTree)) {}

void Compiler::compile() {
  for (const auto& stmt : syntaxTree) {
    if (stmt) stmt->accept(*this);
  }

  chunk.push_back({OpCode::OP_RETURN, std::monostate{}});
}

void Compiler::emit(OpCode op) {
  chunk.push_back(Instruction{op, Value{}});
}

void Compiler::emitConstant(const Value& value) {
  chunk.push_back(Instruction{OpCode::OP_CONSTANT, value});
}

void Compiler::visitLiteralExpr(const LiteralExpr *expr) {
  emitConstant(expr->getValue());
}

void Compiler::visitBinaryExpr(const BinaryExpr *expr) {
  expr->getLeft()->accept(*this);
  expr->getRight()->accept(*this);

  switch (expr->getOperatorType()) {
  case TokenType::TOKEN_PLUS:
    emit(OpCode::OP_ADD);
    break;
  case TokenType::TOKEN_MINUS:
    emit(OpCode::OP_SUB);
    break;
  case TokenType::TOKEN_STAR:
    emit(OpCode::OP_MUL);
    break;
  case TokenType::TOKEN_SLASH:
    emit(OpCode::OP_DIV);
    break;
  case TokenType::TOKEN_GREATER:
    emit(OpCode::OP_GREATER);
    break;
  case TokenType::TOKEN_GREATER_EQUAL:
    emit(OpCode::OP_GREATER_EQUAL);
    break;
  case TokenType::TOKEN_EQUAL_EQUAL:
    emit(OpCode::OP_EQUAL);
    break;
  case TokenType::TOKEN_BANG_EQUAL:
    emit(OpCode::OP_NOT_EQUAL);
    break;
  case TokenType::TOKEN_LESS:
    emit(OpCode::OP_LESS);
    break;
  case TokenType::TOKEN_LESS_EQUAL:
    emit(OpCode::OP_LESS_EQUAL);
    break;
  case TokenType::TOKEN_AND:
    emit(OpCode::OP_AND);
    break;
  case TokenType::TOKEN_OR:
    emit(OpCode::OP_OR);
    break;
  default:
    break;
  }
}

void Compiler::visitAssignExpr(const AssignExpr * /*expr*/) {}

void Compiler::visitBooleanExpr(const BooleanExpr *expr) {
  if (expr->getValue()) {
    emit(OpCode::OP_TRUE);
  } else {
    emit(OpCode::OP_FALSE);
  }
}

void Compiler::visitGroupingExpr(const GroupingExpr *expr) {
  expr->getExpr()->accept(*this);
}

void Compiler::visitNameExpr(const NameExpr * /*expr*/) {}

void Compiler::visitPostfixExpr(const PostfixExpr * /*expr*/) {}

void Compiler::visitPrefixExpr(const PrefixExpr *expr) {}

void Compiler::visitVarExpr(const VarExpr * /*expr*/) {}

void Compiler::visitPrintStmt(const PrintStmt *stmt) {
  stmt->getExpr()->accept(*this);
  emit(OpCode::OP_PRINT);
}

void Compiler::visitBlockStmt(const BlockStmt * /*stmt*/) {}

void Compiler::visitExprStmt(const ExprStmt *stmt) {
  stmt->getExpr()->accept(*this);
  emit(OpCode::OP_POP);
}

void Compiler::visitIfStmt(const IfStmt * /*stmt*/) {}

void Compiler::visitVarStmt(const VarStmt * /*stmt*/) {}
