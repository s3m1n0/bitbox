#include "bencoding.hpp"

#include <format>
#include <iostream>

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
  return BencodeValue{};
}

BencodeValue BencodeParser::parse_integer() { return BencodeValue{}; }
BencodeValue BencodeParser::parse_list() { return BencodeValue{}; }
BencodeValue BencodeParser::parse_string() { return BencodeValue{}; }
BencodeValue BencodeParser::parse_dictionary() { return BencodeValue{}; }