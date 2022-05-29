CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
project(wxjson)

set(wxjson_SOURCES
src/jsonreader.cpp
src/jsonval.cpp
src/jsonwriter.cpp
)

add_library(wxjson STATIC ${wxjson_SOURCES})
target_include_directories(wxjson SYSTEM PUBLIC include)
target_link_libraries(wxjson PUBLIC ${wxWidgets_LIBRARIES})

include_directories(
    ./include
)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(WXJSON_LIBRARIES wxjson CACHE INTERNAL "")
set(WXJSON_INCLUDE_DIRS ${dirs} CACHE INTERNAL "")


