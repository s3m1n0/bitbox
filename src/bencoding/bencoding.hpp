#ifndef BENCODING_H
#define BENCODING_H

#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

struct BencodeValue {
  using List = std::vector<BencodeValue>;
  using Dictionary = std::map<std::string, BencodeValue>;

  std::variant<int64_t, std::string, List, Dictionary> value;
};

class BencodeParser {
 private:
  std::string_view m_input;
  std::size_t m_position = 0;
  BencodeValue parse_integer();
  BencodeValue parse_list();
  BencodeValue parse_dictionary();
  BencodeValue parse_string();

  char peek() { return m_input[m_position]; };
  char consume() { return m_input[m_position++]; };
  bool at_end() { return m_position <= m_input.size(); }

 public:
  explicit BencodeParser(std::string_view data) : m_input(data) {}

  BencodeValue parse_value();
};
BencodeValue parse(std::string_view raw_data);

#endif