#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <unordered_map>
#include <vector>

typedef enum {
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,

  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,

  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,

  TOKEN_AND,
  TOKEN_CLASS,
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FOR,
  TOKEN_FUN,
  TOKEN_IF,
  TOKEN_NIL,
  TOKEN_OR,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_SUPER,
  TOKEN_THIS,
  TOKEN_TRUE,
  TOKEN_VAR,
  TOKEN_WHILE,

  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

const char *tokenTypeToString(TokenType type);

typedef struct {
  TokenType type;
  std::string lexeme;
  int line;
} Token;

class Lexer {
public:
  Lexer(std::string source);
  std::vector<Token> scanTokens();

private:
  std::string source;
  std::string::iterator start;
  std::string::iterator current;

  std::unordered_map<std::string, TokenType> keywords;

  int line;

  std::vector<Token> tokens;
 
  void skipWhitespace();

  TokenType identifierType();

  Token makeToken(TokenType type);
  Token errorToken(std::string message);
  Token scanToken();
  Token identifier();
  Token string();
  Token number();

  bool isAtEnd();
  bool match(char expected);

  char peek();
  char peekNext();
  char advance();
};

#endif // !LEXER_H
