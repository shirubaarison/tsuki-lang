#ifndef PARSER_H
#define PARSER_H

#include "frontend/ast/Ast.h"
#include "frontend/lexer/Lexer.h"

#include <vector>

class Parser {
public:
  Parser(std::vector<Token> tokens);
  std::vector<Stmt> parse();

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

  bool getHadError() const;

private:
  std::vector<Token> tokens;
  std::vector<Stmt> stmts;

  Token current;
  Token previous;

  bool hadError;
  bool panicMode;

  size_t currentIndex;

  Token scanToken();
  Token peek() const;
  Token peekAhead() const;

  bool isAtEnd();
  bool check(TokenType type);
  bool match(TokenType type);

  void advance();
  void consume(TokenType type, const std::string &message);
  void error(Token token, const std::string &message);

  Stmt declaration();
  Stmt statement();
  Stmt block();
  Stmt expressionStatement();
  Stmt printStatement();
  Stmt ifStatement();
  Stmt whileStatement();

  Expr parsePrecedence(Precedence precedence);

  Expr expression();
  Expr parseNud(const Token &token);
  Expr parseLhs(bool canAssign, Expr lhs, TokenType op, Expr rhs);
  Expr parseBinary(const Token &token);

  void synchronize();
};

class ParserError : public std::exception {
public:
  ParserError(Token token, std::string m);
  const char *what() const noexcept override;
  const Token getToken() const;

private:
  Token mToken;
  std::string msg;
};

#endif // !PARSER_H
