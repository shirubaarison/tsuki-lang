#ifndef COMPILER_H
#define COMPILER_H

#include "VM/VM.h"
#include "Visitor.h"
#include "stmt/Stmt.h"
#include <memory>

class Compiler : public Visitor {
private:
  std::vector<Instruction>& chunk;
  std::vector<std::unique_ptr<Stmt>> syntaxTree;

  void emit(OpCode op);
  void emit(OpCode op, const Value& value);
  void emitConstant(const Value& value);
public:
  Compiler(std::vector<Instruction>& targetChunk, std::vector<std::unique_ptr<Stmt>> syntaxTree);
  void compile();

  void visitLiteralExpr(const LiteralExpr* expr) override;
  void visitBinaryExpr(const BinaryExpr* expr) override;
  void visitAssignExpr(const AssignExpr *expr) override;
  void visitBooleanExpr(const BooleanExpr *expr) override;
  void visitGroupingExpr(const GroupingExpr *expr) override;
  void visitNameExpr(const NameExpr* expr) override;
  void visitPostfixExpr(const PostfixExpr *expr) override;
  void visitPrefixExpr(const PrefixExpr *expr) override;
  void visitVarExpr(const VarExpr *expr) override;

  void visitPrintStmt(const PrintStmt* stmt) override;
  void visitBlockStmt(const BlockStmt *stmt)override;
  void visitExprStmt(const ExprStmt *stmt)override;
  void visitIfStmt(const IfStmt *stmt)override;
  void visitVarStmt(const VarStmt *stmt)override;
};

#endif // !COMPILER_H
