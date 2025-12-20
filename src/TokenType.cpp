#include "TokenType.h"

const char *tokenToOperator(TokenType type) {
  switch (type) {
  case TokenType::TOKEN_PLUS:
    return "+";
  case TokenType::TOKEN_MINUS:
    return "-";
  case TokenType::TOKEN_STAR:
    return "*";
  case TokenType::TOKEN_SLASH:
    return "/";
  case TokenType::TOKEN_BANG:
    return "!";
  case TokenType::TOKEN_GREATER:
    return ">";
  case TokenType::TOKEN_GREATER_EQUAL:
    return ">=";
  case TokenType::TOKEN_LESS:
    return "<";
  case TokenType::TOKEN_LESS_EQUAL:
    return "<=";
  case TokenType::TOKEN_EQUAL:
    return "=";
  case TokenType::TOKEN_EQUAL_EQUAL:
    return "==";
  case TokenType::TOKEN_BANG_EQUAL:
    return "!=";
  case TokenType::TOKEN_AND:
    return "and";
  case TokenType::TOKEN_OR:
    return "or";
  default:
    return "forget to add";
  }
}

const char *tokenTypeToString(TokenType type) {
  switch (type) {
  case TokenType::TOKEN_LEFT_PAREN:
    return "TOKEN_LEFT_PAREN";
  case TokenType::TOKEN_RIGHT_PAREN:
    return "TOKEN_RIGHT_PAREN";
  case TokenType::TOKEN_LEFT_BRACE:
    return "TOKEN_LEFT_BRACE";
  case TokenType::TOKEN_RIGHT_BRACE:
    return "TOKEN_RIGHT_BRACE";
  case TokenType::TOKEN_COMMA:
    return "TOKEN_COMMA";
  case TokenType::TOKEN_DOT:
    return "TOKEN_DOT";
  case TokenType::TOKEN_MINUS:
    return "TOKEN_MINUS";
  case TokenType::TOKEN_PLUS:
    return "TOKEN_PLUS";
  case TokenType::TOKEN_SEMICOLON:
    return "TOKEN_SEMICOLON";
  case TokenType::TOKEN_SLASH:
    return "TOKEN_SLASH";
  case TokenType::TOKEN_STAR:
    return "TOKEN_STAR";
  case TokenType::TOKEN_BANG:
    return "TOKEN_BANG";
  case TokenType::TOKEN_BANG_EQUAL:
    return "TOKEN_BANG_EQUAL";
  case TokenType::TOKEN_EQUAL:
    return "TOKEN_EQUAL";
  case TokenType::TOKEN_EQUAL_EQUAL:
    return "TOKEN_EQUAL_EQUAL";
  case TokenType::TOKEN_GREATER:
    return "TOKEN_GREATER";
  case TokenType::TOKEN_GREATER_EQUAL:
    return "TOKEN_GREATER_EQUAL";
  case TokenType::TOKEN_LESS:
    return "TOKEN_LESS";
  case TokenType::TOKEN_LESS_EQUAL:
    return "TOKEN_LESS_EQUAL";
  case TokenType::TOKEN_IDENTIFIER:
    return "TOKEN_IDENTIFIER";
  case TokenType::TOKEN_STRING:
    return "TOKEN_STRING";
  case TokenType::TOKEN_NUMBER:
    return "TOKEN_NUMBER";
  case TokenType::TOKEN_AND:
    return "TOKEN_AND";
  case TokenType::TOKEN_CLASS:
    return "TOKEN_CLASS";
  case TokenType::TOKEN_ELSE:
    return "TOKEN_ELSE";
  case TokenType::TOKEN_FALSE:
    return "TOKEN_FALSE";
  case TokenType::TOKEN_FOR:
    return "TOKEN_FOR";
  case TokenType::TOKEN_FUN:
    return "TOKEN_FUN";
  case TokenType::TOKEN_IF:
    return "TOKEN_IF";
  case TokenType::TOKEN_NIL:
    return "TOKEN_NIL";
  case TokenType::TOKEN_OR:
    return "TOKEN_OR";
  case TokenType::TOKEN_PRINT:
    return "TOKEN_PRINT";
  case TokenType::TOKEN_RETURN:
    return "TOKEN_RETURN";
  case TokenType::TOKEN_SUPER:
    return "TOKEN_SUPER";
  case TokenType::TOKEN_THIS:
    return "TOKEN_THIS";
  case TokenType::TOKEN_TRUE:
    return "TOKEN_TRUE";
  case TokenType::TOKEN_VAR:
    return "TOKEN_VAR";
  case TokenType::TOKEN_WHILE:
    return "TOKEN_WHILE";
  case TokenType::TOKEN_ERROR:
    return "TOKEN_ERROR";
  case TokenType::TOKEN_EOF:
    return "TOKEN_EOF";
  default:
    return "UNKNOWN";
  }
}
