#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "TokenType.h"

const char *tokenToOperator(TokenType type);
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

  void setSource(std::string& source);

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
