FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests bencoding_test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME}_lib)

include(Catch)

catch_discover_tests(tests)