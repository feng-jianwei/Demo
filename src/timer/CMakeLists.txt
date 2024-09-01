file(GLOB SOURCE_FILES "*.cpp")
add_library(game_timer_mgr ${SOURCE_FILES})

target_link_libraries(game_timer_mgr PRIVATE 
    tpool)

