#include "Parser.h"

#include <iostream>
#include <memory>
#include <string>

#include "Lexer.h"
#include "TokenType.h"
#include "expressions/BinaryExpr.h"
#include "expressions/BooleanExpr.h"
#include "expressions/Expr.h"
#include "expressions/GroupingExpr.h"
#include "expressions/LiteralExpr.h"
#include "expressions/PrefixExpr.h"
#include "expressions/VarExpr.h"

inline static Parser::Precedence getPrecedence(Token op) {
  switch (op.type) {
  case TokenType::TOKEN_PLUS:
    return Parser::Precedence::PREC_TERM;
  case TokenType::TOKEN_MINUS:
    return Parser::Precedence::PREC_TERM;
  case TokenType::TOKEN_STAR:
    return Parser::Precedence::PREC_FACTOR;
  case TokenType::TOKEN_SLASH:
    return Parser::Precedence::PREC_FACTOR;

  default:
    return Parser::Precedence::PREC_NONE;
  }
}

Parser::Parser(std::vector<Token> tokens)
    : tokens(tokens), hadError(false), panicMode(false), currentIndex(0) {}

std::vector<std::unique_ptr<Expr>> Parser::parse() {
  if (!this->tokens.empty()) {
    current = this->tokens[0];
  }

  try {
    while (!isAtEnd()) {
      std::unique_ptr<Expr> expr = declaration();
      exprs.push_back(std::move(expr));
    }
  } catch (const ParserError &parseError) {
    error(parseError.what());
  }
  return std::move(exprs);
}

std::unique_ptr<Expr> Parser::declaration() {
  if (match(TokenType::TOKEN_VAR)) {
    return varDeclaration();
  } else {
    return statement();
  }
}

std::unique_ptr<Expr> Parser::varDeclaration() {
  consume(TokenType::TOKEN_IDENTIFIER, "Expect variable name");
  std::string varName = current.lexeme;

  std::unique_ptr<Expr> rhs = nullptr;
  if (match(TokenType::TOKEN_EQUAL)) {
    rhs = expression();
  }

  consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  return std::make_unique<VarExpr>(varName, std::move(rhs));
}

std::unique_ptr<Expr> Parser::statement() { return expressionStatement(); }

std::unique_ptr<Expr> Parser::expressionStatement() {
  std::unique_ptr<Expr> expr = expression();
  consume(TokenType::TOKEN_SEMICOLON, "Expected ';' after expression");

  return expr;
}

std::unique_ptr<Expr> Parser::expression() {
  return parsePrecedence(Precedence::PREC_ASSIGNMENT);
}

std::unique_ptr<Expr> Parser::parsePrecedence(Precedence precedence) {
  Token token = scanToken();
  std::unique_ptr<Expr> lhs = parseNud(token);

  if (!lhs) {
    return nullptr;
  }

  while (precedence <= getPrecedence(peek())) {
    Token op = peek();
    advance();

    std::unique_ptr<Expr> rhs = parsePrecedence(
        static_cast<Precedence>(static_cast<int>(getPrecedence(op)) + 1));

    if (!rhs) {
      return nullptr;
    }

    lhs = std::make_unique<BinaryExpr>(std::move(lhs), op.type, std::move(rhs));
  }

  return lhs;
}

std::unique_ptr<Expr> Parser::parseNud(const Token &token) {
  switch (token.type) {
  // ---- Literals ----
  case TokenType::TOKEN_FALSE:
    return std::make_unique<BooleanExpr>(false);
  case TokenType::TOKEN_TRUE:
    return std::make_unique<BooleanExpr>(true);
  case TokenType::TOKEN_NIL:
    return std::make_unique<LiteralExpr>(nullptr);
  case TokenType::TOKEN_NUMBER:
  case TokenType::TOKEN_STRING:
    return std::make_unique<LiteralExpr>(token.lexeme);

  // --- Grouping -----
  case TokenType::TOKEN_LEFT_PAREN: {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }

  // ---- Prefix Operators
  case TokenType::TOKEN_BANG:
  case TokenType::TOKEN_MINUS: {
    TokenType op = token.type;
    std::unique_ptr<Expr> right = expression();
    return std::make_unique<PrefixExpr>(op, std::move(right));
  }

  case TokenType::TOKEN_IDENTIFIER:
    return std::make_unique<VarExpr>(token.lexeme, nullptr);

  default:
    throw ParserError("Unexpected right expression");
  }
}

Token Parser::scanToken() {
  if (currentIndex >= tokens.size()) {
    throw ParserError("Expect right expression");
    return tokens.back();
  }

  return tokens[currentIndex++];
}

void Parser::advance() {
  if (!isAtEnd()) {
    previous = current;
    current = scanToken();
  }
}

Token Parser::peek() const {
  if (currentIndex >= tokens.size()) {
    return tokens.back();
  }

  return tokens[currentIndex];
}

void Parser::error(std::string message) {
  std::cerr << "ParserError: " << message << ": '" << current.lexeme << "'"
            << std::endl;
  hadError = true;
}

void Parser::consume(TokenType type, const char *message) {
  if (check(type)) {
    advance();
    return;
  }
  throw ParserError(message);
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }

  return false;
}

bool Parser::isAtEnd() { return peek().type == TokenType::TOKEN_EOF; }

bool Parser::check(TokenType type) {
  if (isAtEnd()) {
    return false;
  }

  return peek().type == type;
}
