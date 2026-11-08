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

BencodeValue parse(std::string_view raw_data);

#endif