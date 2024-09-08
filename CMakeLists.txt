cmake_minimum_required(VERSION 3.20)
project(GameServer)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_compile_options(-Wall -Wextra)
add_link_options(-Wl,--allow-shlib-undefined)

include(FetchContent)
# 外部项目申明
FetchContent_Declare(
  civetweb
  GIT_REPOSITORY https://github.com/civetweb/civetweb.git
  GIT_TAG v1.16  # 可以指定分支、标签或提交哈希
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/civetweb
)

FetchContent_Declare(
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG curl-8_9_1 # 可以指定分支、标签或提交哈希
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/curl
)

# 手动下载项目而不添加到构建系统中
FetchContent_Populate(civetweb)
FetchContent_Populate(curl)
include_directories(${PROJECT_SOURCE_DIR}/third_party/civetweb/include)

include_directories(src)
include_directories(include)
add_subdirectory(src)
add_subdirectory(third_party/civetweb)
add_subdirectory(third_party/curl)