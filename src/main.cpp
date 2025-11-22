#include <fstream>
#include <iostream>
#include <mstch/mstch.hpp>
#include <mstch_gen.h>
#include <parser.h>
#include <sstream>

using namespace toyrpc;

std::string readFileToString(const std::string &file_path) {
  std::ifstream file_stream(file_path);
  if (!file_stream.is_open()) {
    throw std::runtime_error("Could not open file: " + file_path);
  }
  std::stringstream buffer;
  buffer << file_stream.rdbuf();
  return buffer.str();
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <idl_file>" << std::endl;
    return 1;
  }
  std::string idl_content = readFileToString(argv[1]);
  Tokenizer tokenizer;
  auto tokens = tokenizer.tokenize(idl_content);
  Parser parser(tokens);
  auto services = parser.parse();

  mstch::map context = convertServiceToMstch(services[0]);
  std::string serde_h_template_str =
      readFileToString("template/serde.h.mustache");
  std::string serde_cpp_template_str =
      readFileToString("template/serde.cpp.mustache");

  std::string serde_h_code = mstch::render(serde_h_template_str, context);
  std::string serde_cpp_code = mstch::render(serde_cpp_template_str, context);
  std::cout << serde_h_code;
  std::cout << serde_cpp_code;
  return 0;
}