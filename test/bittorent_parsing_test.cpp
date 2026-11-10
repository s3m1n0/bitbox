
#include <catch2/catch_test_macros.hpp>
#include <string>

#include "../src/bencoding/bencoding.hpp"
#include "../src/bittorent/bittorent.hpp"

TEST_CASE("BittorrentConfig factory", "[bittorrent][config]") {
  SECTION("Parses a single-file torrent") {
    const std::string data =
        "d"
        "8:announce14:http://tracker"
        "4:infod"
        "6:lengthi12345e"
        "4:name8:test.bin"
        "12:piece lengthi16384e"
        "6:pieces40:1234567890123456789012345678901234567890"
        "ee";

    const auto value = parse(data);
    const auto config = bittorentConfigFactory(value);

    REQUIRE(config.announce == "http://tracker");
    REQUIRE(config.info.name == "test.bin");
    REQUIRE(config.info.length == 12345);
    REQUIRE(config.info.piece_length == 16384);

    REQUIRE(config.info.pieces.size() == 2);
    REQUIRE(config.info.pieces[0] == "12345678901234567890");
    REQUIRE(config.info.pieces[1] == "12345678901234567890");

    REQUIRE(config.info.files.empty());
  }

  SECTION("Parses a multi-file torrent") {
    const std::string data =
        "d"
        "8:announce14:http://tracker"
        "4:infod"
        "5:filesl"
        "d"
        "6:lengthi100e"
        "4:pathl3:dir5:a.txtee"
        "d"
        "6:lengthi200e"
        "4:pathl3:dir5:b.txtee"
        "e"
        "4:name8:test-dir"
        "12:piece lengthi16384e"
        "6:pieces20:12345678901234567890"
        "ee";

    const auto value = parse(data);
    const auto config = bittorentConfigFactory(value);

    REQUIRE(config.info.name == "test-dir");
    REQUIRE(config.info.files.size() == 2);

    REQUIRE(config.info.files[0].length == 100);
    REQUIRE(config.info.files[0].path.size() == 2);
    REQUIRE(config.info.files[0].path[0] == "dir");
    REQUIRE(config.info.files[0].path[1] == "a.txt");

    REQUIRE(config.info.files[1].length == 200);
    REQUIRE(config.info.files[1].path.size() == 2);
    REQUIRE(config.info.files[1].path[0] == "dir");
    REQUIRE(config.info.files[1].path[1] == "b.txt");

    REQUIRE(config.info.pieces.size() == 1);
    REQUIRE(config.info.length == 0);
  }

  SECTION("Parses announce-list") {
    const std::string data =
        "d"
        "8:announce7:tracker"
        "13:announce-listll"
        "8:tracker1"
        "8:tracker2"
        "el"
        "8:tracker3"
        "ee"
        "4:infod"
        "6:lengthi1e"
        "4:name1:a"
        "12:piece lengthi16384e"
        "6:pieces20:12345678901234567890"
        "ee";

    const auto value = parse(data);
    const auto config = bittorentConfigFactory(value);

    REQUIRE(config.announce_list.size() == 3);
    REQUIRE(config.announce_list[0] == "tracker1");
    REQUIRE(config.announce_list[1] == "tracker2");
    REQUIRE(config.announce_list[2] == "tracker3");
  }
}

TEST_CASE("BittorrentConfig factory error handling",
          "[bittorrent][config][errors]") {
  SECTION("Rejects missing announce field") {
    const std::string data =
        "d"
        "4:infod"
        "6:lengthi1e"
        "4:name1:a"
        "12:piece lengthi16384e"
        "6:pieces20:12345678901234567890"
        "ee";

    const auto value = parse(data);

    REQUIRE_THROWS(bittorentConfigFactory(value));
  }

  SECTION("Rejects missing info field") {
    const std::string data =
        "d"
        "8:announce7:tracker"
        "e";

    const auto value = parse(data);

    REQUIRE_THROWS(bittorentConfigFactory(value));
  }

  SECTION("Rejects invalid pieces length") {
    const std::string data =
        "d"
        "8:announce7:tracker"
        "4:infod"
        "6:lengthi1e"
        "4:name1:a"
        "12:piece lengthi16384e"
        "6:pieces19:1234567890123456789"
        "ee";

    const auto value = parse(data);

    REQUIRE_THROWS(bittorentConfigFactory(value));
  }
}
