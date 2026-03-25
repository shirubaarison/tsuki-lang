#include "frontend/ast/Ast.h"

#include "frontend/token/TokenType.h"
#include "runtime/value/Value.h"

void printExpr(std::ostream &out, const Expr &expr) {
  std::visit(overload{[&](const std::unique_ptr<BinaryExpr> &e) {
    out << "BinaryExpr(";
    printExpr(out, e->left);
    out << " " << tokenToOperator(e->op) << " ";
    printExpr(out, e->right);
    out << ")";
  },
    [&](const std::unique_ptr<LiteralExpr> &e) {
      out << "LiteralExpr(";
      std::visit(ValuePrinter{}, e->value);
      out << ")";
    },
    [&](const std::unique_ptr<AssignExpr> &e) {
      out << "AssignExpr(" << e->name << " : ";
      printExpr(out, e->expr);
      out << ")";
    },
    [&](const std::unique_ptr<BooleanExpr> &e) {
      out << "BooleanExpr(" << (e->value ? "true" : "false")
        << ")";
    },
    [&](const std::unique_ptr<GroupingExpr> &e) {
      out << "GroupingExpr(";
      printExpr(out, e->expr);
      out << ")";
    },
    [&](const std::unique_ptr<NameExpr> &e) {
      out << "NameExpr(" << e->name << ")";
    },
    [&](const std::unique_ptr<PrefixExpr> &e) {
      out << "PrefixExpr(";
      out << tokenToOperator(e->op);
      printExpr(out, e->expr);
      out << ")";
    },
    [&](const std::unique_ptr<VarExpr> &e) {
      out << "VarExpr(";
      printExpr(out, e->rhs);
      out << ")";
    }},
             expr);
}

void printStmt(std::ostream &out, const Stmt &stmt) {
  std::visit(
    overload{[&](const std::unique_ptr<PrintStmt> &s) {
      out << "PrintStmt(";
      printExpr(out, s->expr);
      out << ")\n";
    },
      [&](const std::unique_ptr<BlockStmt> &s) {
        out << "blockStmt(";
        for (const auto &st : s->statements) {
          printStmt(out, st);
        }
        out << ")";
      },
      [&](const std::unique_ptr<ExprStmt> &s) {
        out << "ExprStmt(";
        printExpr(out, s->expr);
        out << ")";
      },
      [&](const std::unique_ptr<IfStmt> &s) {
        out << "ifStmt(";
        printExpr(out, s->condition);
        out << ")\n{";
        printStmt(out, s->thenBranch);
        out << "}";
        if (std::get_if<std::unique_ptr<BlockStmt>>(&s->elseBranch) ||
          std::get_if<std::unique_ptr<ExprStmt>>(&s->elseBranch) ||
          std::get_if<std::unique_ptr<PrintStmt>>(&s->elseBranch) ||
          std::get_if<std::unique_ptr<IfStmt>>(&s->elseBranch) ||
          std::get_if<std::unique_ptr<WhileStmt>>(&s->elseBranch)) {
          bool hasElse = std::visit(
            [](const auto &p) -> bool { return p != nullptr; },
            s->elseBranch);
          if (hasElse) {
            out << "elseBranch(\n";
            printStmt(out, s->elseBranch);
            out << ")";
          }
        }
      },
      [&](const std::unique_ptr<WhileStmt> &s) {
        out << "WhileStmt(";
        printExpr(out, s->condition);
        out << ") {";
        printStmt(out, s->body);
        out << "}";
      }},
    stmt);
}
