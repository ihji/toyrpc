#include <gtest/gtest.h>
#include <tokenizer.h>

TEST(TokenizerTest, BasicTokenization) {
  Tokenizer tokenizer;
  std::string input = R"(
    service Calculator {
      rpc Add(i64 a, i64 b) -> i64;
      rpc Concat(string a, string b) -> string;
    }
  )";

  auto tokens = tokenizer.tokenize(input);

  std::vector<TokenType> expected_types = {
      TokenType::SERVICE, TokenType::IDENTIFIER, TokenType::LBRACE,
      TokenType::RPC,     TokenType::IDENTIFIER, TokenType::LPAREN,
      TokenType::I64,     TokenType::IDENTIFIER, TokenType::COMMA,
      TokenType::I64,     TokenType::IDENTIFIER, TokenType::RPAREN,
      TokenType::ARROW,   TokenType::I64,        TokenType::SEMICOLON,
      TokenType::RPC,     TokenType::IDENTIFIER, TokenType::LPAREN,
      TokenType::STRING,  TokenType::IDENTIFIER, TokenType::COMMA,
      TokenType::STRING,  TokenType::IDENTIFIER, TokenType::RPAREN,
      TokenType::ARROW,   TokenType::STRING,     TokenType::SEMICOLON,
      TokenType::RBRACE,  TokenType::END_OF_FILE};

  ASSERT_EQ(tokens.size(), expected_types.size());

  for (size_t i = 0; i < tokens.size(); ++i) {
    EXPECT_EQ(tokens[i].type, expected_types[i])
        << "Token " << i << " type mismatch";
  }
}
