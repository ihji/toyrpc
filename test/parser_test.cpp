#include <gtest/gtest.h>
#include <parser.h>

namespace toyrpc {
TEST(ParserTest, BasicParsing) {
  Tokenizer tokenizer;
  std::string input = R"(
        service Calculator {
        rpc Add(i64 a, i64 b) -> i64;
        rpc Concat(string a, string b) -> string;
        }
    )";

  auto tokens = tokenizer.tokenize(input);
  Parser parser(tokens);
  auto services = parser.parse();

  ASSERT_EQ(services.size(), 1);
  const auto &service = services[0];
  EXPECT_EQ(service.service_name, "Calculator");
  ASSERT_EQ(service.methods.size(), 2);

  const auto &method1 = service.methods[0];
  EXPECT_EQ(method1.method_name, "Add");
  ASSERT_EQ(method1.params.size(), 2);
  EXPECT_EQ(method1.params[0].type, Type::I64);
  EXPECT_EQ(method1.params[0].name, "a");
  EXPECT_EQ(method1.params[1].type, Type::I64);
  EXPECT_EQ(method1.params[1].name, "b");
  EXPECT_EQ(method1.return_type, Type::I64);

  const auto &method2 = service.methods[1];
  EXPECT_EQ(method2.method_name, "Concat");
  ASSERT_EQ(method2.params.size(), 2);
  EXPECT_EQ(method2.params[0].type, Type::STRING);
  EXPECT_EQ(method2.params[0].name, "a");
  EXPECT_EQ(method2.params[1].type, Type::STRING);
  EXPECT_EQ(method2.params[1].name, "b");
  EXPECT_EQ(method2.return_type, Type::STRING);
}
} // namespace toyrpc