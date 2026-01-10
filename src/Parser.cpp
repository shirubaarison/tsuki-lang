#include "Parser.h"

#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "Lexer.h"
#include "TokenType.h"

#include "stmt/VarStmt.h"
#include "stmt/ExprStmt.h"
#include "stmt/PrintStmt.h"
#include "stmt/IfStmt.h"
#include "stmt/BlockStmt.h"

#include "expressions/BinaryExpr.h"
#include "expressions/AssignExpr.h"
#include "expressions/BooleanExpr.h"
#include "expressions/Expr.h"
#include "expressions/GroupingExpr.h"
#include "expressions/LiteralExpr.h"
#include "expressions/NameExpr.h"
#include "expressions/PrefixExpr.h"
#include "expressions/VarExpr.h"

inline static Parser::Precedence getPrecedence(Token op) {
  switch (op.type) {
    case TokenType::TOKEN_EQUAL_EQUAL:
    case TokenType::TOKEN_BANG_EQUAL:
      return Parser::Precedence::PREC_EQUALITY;

    case TokenType::TOKEN_GREATER:
    case TokenType::TOKEN_GREATER_EQUAL:
    case TokenType::TOKEN_LESS:
    case TokenType::TOKEN_LESS_EQUAL:
      return Parser::Precedence::PREC_COMPARISON;

    case TokenType::TOKEN_PLUS:
    case TokenType::TOKEN_MINUS:
      return Parser::Precedence::PREC_TERM;

    case TokenType::TOKEN_STAR:
    case TokenType::TOKEN_SLASH:
      return Parser::Precedence::PREC_FACTOR;

    case TokenType::TOKEN_AND:
      return Parser::Precedence::PREC_AND;

    case TokenType::TOKEN_OR:
      return Parser::Precedence::PREC_OR;

    default:
      return Parser::Precedence::PREC_NONE;
  }
}

Parser::Parser(std::vector<Token> tokens)
    : tokens(tokens), hadError(false), panicMode(false), currentIndex(0) {}

ParserError::ParserError(Token token, const char *m) : mToken(token), msg(m) {}

const char *ParserError::what() const noexcept { return msg; }

const Token ParserError::getToken() const { return mToken; }

const bool Parser::getHadError() const { return hadError; }

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  if (!this->tokens.empty()) {
    current = this->tokens[0];
  }

  while (!isAtEnd()) {
    std::unique_ptr<Stmt> stmt = declaration();
    stmts.push_back(std::move(stmt));
  }

  return std::move(stmts);
}

std::unique_ptr<Stmt> Parser::declaration() {
  try {
    if (match(TokenType::TOKEN_VAR)) {
      return varDeclaration();
    } else {
      return statement();
    }

  } catch (const ParserError &parserError) {
    error(parserError.getToken(), parserError.what());

    if (panicMode) {
      synchronize();
    }

    return nullptr;
  }
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
  consume(TokenType::TOKEN_IDENTIFIER, "Expect variable name.");
  std::string varName = current.lexeme;

  std::unique_ptr<Expr> rhs = nullptr;
  if (match(TokenType::TOKEN_EQUAL)) {
    rhs = expression();
  }

  consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  std::unique_ptr<VarExpr> varExpr = std::make_unique<VarExpr>(varName, std::move(rhs));

  return std::make_unique<VarStmt>(varName, std::move(varExpr));
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match(TokenType::TOKEN_PRINT)) {
    return printStatement();
  } else if (match(TokenType::TOKEN_IF)) {
    return ifStatement();
  } else if (match(TokenType::TOKEN_LEFT_BRACE)){
    return block();
  } else {
    return expressionStatement(); 
  }
}

