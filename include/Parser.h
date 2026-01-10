#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "expressions/Expr.h"
#include "stmt/Stmt.h"
#include <memory>
#include <vector>

class Parser {
public:
  Parser(std::vector<Token> tokens);
  std::vector<std::unique_ptr<Stmt>> parse();

  enum class Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
  };

  const bool getHadError() const;

private:
  std::vector<Token> tokens;
  std::vector<std::unique_ptr<Stmt>> stmts;

  Token current;
  Token previous;

  bool hadError;
  bool panicMode;

  size_t currentIndex;

  Token scanToken();
  Token peek() const;

  bool isAtEnd();
  bool check(TokenType type);
  bool match(TokenType type);

  void advance();
  void consume(TokenType type, const char *message);
  void error(Token token, const std::string message);

  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> varDeclaration();

  std::unique_ptr<Stmt> statement();
  std::unique_ptr<Stmt> block();
  std::unique_ptr<Stmt> expressionStatement();
  std::unique_ptr<Stmt> printStatement();
  std::unique_ptr<Stmt> ifStatement();

  std::unique_ptr<Expr> parsePrecedence(Precedence precedence);

  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> parseNud(const Token &token);
  std::unique_ptr<Expr> parseLhs(bool canAssign, std::unique_ptr<Expr> lhs,
                                 TokenType op, std::unique_ptr<Expr> rhs);
  std::unique_ptr<Expr> parseBinary(const Token &token);

  void synchronize();
};

class ParserError : public std::exception {
public:
  ParserError(Token token, const char *m);
  const char *what() const noexcept override;
  const Token getToken() const;

private:
  Token mToken;
  const char *msg;
};

#endif // !PARSER_H
