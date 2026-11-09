#include "bencoding.hpp"

#include <charconv>
#include <format>
#include <iostream>
#include <stdexcept>

BencodeValue BencodeParser::parse_value() {
  switch (peek()) {
    case 'i':
      return parse_integer();

    case 'l':
      return parse_list();

    case 'd':
      return parse_dictionary();

    default:
      if (std::isdigit(peek())) {
        return parse_string();
      }

      std::cout << std::format("invalid character: {}\n", peek());
      exit(1);
  }
}

BencodeValue BencodeParser::parse_integer() {
  consume();  // i

  std::string int_string{};

  while (peek() != 'e' && !at_end()) {
    int_string += consume();
  }

  if (at_end() || int_string.empty()) {
    throw std::runtime_error("invalid integer");
  }

  consume();  // e

  int value{};

  auto [ptr, ec] = std::from_chars(
      int_string.data(), int_string.data() + int_string.size(), value);

  if (ec != std::errc{} || ptr != int_string.data() + int_string.size()) {
    throw std::runtime_error("invalid integer");
  }

  return BencodeValue{value};
}
BencodeValue BencodeParser::parse_list() { return BencodeValue{}; }
BencodeValue BencodeParser::parse_string() { return BencodeValue{}; }
BencodeValue BencodeParser::parse_dictionary() { return BencodeValue{}; }