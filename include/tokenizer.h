#pragma once

#include <string>
#include <vector>

enum class TokenType {
  SERVICE,
  RPC,
  IDENTIFIER,
  I64,
  STRING,
  LBRACE,
  RBRACE,
  LPAREN,
  RPAREN,
  ARROW,
  SEMICOLON,
  COMMA,
  END_OF_FILE,
};

struct Token {
  TokenType type;
  std::string text;
  int line;
};

struct Tokenizer {
  std::vector<Token> tokenize(const std::string &input);
};