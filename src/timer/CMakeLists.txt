file(GLOB SOURCE_FILES "*.cpp")
add_library(demo_timer_mgr ${SOURCE_FILES})

target_link_libraries(demo_timer_mgr PRIVATE 
    tpool)

