#include "bittorent.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "../bencoding/bencoding.hpp"

namespace {

const BencodeValue::Dictionary& as_dictionary(const BencodeValue& value,
                                              const std::string& context) {
  if (const auto* dictionary =
          std::get_if<BencodeValue::Dictionary>(&value.value)) {
    return *dictionary;
  }

  throw std::runtime_error(context + " must be a dictionary");
}

const BencodeValue* find_value(const BencodeValue::Dictionary& dictionary,
                               const std::string& key) {
  const auto it = dictionary.find(key);
  return it == dictionary.end() ? nullptr : &it->second;
}

std::string get_required_string(const BencodeValue::Dictionary& dictionary,
                                const std::string& key) {
  const auto* value = find_value(dictionary, key);
  if (value == nullptr) {
    throw std::runtime_error("missing required field: " + key);
  }

  if (const auto* string_value = std::get_if<std::string>(&value->value)) {
    return *string_value;
  }

  throw std::runtime_error("field '" + key + "' must be a string");
}

std::int64_t get_required_integer(const BencodeValue::Dictionary& dictionary,
                                  const std::string& key) {
  const auto* value = find_value(dictionary, key);
  if (value == nullptr) {
    throw std::runtime_error("missing required field: " + key);
  }

  if (const auto* integer = std::get_if<std::int64_t>(&value->value)) {
    return *integer;
  }

  throw std::runtime_error("field '" + key + "' must be an integer");
}

std::vector<std::string> flatten_tracker_urls(const BencodeValue& value) {
  const auto* list = std::get_if<BencodeValue::List>(&value.value);
  if (list == nullptr) {
    throw std::runtime_error("announce-list must be a list");
  }

  std::vector<std::string> urls;

  for (const auto& entry : *list) {
    if (const auto* string_value = std::get_if<std::string>(&entry.value)) {
      urls.push_back(*string_value);
      continue;
    }

    if (const auto* nested_list =
            std::get_if<BencodeValue::List>(&entry.value)) {
      for (const auto& nested_entry : *nested_list) {
        if (const auto* nested_string =
                std::get_if<std::string>(&nested_entry.value)) {
          urls.push_back(*nested_string);
          continue;
        }
        throw std::runtime_error(
            "announce-list entries must be strings or lists");
      }
      continue;
    }

    throw std::runtime_error("announce-list entries must be strings or lists");
  }

  return urls;
}

std::vector<std::string> parse_pieces(const std::string& pieces) {
  if (pieces.empty()) {
    return {};
  }

  if (pieces.size() % 20 != 0) {
    throw std::runtime_error("invalid pieces length");
  }

  std::vector<std::string> result;
  result.reserve(pieces.size() / 20);

  for (std::size_t index = 0; index < pieces.size(); index += 20) {
    result.emplace_back(pieces.substr(index, 20));
  }

  return result;
}

std::vector<std::string> parse_path(const BencodeValue& value) {
  const auto* list = std::get_if<BencodeValue::List>(&value.value);
  if (list == nullptr) {
    throw std::runtime_error("path must be a list");
  }

  std::vector<std::string> path;
  path.reserve(list->size());

  for (const auto& part : *list) {
    if (const auto* string_value = std::get_if<std::string>(&part.value)) {
      path.push_back(*string_value);
      continue;
    }

    throw std::runtime_error("path entries must be strings");
  }

  return path;
}

}  // namespace

BittorrentConfig bittorentConfigFactory(const BencodeValue& value) {
  const auto& root = as_dictionary(value, "torrent root");

  const auto* announce_value = find_value(root, "announce");
  if (announce_value == nullptr) {
    throw std::runtime_error("missing required field: announce");
  }

  const auto* announce = std::get_if<std::string>(&announce_value->value);
  if (announce == nullptr) {
    throw std::runtime_error("field 'announce' must be a string");
  }

  std::vector<std::string> announce_list;
  const auto* announce_list_value = find_value(root, "announce-list");
  if (announce_list_value != nullptr) {
    announce_list = flatten_tracker_urls(*announce_list_value);
  }

  const auto* info_value = find_value(root, "info");
  if (info_value == nullptr) {
    throw std::runtime_error("missing required field: info");
  }

  const auto& info_dictionary = as_dictionary(*info_value, "info");

  BittorrentInfo info;
  info.name = get_required_string(info_dictionary, "name");
  info.piece_length = static_cast<std::size_t>(
      get_required_integer(info_dictionary, "piece length"));
  if (info.piece_length == 0) {
    throw std::runtime_error("piece length must be positive");
  }

  const auto* pieces_value = find_value(info_dictionary, "pieces");
  if (pieces_value == nullptr) {
    throw std::runtime_error("missing required field: pieces");
  }

  const auto* pieces_string = std::get_if<std::string>(&pieces_value->value);
  if (pieces_string == nullptr) {
    throw std::runtime_error("field 'pieces' must be a string");
  }

  info.pieces = parse_pieces(*pieces_string);

  const auto* length_value = find_value(info_dictionary, "length");
  if (length_value != nullptr) {
    const auto* length = std::get_if<std::int64_t>(&length_value->value);
    if (length == nullptr) {
      throw std::runtime_error("field 'length' must be an integer");
    }
    info.length = static_cast<std::size_t>(*length);
  }

  const auto* files_value = find_value(info_dictionary, "files");
  if (files_value != nullptr) {
    const auto* files = std::get_if<BencodeValue::List>(&files_value->value);
    if (files == nullptr) {
      throw std::runtime_error("field 'files' must be a list");
    }

    for (const auto& file_entry : *files) {
      const auto& file_dictionary = as_dictionary(file_entry, "file entry");

      BittorrentFileInfo file;
      file.length = static_cast<std::size_t>(
          get_required_integer(file_dictionary, "length"));

      const auto* path_value = find_value(file_dictionary, "path");
      if (path_value == nullptr) {
        throw std::runtime_error("missing required field: path");
      }
      file.path = parse_path(*path_value);

      info.files.push_back(file);
    }
  }

  BittorrentConfig config(*announce, std::move(announce_list), info.name);
  config.info = std::move(info);
  return config;
}
