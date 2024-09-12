file(GLOB SOURCE_FILES "*.cpp")
add_executable(demo_server ${SOURCE_FILES})

target_link_libraries(demo_server PRIVATE 
    demo_db
    demo_utils
    civetweb-cpp
    demo_event)
