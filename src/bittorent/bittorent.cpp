#include "bittorent.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "../bencoding/bencoding.hpp"

BittorrentConfig bittorentConfigFactory(const BencodeValue& value) {
  BittorrentInfo info{};
  std::string announce{};
  std::vector<std::string> announce_list{};
  std::size_t piece_length{};
  std::vector<std::array<std::uint8_t, 20>> pieces;
  {
  }
  std::string{"nice_name"};
  TorrentFiles files{};

  return BittorrentConfig{.announce = std::move(announce),
                          .info{.name = std::move("nice name"),
                                .piece_length = piece_length,
                                .pieces = std::move(pieces),
                                .files = std::move(files)}};
}
