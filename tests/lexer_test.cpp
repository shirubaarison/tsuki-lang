#include "frontend/lexer/Lexer.h"
#include "frontend/token/TokenType.h"
#include <catch2/catch_test_macros.hpp>

static std::vector<Token> lex(std::string src)
{
  Lexer l(src);
  return l.scanTokens();
}

TEST_CASE("single tokens")
{
  auto t = lex("(){},.-+;*");

  REQUIRE(t[0].type == TokenType::TOKEN_LEFT_PAREN);
  REQUIRE(t[1].type == TokenType::TOKEN_RIGHT_PAREN);
  REQUIRE(t[2].type == TokenType::TOKEN_LEFT_BRACE);
  REQUIRE(t[3].type == TokenType::TOKEN_RIGHT_BRACE);
  REQUIRE(t[4].type == TokenType::TOKEN_COMMA);
  REQUIRE(t[5].type == TokenType::TOKEN_DOT);
  REQUIRE(t[6].type == TokenType::TOKEN_MINUS);
  REQUIRE(t[7].type == TokenType::TOKEN_PLUS);
  REQUIRE(t[8].type == TokenType::TOKEN_SEMICOLON);
  REQUIRE(t[9].type == TokenType::TOKEN_STAR);
}

TEST_CASE("operators")
{
  auto t = lex("! != = == < <= > >=");

  REQUIRE(t[0].type == TokenType::TOKEN_BANG);
  REQUIRE(t[1].type == TokenType::TOKEN_BANG_EQUAL);
  REQUIRE(t[2].type == TokenType::TOKEN_EQUAL);
  REQUIRE(t[3].type == TokenType::TOKEN_EQUAL_EQUAL);
  REQUIRE(t[4].type == TokenType::TOKEN_LESS);
  REQUIRE(t[5].type == TokenType::TOKEN_LESS_EQUAL);
  REQUIRE(t[6].type == TokenType::TOKEN_GREATER);
  REQUIRE(t[7].type == TokenType::TOKEN_GREATER_EQUAL);
}

TEST_CASE("numbers")
{
  auto t = lex("123 45.67");

  REQUIRE(t[0].type == TokenType::TOKEN_NUMBER);
  REQUIRE(t[0].lexeme == "123");

  REQUIRE(t[1].type == TokenType::TOKEN_NUMBER);
  REQUIRE(t[1].lexeme == "45.67");
}

TEST_CASE("identifiers and keywords")
{
  auto t = lex("if foo while true bar");

  REQUIRE(t[0].type == TokenType::TOKEN_IF);
  REQUIRE(t[1].type == TokenType::TOKEN_IDENTIFIER);
  REQUIRE(t[1].lexeme == "foo");

  REQUIRE(t[2].type == TokenType::TOKEN_WHILE);
  REQUIRE(t[3].type == TokenType::TOKEN_TRUE);

  REQUIRE(t[4].type == TokenType::TOKEN_IDENTIFIER);
  REQUIRE(t[4].lexeme == "bar");
}

TEST_CASE("string literal")
{
  auto t = lex("\"hello\"");

  REQUIRE(t[0].type == TokenType::TOKEN_STRING);
  REQUIRE(t[0].lexeme == "hello");
}

TEST_CASE("unterminated string")
{
  auto t = lex("\"hello");

  REQUIRE(t[0].type == TokenType::TOKEN_ERROR);
}

TEST_CASE("comments are ignored")
{
  auto t = lex("123 // comment\n456");

  REQUIRE(t[0].lexeme == "123");
  REQUIRE(t[1].lexeme == "456");
}

TEST_CASE("whitespace and newlines")
{
  auto t = lex(" \n\t123\n456");

  REQUIRE(t[0].lexeme == "123");
  REQUIRE(t[1].lexeme == "456");
  REQUIRE(t[1].line == 3);
}

TEST_CASE("whitespace is ignored")
{
  std::vector<std::string> inputs = {
    "123",
    " 123",
    "123 ",
    "  123  ",
    "\t123",
    "\n123",
    " \n\t 123",
  };

  for (auto& input : inputs)
  {
    auto tokens = lex(input);
    REQUIRE(tokens.size() == 2); // two because of EOF
    REQUIRE(tokens[0].lexeme == "123");
    REQUIRE(tokens[0].type == TokenType::TOKEN_NUMBER);
  }
}

TEST_CASE("unexpected character")
{
  auto t = lex("@");

  REQUIRE(t[0].type == TokenType::TOKEN_ERROR);
}
