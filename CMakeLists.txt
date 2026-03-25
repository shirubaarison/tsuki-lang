cmake_minimum_required(VERSION 3.20)
project(TsukiLang LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRCFILES CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER SRCFILES EXCLUDE REGEX "src/cli/main.cpp$")
add_library(tsuki_core ${SRCFILES})
target_include_directories(tsuki_core PUBLIC include)
target_include_directories(tsuki_core
    PUBLIC ${PROJECT_SOURCE_DIR}/src
)

add_executable(TsukiLang src/cli/main.cpp)
target_link_libraries(TsukiLang PRIVATE tsuki_core PkgConfig::READLINE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(READLINE REQUIRED IMPORTED_TARGET readline)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE TESTFILES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(tests ${TESTFILES})
target_include_directories(tests PRIVATE include)
target_link_libraries(tests PRIVATE tsuki_core Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)

target_compile_options(tsuki_core PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(TsukiLang PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
