#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../bencoding/bencoding.hpp"

struct BittorrentFileInfo {
  std::size_t length{};
  std::vector<std::string> path;
};

struct BittorrentInfo {
  std::string name;
  std::size_t piece_length{};
  std::vector<std::string> pieces;

  // Single-file or multi-file torrent
  std::size_t length{};
  std::vector<BittorrentFileInfo> files;
};

class BittorrentConfig {
 public:
  BittorrentConfig(std::string announce, std::vector<std::string> announce_list,
                   std::string name)
      : announce(std::move(announce)), announce_list(std::move(announce_list)) {
    info.name = std::move(name);
  }

  std::string announce;
  std::vector<std::string> announce_list;

  BittorrentInfo info;
};

BittorrentConfig bittorentConfigFactory(const BencodeValue& value);