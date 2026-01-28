include(io_uring)
file(GLOB SOURCE_FILES "*.cpp")
add_library(demo_aio SHARED ${SOURCE_FILES})
target_link_libraries(demo_aio PRIVATE tpool)