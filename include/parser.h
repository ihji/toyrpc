#pragma once

#include <ast.h>
#include <tokenizer.h>
#include <vector>

namespace toyrpc {
class Parser {
public:
  Parser(const std::vector<Token> &tokens);
  std::vector<Service> parse();

private:
  size_t cur_t_idx_;
  std::vector<Token> tokens_;

  Service parse_service();
  Method parse_method();
  Param parse_param();
  Type parse_type();

  Token expect(TokenType expected_type);
  Token consume();
  Token peek();
};
} // namespace toyrpc