std::unique_ptr<Stmt> Parser::printStatement() {
  std::unique_ptr<Expr> expr = expression();

  consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after expression");

  return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
  consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'if'");

  std::unique_ptr<Expr> condition = expression();

  consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after 'if'");

  std::unique_ptr<Stmt> thenBranch = statement();

  std::unique_ptr<Stmt> elseBranch = nullptr;

  if (check(TokenType::TOKEN_ELSE)) {
    match(TokenType::TOKEN_ELSE);
    elseBranch = statement();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::block() {
  std::vector<std::unique_ptr<Stmt>> statements;

  while (!check(TokenType::TOKEN_RIGHT_BRACE)) {
    statements.push_back(statement());
  }

  consume(TokenType::TOKEN_RIGHT_BRACE, "Expect '}' after block declaration.");

  return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
  std::unique_ptr<Expr> expr = expression();

  consume(TokenType::TOKEN_SEMICOLON, "Expected ';' after expression.");

  return std::make_unique<ExprStmt>(std::move(expr));
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

    bool canAssign = precedence <= Precedence::PREC_ASSIGNMENT;

    std::unique_ptr<Expr> rhs = parsePrecedence(
        static_cast<Precedence>(static_cast<int>(getPrecedence(op)) + 1));

    if (!rhs) {
      return nullptr;
    }

    lhs = parseLhs(canAssign, std::move(lhs), op.type, std::move(rhs));
  }

  return lhs;
}

std::unique_ptr<Expr> Parser::parseLhs(bool canAssign, std::unique_ptr<Expr> lhs, TokenType op, std::unique_ptr<Expr> rhs) {
  if (canAssign && op == TokenType::TOKEN_EQUAL) {
    if (dynamic_cast<const LiteralExpr*>(lhs.get()) == nullptr) {
      return nullptr;
    }
    return std::make_unique<AssignExpr>(std::move(lhs), std::move(rhs));
  }

  return std::make_unique<BinaryExpr>(std::move(lhs), op, std::move(rhs));
}

std::unique_ptr<Expr> Parser::parseNud(const Token &token) {
  switch (token.type) {
  case TokenType::TOKEN_FALSE:
    return std::make_unique<BooleanExpr>(false);

  case TokenType::TOKEN_TRUE:
    return std::make_unique<BooleanExpr>(true);

  case TokenType::TOKEN_NIL:
    return std::make_unique<LiteralExpr>(std::monostate{});

  case TokenType::TOKEN_NUMBER: {
      double value = std::stod(token.lexeme);
      return std::make_unique<LiteralExpr>(value);
  }
  case TokenType::TOKEN_STRING:
    return std::make_unique<LiteralExpr>(token.lexeme);

  case TokenType::TOKEN_LEFT_PAREN: {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }

  case TokenType::TOKEN_BANG:
  case TokenType::TOKEN_MINUS: {
    TokenType op = token.type;
    std::unique_ptr<Expr> right = parsePrecedence(Precedence::PREC_UNARY);
    return std::make_unique<PrefixExpr>(op, std::move(right));
  }

  case TokenType::TOKEN_IDENTIFIER:
    return std::make_unique<NameExpr>(token.lexeme);

  default:
    throw ParserError(token, "Unexpected expression.");
  }
}

Token Parser::scanToken() {
  if (currentIndex >= tokens.size()) {
    throw ParserError(current, "Expect right expression.");
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

void Parser::error(Token token, std::string message) {
  if (panicMode) {
    return;
  }

  panicMode = true;

  std::string errorMsg = "[line " + std::to_string(current.line) + "] ";
  if (token.type == TokenType::TOKEN_EOF) {
    std::cerr << errorMsg << "ParserError at end." << std::endl;
  } else if (token.type == TokenType::TOKEN_ERROR) {
    // nothing
  } else {
    std::cerr << errorMsg << "ParserError at '" + token.lexeme + "': " + message
              << std::endl;
  }
  hadError = true;
}

void Parser::consume(TokenType type, const char *message) {
  if (check(type)) {
    advance();
    return;
  }

  throw ParserError(peek(), message);
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

void Parser::synchronize() {
  panicMode = false;

  while (!isAtEnd()) {
    if (current.type == TokenType::TOKEN_SEMICOLON)
      return;

    switch (current.type) {
    case TokenType::TOKEN_CLASS:
    case TokenType::TOKEN_FUN:
    case TokenType::TOKEN_VAR:
    case TokenType::TOKEN_FOR:
    case TokenType::TOKEN_IF:
    case TokenType::TOKEN_WHILE:
    case TokenType::TOKEN_PRINT:
    case TokenType::TOKEN_RETURN:
      return;

    default:;
    }

    advance();
  }
}
