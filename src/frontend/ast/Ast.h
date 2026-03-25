#ifndef AST_H
#define AST_H

#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

#include "frontend/token/TokenType.h"
#include "runtime/value/Value.h"

struct BinaryExpr;
struct LiteralExpr;
struct AssignExpr;
struct BooleanExpr;
struct GroupingExpr;
struct NameExpr;
struct PrefixExpr;
struct VarExpr;

struct PrintStmt;
struct BlockStmt;
struct ExprStmt;
struct IfStmt;
struct WhileStmt;

using Expr =
    std::variant<std::unique_ptr<BinaryExpr>, std::unique_ptr<LiteralExpr>,
                 std::unique_ptr<AssignExpr>, std::unique_ptr<BooleanExpr>,
                 std::unique_ptr<GroupingExpr>, std::unique_ptr<NameExpr>,
                 std::unique_ptr<PrefixExpr>, std::unique_ptr<VarExpr>>;

using Stmt = std::variant<std::unique_ptr<PrintStmt>,
                          std::unique_ptr<BlockStmt>, std::unique_ptr<ExprStmt>,
                          std::unique_ptr<IfStmt>, std::unique_ptr<WhileStmt>>;

struct BinaryExpr {
  Expr left;
  TokenType op;
  Expr right;
};

struct LiteralExpr {
  Value value;
};

struct AssignExpr {
  std::string name;
  Expr expr;
};

struct BooleanExpr {
  bool value;
};

struct GroupingExpr {
  Expr expr;
};

struct NameExpr {
  std::string name;
};

struct PrefixExpr {
  TokenType op;
  Expr expr;
};

struct VarExpr {
  std::string name;
  Expr rhs;
};

struct PrintStmt {
  Expr expr;
};

struct BlockStmt {
  std::vector<Stmt> statements;
};

struct ExprStmt {
  Expr expr;
};

struct IfStmt {
  Expr condition;
  Stmt thenBranch;
  Stmt elseBranch;
};

struct WhileStmt {
  Expr condition;
  Stmt body;
};

void printExpr(std::ostream &out, const Expr &expr);
void printStmt(std::ostream &out, const Stmt &stmt);

#endif // !AST_H
