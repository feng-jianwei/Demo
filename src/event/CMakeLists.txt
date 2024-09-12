file(GLOB SOURCE_FILES "*.cpp")
add_library(demo_event SHARED ${SOURCE_FILES})
target_link_libraries(demo_event PRIVATE tpool)