#ifndef VISITOR_CLASS_H
#define VISITOR_CLASS_H

class BinaryExpr;
class LiteralExpr;
class AssignExpr;
class BooleanExpr;
class GroupingExpr;
class NameExpr;
class PostfixExpr;
class PrefixExpr;
class VarExpr;

class PrintStmt;
class BlockStmt;
class ExprStmt;
class IfStmt;
class VarStmt;

class Visitor {
public:
  virtual void visitBinaryExpr(const BinaryExpr* expr) = 0;
  virtual void visitLiteralExpr(const LiteralExpr* expr) = 0;
  virtual void visitAssignExpr(const AssignExpr *expr) = 0;
  virtual void visitBooleanExpr(const BooleanExpr *expr) = 0;
  virtual void visitGroupingExpr(const GroupingExpr *expr) = 0;
  virtual void visitNameExpr(const NameExpr* expr) = 0;
  virtual void visitPostfixExpr(const PostfixExpr *expr) = 0;
  virtual void visitPrefixExpr(const PrefixExpr *expr) = 0;
  virtual void visitVarExpr(const VarExpr *expr) = 0;

  virtual void visitPrintStmt(const PrintStmt *stmt) = 0;
  virtual void visitBlockStmt(const BlockStmt *stmt) = 0;
  virtual void visitExprStmt(const ExprStmt *stmt) = 0;
  virtual void visitIfStmt(const IfStmt *stmt) = 0;
  virtual void visitVarStmt(const VarStmt *stmt) = 0;
};

#endif // !VISITOR_CLASS_H

