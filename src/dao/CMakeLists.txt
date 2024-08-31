set(SOURCE_FILES
    postgres_conn.cpp
)

add_library(game_db SHARED ${SOURCE_FILES})

target_include_directories(game_db PRIVATE /usr/include/postgresql)
target_link_directories(game_db PUBLIC /usr/lib/x86_64-linux-gnu)
target_link_libraries(game_db PUBLIC pq)