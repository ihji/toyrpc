#include <ast.h>
#include <parser.h>

Parser::Parser(const std::vector<Token> &tokens)
    : cur_t_idx_(0), tokens_(tokens) {}

std::vector<Service> Parser::parse() {
  std::vector<Service> services;
  while (peek().type != TokenType::END_OF_FILE) {
    services.push_back(parse_service());
  }
  return services;
}

Param Parser::parse_param() {
  Type param_type = parse_type();
  Token name_token = expect(TokenType::IDENTIFIER);
  return Param{param_type, name_token.text};
}

Type Parser::parse_type() {
  Token type_token = consume();
  switch (type_token.type) {
  case TokenType::I64:
    return Type::I64;
  case TokenType::STRING:
    return Type::STRING;
  default:
    throw std::runtime_error("Unexpected type token");
  }
}

Method Parser::parse_method() {
  expect(TokenType::RPC);
  Token method_name_token = expect(TokenType::IDENTIFIER);
  expect(TokenType::LPAREN);
  std::vector<Param> params;
  while (peek().type != TokenType::RPAREN) {
    params.push_back(parse_param());
    if (peek().type == TokenType::COMMA) {
      consume(); // consume COMMA
    }
  }
  expect(TokenType::RPAREN);
  expect(TokenType::ARROW);
  Type return_type = parse_type();
  expect(TokenType::SEMICOLON);
  return Method{method_name_token.text, params, return_type};
}

Service Parser::parse_service() {
  expect(TokenType::SERVICE);
  Token service_name_token = expect(TokenType::IDENTIFIER);
  expect(TokenType::LBRACE);
  std::vector<Method> methods;
  while (peek().type != TokenType::RBRACE) {
    methods.push_back(parse_method());
  }
  expect(TokenType::RBRACE);
  return Service{service_name_token.text, methods};
}

Token Parser::expect(TokenType expected_type) {
  if (cur_t_idx_ < tokens_.size()) {
    Token token = tokens_[cur_t_idx_];
    if (token.type == expected_type) {
      ++cur_t_idx_;
      return token;
    }
    throw std::runtime_error("Unexpected token type");
  }
  throw std::runtime_error("Unexpected end of tokens");
}

Token Parser::consume() {
  if (cur_t_idx_ < tokens_.size()) {
    return tokens_[cur_t_idx_++];
  }
  throw std::runtime_error("Unexpected end of tokens");
}

Token Parser::peek() {
  if (cur_t_idx_ < tokens_.size()) {
    return tokens_[cur_t_idx_];
  }
  throw std::runtime_error("Unexpected end of tokens");
}
