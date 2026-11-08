#include "../src/bencoding/bencoding.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parse Bencode integers", "[bencoding]") {
  auto result = parse("i42e");

  REQUIRE(std::holds_alternative<int64_t>(result.value));
  REQUIRE(std::get<int64_t>(result.value) == 42);
}

TEST_CASE("Parse Bencode strings", "[bencoding]") {
  auto result = parse("5:hello");

  REQUIRE(std::holds_alternative<std::string>(result.value));
  REQUIRE(std::get<std::string>(result.value) == "hello");
}

TEST_CASE("Parse Bencode lists", "[bencoding]") {
  auto result = parse("li1e5:helloe");

  REQUIRE(std::holds_alternative<BencodeValue::List>(result.value));

  const auto& list = std::get<BencodeValue::List>(result.value);

  REQUIRE(list.size() == 2);
  REQUIRE(std::get<int64_t>(list[0].value) == 1);
  REQUIRE(std::get<std::string>(list[1].value) == "hello");
}

TEST_CASE("Parse Bencode dictionaries", "[bencoding]") {
  auto result = parse("d3:agei18e4:name4:Johne");

  REQUIRE(std::holds_alternative<BencodeValue::Dictionary>(result.value));

  const auto& dict = std::get<BencodeValue::Dictionary>(result.value);

  REQUIRE(dict.contains("age"));
  REQUIRE(dict.contains("name"));

  REQUIRE(std::get<int64_t>(dict.at("age").value) == 18);
  REQUIRE(std::get<std::string>(dict.at("name").value) == "John ");
}