#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "expressions/Expr.h"
#include <memory>
#include <vector>

class Parser {
public:
  Parser(std::vector<Token> tokens);
  std::vector<std::unique_ptr<Expr>> parse();
  std::unique_ptr<Expr> doParse();

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

private:
  std::vector<Token> tokens;
  std::vector<std::unique_ptr<Expr>> exprs;

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
  void error(const std::string message);

  std::unique_ptr<Expr> parsePrecedence(Precedence precedence);
  std::unique_ptr<Expr> statement();
  std::unique_ptr<Expr> expressionStatement();
  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> parseNud(const Token &token);
};

class ParserError : public std::exception {
public:
  ParserError(const char *m) : msg(m) {}
  const char *what() const noexcept override { return msg; }

private:
  const char *msg;
};

#endif // !PARSER_H
