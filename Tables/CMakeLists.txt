file(GLOB_RECURSE TABLES_SOURCES "*.cpp")
add_library(tables STATIC ${TABLES_SOURCES})
