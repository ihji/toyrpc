#include <tokenizer.h>

namespace toyrpc {
std::vector<Token> Tokenizer::tokenize(const std::string &input) {
  std::vector<Token> tokens;
  int line = 1;
  size_t i = 0;
  while (i < input.size()) {
    char c = input[i];
    if (isspace(c)) {
      if (c == '\n') {
        line++;
      }
      i++;
      continue;
    }

    if (isalpha(c)) {
      size_t start = i;
      while (i < input.size() && (isalnum(input[i]) || input[i] == '_')) {
        i++;
      }
      std::string word = input.substr(start, i - start);
      if (word == "service") {
        tokens.push_back({TokenType::SERVICE, word, line});
      } else if (word == "rpc") {
        tokens.push_back({TokenType::RPC, word, line});
      } else if (word == "i64") {
        tokens.push_back({TokenType::I64, word, line});
      } else if (word == "string") {
        tokens.push_back({TokenType::STRING, word, line});
      } else {
        tokens.push_back({TokenType::IDENTIFIER, word, line});
      }
      continue;
    }

    switch (c) {
    case '{':
      tokens.push_back({TokenType::LBRACE, "{", line});
      break;
    case '}':
      tokens.push_back({TokenType::RBRACE, "}", line});
      break;
    case '(':
      tokens.push_back({TokenType::LPAREN, "(", line});
      break;
    case ')':
      tokens.push_back({TokenType::RPAREN, ")", line});
      break;
    case '-':
      if (i + 1 < input.size() && input[i + 1] == '>') {
        tokens.push_back({TokenType::ARROW, "->", line});
        i++;
      }
      break;
    case ';':
      tokens.push_back({TokenType::SEMICOLON, ";", line});
      break;
    case ',':
      tokens.push_back({TokenType::COMMA, ",", line});
      break;
    default:
      throw std::runtime_error("Unexpected character: " + std::string(1, c));
    }
    i++;
  }
  tokens.push_back({TokenType::END_OF_FILE, "", line});
  return tokens;
}
} // namespace toyrpc