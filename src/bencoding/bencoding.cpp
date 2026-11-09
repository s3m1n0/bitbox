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
BencodeValue BencodeParser::parse_string() {
  std::string length_string{};

  // Read the length until ':'
  while (!at_end() && peek() != ':') {
    length_string += consume();
  }

  if (at_end() || length_string.empty()) {
    throw std::runtime_error("invalid string length");
  }

  consume();  // ':'

  std::size_t length{};

  auto [ptr, ec] =
      std::from_chars(length_string.data(),
                      length_string.data() + length_string.size(), length);

  if (ec != std::errc{} || ptr != length_string.data() + length_string.size()) {
    throw std::runtime_error("invalid string length");
  }

  // Check whether enough input remains
  if (length > m_input.size() - m_position) {
    throw std::runtime_error("unexpected end of input");
  }

  // Extract the string and advance the parser
  std::string result{m_input.substr(m_position, length)};
  m_position += length;

  return BencodeValue{std::move(result)};
}

BencodeValue BencodeParser::parse_dictionary() { return BencodeValue{}; }