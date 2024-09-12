file(GLOB SOURCE_FILES "*.cpp")
add_library(http_client ${SOURCE_FILES})

add_dependencies(http_client libcurl_shared)
target_link_directories(http_client PRIVATE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_include_directories(http_client PRIVATE ${PROJECT_SOURCE_DIR}/third_party/curl/include)
target_link_libraries(http_client PRIVATE curl demo_timer_mgr)
