#include "../src/bencoding/bencoding.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parse Bencode integers", "[bencoding]") {
  BencodeParser parser("i42e");
  auto result = parser.parse_value();

  REQUIRE(std::holds_alternative<int64_t>(result.value));
  REQUIRE(std::get<int64_t>(result.value) == 42);
}

TEST_CASE("parses Bencode negative integers", "[bencoding]") {
  BencodeParser parser("i-42e");
  auto result = parser.parse_value();

  REQUIRE(std::holds_alternative<int64_t>(result.value));
  REQUIRE(std::get<int64_t>(result.value) == -42);
}

TEST_CASE("Parse Bencode strings", "[bencoding]") {
  BencodeParser parser("5:hello");
  auto result = parser.parse_value();

  REQUIRE(std::holds_alternative<std::string>(result.value));
  REQUIRE(std::get<std::string>(result.value) == "hello");
}

TEST_CASE("Parse Bencode lists", "[bencoding]") {
  BencodeParser parser("li1e5:helloe");
  auto result = parser.parse_value();

  REQUIRE(std::holds_alternative<BencodeValue::List>(result.value));

  const auto& list = std::get<BencodeValue::List>(result.value);

  REQUIRE(list.size() == 2);
  REQUIRE(std::get<int64_t>(list[0].value) == 1);
  REQUIRE(std::get<std::string>(list[1].value) == "hello");
}

TEST_CASE("Parse Bencode dictionaries", "[bencoding]") {
  BencodeParser parser("d3:agei18e4:name4:Johne");
  auto result = parser.parse_value();

  REQUIRE(std::holds_alternative<BencodeValue::Dictionary>(result.value));

  const auto& dict = std::get<BencodeValue::Dictionary>(result.value);

  REQUIRE(dict.contains("age"));
  REQUIRE(dict.contains("name"));

  REQUIRE(std::get<int64_t>(dict.at("age").value) == 18);
  REQUIRE(std::get<std::string>(dict.at("name").value) == "John");
}