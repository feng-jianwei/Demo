file(GLOB SOURCE_FILES "*.cpp")
add_library(game_event SHARED ${SOURCE_FILES})
target_link_libraries(game_event PRIVATE tpool)