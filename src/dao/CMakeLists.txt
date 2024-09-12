set(SOURCE_FILES
    postgres_conn.cpp
)

add_library(demo_db SHARED ${SOURCE_FILES})

target_include_directories(demo_db PRIVATE /usr/include/postgresql)
target_link_directories(demo_db PUBLIC /usr/lib/x86_64-linux-gnu)
target_link_libraries(demo_db PUBLIC pq)