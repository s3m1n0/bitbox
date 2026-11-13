#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "../bencoding/bencoding.hpp"

struct BittorrentFileInfo {
  std::size_t length{};
  std::vector<std::string> path;
};

using BittorentSingleFile = std::size_t;
using TorrentFiles =
    std::variant<BittorentSingleFile, std::vector<BittorrentFileInfo>>;

struct BittorrentInfo {
  std::string name;
  std::size_t piece_length{};
  std::vector<std::array<std::uint8_t, 20>> pieces;

  TorrentFiles files;
};

using Announce = std::variant<std::string, std::vector<std::string>>;
struct BittorrentConfig {
  //  public:
  //   BittorrentConfig(std::string announce, std::string name,
  //                    std::size_t piece_length,
  //                    std::vector<std::array<std::uint8_t, 20>> pieces,
  //                    TorrentFiles files)
  //       : announce(std::move(announce)),
  //         info{std::move(name), piece_length, std::move(pieces),
  //              std::move(files)} {};

  Announce announce;

  BittorrentInfo info;
};

BittorrentConfig bittorentConfigFactory(const BencodeValue& value);