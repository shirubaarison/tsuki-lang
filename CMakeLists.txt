cmake_minimum_required(VERSION 4.1)
project(TsukiLang LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXECUTABLE_ENABLE_EXPORTS TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

file(GLOB_RECURSE SRCFILES "src/*.cpp")
file(GLOB_RECURSE INCLUDEFILES "include/")

add_executable(TsukiLang ${SRCFILES})

target_include_directories(TsukiLang PUBLIC include)

find_package(PkgConfig REQUIRED)

pkg_check_modules(READLINE REQUIRED IMPORTED_TARGET readline)

target_link_libraries(TsukiLang PRIVATE PkgConfig::READLINE)

target_compile_options(TsukiLang PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wno-unused-parameter
  -Wno-unused-value
  -Wno-missing-field-initializers
  -Wno-narrowing
  -Wno-pointer-arith
  -Wno-clobbered
)
