file(GLOB SOURCE_FILES "*.cpp")
add_executable(game_server ${SOURCE_FILES})

target_link_libraries(game_server PRIVATE 
    game_db
    game_utils
    civetweb-cpp
    game_event)
