#include "bencoding.hpp"

#include <charconv>
#include <format>
#include <iostream>
#include <stdexcept>

BencodeValue BencodeParser::parse_value() {
  if (at_end()) {
    throw std::runtime_error("unexpected end of input");
  }

  switch (peek()) {
    case 'i':
      return parse_integer();

    case 'l':
      return parse_list();

    case 'd':
      return parse_dictionary();

    default:
      if (std::isdigit(static_cast<unsigned char>(peek()))) {
        return parse_string();
      }

      throw std::runtime_error("invalid bencoded value");
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

  int64_t value{};

  auto [ptr, ec] = std::from_chars(
      int_string.data(), int_string.data() + int_string.size(), value);

  if (ec != std::errc{} || ptr != int_string.data() + int_string.size()) {
    throw std::runtime_error("invalid integer");
  }

  return BencodeValue{value};
}

BencodeValue BencodeParser::parse_list() {
  std::vector<BencodeValue> list{};

  consume();  // l

  while (!at_end() && peek() != 'e') {
    list.push_back(parse_value());
  }

  if (at_end()) {
    throw std::runtime_error("unterminated list");
  }

  consume();  // e

  return BencodeValue{std::move(list)};
}

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

BencodeValue BencodeParser::parse_dictionary() {
  BencodeValue::Dictionary dictionary{};

  consume();  // d

  while (!at_end() && peek() != 'e') {
    // Dictionary keys must be strings
    if (!std::isdigit(static_cast<unsigned char>(peek()))) {
      throw std::runtime_error("Expected string as dictionary key");
    }

    std::string key = std::get<std::string>(parse_string().value);

    // Parse the corresponding value
    BencodeValue value = parse_value();

    auto [it, inserted] = dictionary.emplace(std::move(key), std::move(value));

    if (!inserted) {
      throw std::runtime_error("Duplicate dictionary key");
    }
  }

  if (at_end()) {
    throw std::runtime_error("Unterminated dictionary");
  }

  consume();  // e

  return BencodeValue{std::move(dictionary)};
}

BencodeValue parse(std::string_view raw_data) {
  BencodeParser parser(raw_data);
  BencodeValue value = parser.parse_value();

  if (!parser.at_end()) {
    throw std::runtime_error("trailing data after parse");
  }

  return value;
}