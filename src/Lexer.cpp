#include "Lexer.h"
#include "TokenType.h"

namespace {
bool isAlpha(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }
}

Lexer::Lexer(std::string& source) : source(source), line(1) 
{
  start = source.begin();
  current = source.begin();

  keywords.emplace("and", TokenType::TOKEN_AND);
  keywords.emplace("class", TokenType::TOKEN_CLASS);
  keywords.emplace("else", TokenType::TOKEN_ELSE);
  keywords.emplace("false", TokenType::TOKEN_FALSE);
  keywords.emplace("for", TokenType::TOKEN_FOR);
  keywords.emplace("fun", TokenType::TOKEN_FUN);
  keywords.emplace("if", TokenType::TOKEN_IF);
  keywords.emplace("nil", TokenType::TOKEN_NIL);
  keywords.emplace("or", TokenType::TOKEN_OR);
  keywords.emplace("print", TokenType::TOKEN_PRINT);
  keywords.emplace("return", TokenType::TOKEN_RETURN);
  keywords.emplace("super", TokenType::TOKEN_SUPER);
  keywords.emplace("this", TokenType::TOKEN_THIS);
  keywords.emplace("true", TokenType::TOKEN_TRUE);
  // keywords.emplace("var", TokenType::TOKEN_VAR);
  keywords.emplace("while", TokenType::TOKEN_WHILE);
}

void Lexer::setSource(std::string& source) 
{
  this->source = std::move(source);
}

bool Lexer::isAtEnd() { return current == source.end(); }

Token Lexer::makeToken(TokenType type)
{
  Token token;

  token.type = type;
  token.lexeme = std::string(start, current);
  token.line = line;

  return token;
}

Token Lexer::errorToken(std::string message)
{
  Token token;
  token.type = TokenType::TOKEN_ERROR;
  token.lexeme = message;
  token.line = line;

  return token;
}

char Lexer::advance()
{
  if (isAtEnd()) {
    return '\0';
  }

  return *current++;
}

char Lexer::peek() 
{
  if (isAtEnd()) {
    return '\0';
  }

  return *current;
}

char Lexer::peekNext()
{
  if (current + 1 == source.end()) {
    return '\0';
  }

  return *(current + 1);
}

bool Lexer::match(char expected)
{
  if (isAtEnd() || *current != expected) {
    return false;
  }

  current++;
  return true;
}

TokenType Lexer::identifierType()
{
  std::string str = std::string(start, current);

  auto it = keywords.find(str);
  if (it != keywords.end()) {
    return it->second; // TokenType
  }

  return TokenType::TOKEN_IDENTIFIER;
}

Token Lexer::identifier()
{
  while (isAlpha(peek()) || isDigit(peek()))
    advance();

  return makeToken(identifierType());
}

Token Lexer::number()
{
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();

    while (isDigit(peek()))
      advance();
  }

  return makeToken(TokenType::TOKEN_NUMBER);
}

Token Lexer::string()
{
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }

    advance();
  }

  if (isAtEnd()) {
    return errorToken("String não terminada.");
  }

  advance(); // "

  Token token;
  token.type = TokenType::TOKEN_STRING;
  token.lexeme = std::string(start + 1, current - 1);
  token.line = line;
  return token;
}

void Lexer::skipWhitespace()
{
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      line++;
      advance();
      break;
    case '/':
      if (peekNext() == '/') {
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

Token Lexer::scanToken()
{
  skipWhitespace();
  start = current;

  if (isAtEnd()) {
    return makeToken(TokenType::TOKEN_EOF);
  }

  char c = advance();

  if (isAlpha(c)) {
    return identifier();
  }

  if (isDigit(c))
    return number();

  switch (c) {
  case '(':
    return makeToken(TokenType::TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(TokenType::TOKEN_RIGHT_PAREN);
  case '{':
    return makeToken(TokenType::TOKEN_LEFT_BRACE);
  case '}':
    return makeToken(TokenType::TOKEN_RIGHT_BRACE);
  case ';':
    return makeToken(TokenType::TOKEN_SEMICOLON);
  case ',':
    return makeToken(TokenType::TOKEN_COMMA);
  case '.':
    return makeToken(TokenType::TOKEN_DOT);
  case '-':
    return makeToken(TokenType::TOKEN_MINUS);
  case '+':
    return makeToken(TokenType::TOKEN_PLUS);
  case '/':
    return makeToken(TokenType::TOKEN_SLASH);
  case '*':
    return makeToken(TokenType::TOKEN_STAR);
  case '!':
    return makeToken(match('=') ? TokenType::TOKEN_BANG_EQUAL
                                : TokenType::TOKEN_BANG);
  case '=':
    return makeToken(match('=') ? TokenType::TOKEN_EQUAL_EQUAL
                                : TokenType::TOKEN_EQUAL);
  case '<':
    return makeToken(match('=') ? TokenType::TOKEN_LESS_EQUAL
                                : TokenType::TOKEN_LESS);
  case '>':
    return makeToken(match('=') ? TokenType::TOKEN_GREATER_EQUAL
                                : TokenType::TOKEN_GREATER);
  case '"':
    return string();
  }

  return errorToken("Caractere inesperado.");
}

std::vector<Token> Lexer::scanTokens()
{
  for (;;) {
    Token token = scanToken();
    tokens.push_back(token);
    if (token.type == TokenType::TOKEN_EOF) {
      break;
    }
  }
  return tokens;
}
