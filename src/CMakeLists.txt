project (JSONParserLib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE JSONParser_SOURCES "*.c")

add_library( JSONParserLib ${JSONParser_SOURCES} )

target_include_directories( JSONParserLib PUBLIC ${PROJECT_SOURCE_DIR} )
set_property(TARGET JSONParserLib PROPERTY C_STANDARD 99)

