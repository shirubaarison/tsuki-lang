#include "frontend/parser/Parser.h"

#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "frontend/ast/Ast.h"
#include "frontend/lexer/Lexer.h"
#include "frontend/token/TokenType.h"
#include "runtime/value/Value.h"

namespace {
inline Parser::Precedence getPrecedence(Token op) {
  switch (op.type) {
    case TokenType::TOKEN_EQUAL:
      return Parser::Precedence::PREC_ASSIGNMENT;

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
} // namespace

Parser::Parser(std::vector<Token> tokens)
: tokens(tokens), hadError(false), panicMode(false), currentIndex(0) {}

ParserError::ParserError(Token token, std::string m)
: mToken(token), msg(std::move(m)) {}

const char *ParserError::what() const noexcept { return msg.c_str(); }

const Token ParserError::getToken() const { return mToken; }

bool Parser::getHadError() const { return hadError; }

std::vector<Stmt> Parser::parse() {
  if (!this->tokens.empty()) {
    current = this->tokens[0];
  }

  while (!isAtEnd()) {
    Stmt stmt = declaration();
    stmts.push_back(std::move(stmt));
  }

  return std::move(stmts);
}

Stmt Parser::declaration() {
  try {
    return statement();
  } catch (const ParserError &parserError) {
    error(parserError.getToken(), parserError.what());

    if (panicMode) {
      synchronize();
    }

    return Stmt{std::make_unique<ExprStmt>(ExprStmt{
      Expr{std::make_unique<LiteralExpr>(LiteralExpr{std::monostate{}})}})};
  }
}

Stmt Parser::statement() {
  if (match(TokenType::TOKEN_PRINT)) {
    return printStatement();
  } else if (match(TokenType::TOKEN_IF)) {
    return ifStatement();
  } else if (match(TokenType::TOKEN_WHILE)) {
    return whileStatement();
  } else if (match(TokenType::TOKEN_LEFT_BRACE)) {
    return block();
  } else {
    return expressionStatement();
  }
}

Stmt Parser::printStatement() {
  Expr expr = expression();

  consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after expression");

  return Stmt{std::make_unique<PrintStmt>(PrintStmt{std::move(expr)})};
}

Stmt Parser::ifStatement() {
  consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'if'");

  Expr condition = expression();

  consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after 'if'");

  Stmt thenBranch = statement();

  Stmt elseBranch = Stmt{std::unique_ptr<ExprStmt>(nullptr)};
  if (check(TokenType::TOKEN_ELSE)) {
    match(TokenType::TOKEN_ELSE);
    elseBranch = statement();
  }

  return Stmt{std::make_unique<IfStmt>(IfStmt{
    std::move(condition), std::move(thenBranch), std::move(elseBranch)})};
}

Stmt Parser::whileStatement() {
  consume(TokenType::TOKEN_LEFT_PAREN, "Exepct '(' after 'while'.");

  Expr condition = expression();

  consume(TokenType::TOKEN_RIGHT_PAREN, "Exepct ')' after condition.");

  Stmt whileStatament = statement();

  return Stmt{std::make_unique<WhileStmt>(
    WhileStmt{std::move(condition), std::move(whileStatament)})};
}

Stmt Parser::block() {
  std::vector<Stmt> statements;

  while (!check(TokenType::TOKEN_RIGHT_BRACE)) {
    statements.push_back(declaration());
  }

  consume(TokenType::TOKEN_RIGHT_BRACE, "Expect '}' after block declaration.");

  return Stmt{std::make_unique<BlockStmt>(BlockStmt{std::move(statements)})};
}

Stmt Parser::expressionStatement() {
  Expr expr = expression();

  consume(TokenType::TOKEN_SEMICOLON, "Expected ';' after expression.");

  return Stmt{std::make_unique<ExprStmt>(ExprStmt{std::move(expr)})};
}

Expr Parser::expression() {
  return parsePrecedence(Precedence::PREC_ASSIGNMENT);
}

Expr Parser::parsePrecedence(Precedence precedence) {
  Token token = scanToken();
  Expr lhs = parseNud(token);

  while (precedence <= getPrecedence(peek()) &&
    getPrecedence(peek()) != Precedence::PREC_NONE) {
    Token op = peek();
    advance();

    bool canAssign = precedence <= Precedence::PREC_ASSIGNMENT;

    Expr rhs = parsePrecedence(
      static_cast<Precedence>(static_cast<int>(getPrecedence(op)) + 1));

    lhs = parseLhs(canAssign, std::move(lhs), op.type, std::move(rhs));
  }

  return lhs;
}

Expr Parser::parseLhs(bool canAssign, Expr lhs, TokenType op, Expr rhs) {
  if (op == TokenType::TOKEN_EQUAL) {
    if (!canAssign)
      throw ParserError(previous, "Invalid assignment target.");

    auto *namePtr = std::get_if<std::unique_ptr<NameExpr>>(&lhs);
    if (namePtr && *namePtr)
      return Expr{std::make_unique<AssignExpr>(
        AssignExpr{(*namePtr)->name, std::move(rhs)})};

    throw ParserError(previous, "Invalid assignment target.");
  }
  return Expr{std::make_unique<BinaryExpr>(
    BinaryExpr{std::move(lhs), op, std::move(rhs)})};
}

Expr Parser::parseNud(const Token &token) {
  switch (token.type) {
    case TokenType::TOKEN_FALSE:
      return Expr{std::make_unique<BooleanExpr>(BooleanExpr{false})};

    case TokenType::TOKEN_TRUE:
      return Expr{std::make_unique<BooleanExpr>(BooleanExpr{true})};

    case TokenType::TOKEN_NIL:
      return Expr{std::make_unique<LiteralExpr>(LiteralExpr{std::monostate{}})};

    case TokenType::TOKEN_NUMBER: {
      Value value;
      if (token.lexeme.contains('.')) {
        value = std::stod(token.lexeme);
      } else {
        value = std::stoi(token.lexeme);
      }
      return Expr{std::make_unique<LiteralExpr>(LiteralExpr{value})};
    }
    case TokenType::TOKEN_STRING:
      return Expr{std::make_unique<LiteralExpr>(LiteralExpr{token.lexeme})};

    case TokenType::TOKEN_LEFT_PAREN: {
      Expr expr = expression();
      consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
      return Expr{std::make_unique<GroupingExpr>(GroupingExpr{std::move(expr)})};
    }

    case TokenType::TOKEN_BANG:
    case TokenType::TOKEN_MINUS: {
      TokenType opType = token.type;
      Expr right = parsePrecedence(Precedence::PREC_UNARY);
      return Expr{
        std::make_unique<PrefixExpr>(PrefixExpr{opType, std::move(right)})};
    }

    case TokenType::TOKEN_IDENTIFIER:
      return Expr{std::make_unique<NameExpr>(NameExpr{token.lexeme})};

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

Token Parser::peekAhead() const {
  if (currentIndex >= tokens.size()) {
    return tokens.back();
  }

  return tokens[currentIndex + 1];
}

void Parser::error(Token token, const std::string &message) {
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

void Parser::consume(TokenType type, const std::string &message) {
  if (check(type)) {
    advance();
    return;
  }

  throw ParserError(peek(), message.c_str());
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
    if (current.type == TokenType::TOKEN_SEMICOLON) {
      return;
    }

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
