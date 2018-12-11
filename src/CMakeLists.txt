project (JSONParserLib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE JSONParser_SOURCES "*.c")

add_library( JSONParserLib ${DnDMonsterViewer_SOURCES} )

target_include_directories( JSONParserLib PUBLIC "." )
set_property(TARGET JSONParserLib PROPERTY C_STANDARD 99)

