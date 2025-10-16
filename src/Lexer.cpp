#include "Lexer.h"

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

Lexer::Lexer(std::string source)
    : source(std::move(source)),
      start(this->source.begin()),
      current(this->source.begin()),
      line(1) {
  keywords.emplace("and", TOKEN_AND);
  keywords.emplace("class", TOKEN_CLASS);
  keywords.emplace("else", TOKEN_ELSE);
  keywords.emplace("false", TOKEN_FALSE);
  keywords.emplace("for", TOKEN_FOR);
  keywords.emplace("fun", TOKEN_FUN);
  keywords.emplace("if", TOKEN_IF);
  keywords.emplace("nil", TOKEN_NIL);
  keywords.emplace("or", TOKEN_OR);
  keywords.emplace("print", TOKEN_PRINT);
  keywords.emplace("return", TOKEN_RETURN);
  keywords.emplace("super", TOKEN_SUPER);
  keywords.emplace("this", TOKEN_THIS);
  keywords.emplace("true", TOKEN_TRUE);
  keywords.emplace("var", TOKEN_VAR);
  keywords.emplace("while", TOKEN_WHILE);
}

bool Lexer::isAtEnd() { return current == source.end(); }

Token Lexer::makeToken(TokenType type) {
  Token token;

  token.type = type;
  token.lexeme = std::string(start, current);
  token.line = line;

  return token;
}

Token Lexer::errorToken(std::string message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.lexeme = message;
  token.line = line;

  return token;
}

char Lexer::advance() {
  if (isAtEnd()) return '\0';

  return *current++;
}

char Lexer::peek() {
  if (isAtEnd()) return '\0';

  return *current;
}

char Lexer::peekNext() {
  if (current + 1 == source.end())
    return '\0';

  return *(current + 1);
}

bool Lexer::match(char expected) {
  if (isAtEnd())
    return false;
  if (*current != expected)
    return false;

  current++;
  return true;
}

TokenType Lexer::identifierType() {
  std::string str = std::string(start, current);

  auto it = keywords.find(str);
  if (it != keywords.end()) {
    return it->second;  // TokenType
  }

  return TOKEN_IDENTIFIER;
}

Token Lexer::identifier() {
  while (isAlpha(peek()) || isDigit(peek()))
    advance();

  return makeToken(identifierType());
}

Token Lexer::number() {
  while (isDigit(peek())) 
    advance();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();

    while (isDigit(peek())) 
      advance();
  }

  return makeToken(TOKEN_NUMBER);
}

Token Lexer::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    
    advance();
  }

  if (isAtEnd())
    return errorToken("String não terminada.");

  advance(); // "

  return makeToken(TOKEN_STRING);
}

void Lexer::skipWhitespace() {
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

Token Lexer::scanToken() {
  skipWhitespace();
  start = current;

  if (isAtEnd())
    return makeToken(TOKEN_EOF);

  char c = advance();

  if (isAlpha(c))
    return identifier();

  if (isDigit(c))
    return number();

  switch (c) {
    case '(':
      return makeToken(TOKEN_LEFT_PAREN);
    case ')':
      return makeToken(TOKEN_RIGHT_PAREN);
    case '{':
      return makeToken(TOKEN_LEFT_BRACE);
    case '}':
      return makeToken(TOKEN_RIGHT_BRACE);
    case ';':
      return makeToken(TOKEN_SEMICOLON);
    case ',':
      return makeToken(TOKEN_COMMA);
    case '.':
      return makeToken(TOKEN_DOT);
    case '-':
      return makeToken(TOKEN_MINUS);
    case '+':
      return makeToken(TOKEN_PLUS);
    case '/':
      return makeToken(TOKEN_SLASH);
    case '*':
      return makeToken(TOKEN_STAR);
    case '!':
      return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
      return string();
  }
  
  return errorToken("Caractere inesperado.");
}

std::vector<Token> Lexer::scanTokens() {
  for (;;) {
    Token token = scanToken();
    tokens.push_back(token);
    if (token.type == TOKEN_EOF) break;
  }

  return tokens;
}

const char* tokenTypeToString(TokenType type) {
  switch (type) {
    case TOKEN_LEFT_PAREN: return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN: return "TOKEN_RIGHT_PAREN";
    case TOKEN_LEFT_BRACE: return "TOKEN_LEFT_BRACE";
    case TOKEN_RIGHT_BRACE: return "TOKEN_RIGHT_BRACE";
    case TOKEN_COMMA: return "TOKEN_COMMA";
    case TOKEN_DOT: return "TOKEN_DOT";
    case TOKEN_MINUS: return "TOKEN_MINUS";
    case TOKEN_PLUS: return "TOKEN_PLUS";
    case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
    case TOKEN_SLASH: return "TOKEN_SLASH";
    case TOKEN_STAR: return "TOKEN_STAR";
    case TOKEN_BANG: return "TOKEN_BANG";
    case TOKEN_BANG_EQUAL: return "TOKEN_BANG_EQUAL";
    case TOKEN_EQUAL: return "TOKEN_EQUAL";
    case TOKEN_EQUAL_EQUAL: return "TOKEN_EQUAL_EQUAL";
    case TOKEN_GREATER: return "TOKEN_GREATER";
    case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";
    case TOKEN_LESS: return "TOKEN_LESS";
    case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";
    case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
    case TOKEN_STRING: return "TOKEN_STRING";
    case TOKEN_NUMBER: return "TOKEN_NUMBER";
    case TOKEN_AND: return "TOKEN_AND";
    case TOKEN_CLASS: return "TOKEN_CLASS";
    case TOKEN_ELSE: return "TOKEN_ELSE";
    case TOKEN_FALSE: return "TOKEN_FALSE";
    case TOKEN_FOR: return "TOKEN_FOR";
    case TOKEN_FUN: return "TOKEN_FUN";
    case TOKEN_IF: return "TOKEN_IF";
    case TOKEN_NIL: return "TOKEN_NIL";
    case TOKEN_OR: return "TOKEN_OR";
    case TOKEN_PRINT: return "TOKEN_PRINT";
    case TOKEN_RETURN: return "TOKEN_RETURN";
    case TOKEN_SUPER: return "TOKEN_SUPER";
    case TOKEN_THIS: return "TOKEN_THIS";
    case TOKEN_TRUE: return "TOKEN_TRUE";
    case TOKEN_VAR: return "TOKEN_VAR";
    case TOKEN_WHILE: return "TOKEN_WHILE";
    case TOKEN_ERROR: return "TOKEN_ERROR";
    case TOKEN_EOF: return "TOKEN_EOF";
    default: return "UNKNOWN";
  }
}
