project (DnDMonsterViewerExe)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE DnDMonsterViewer_SOURCES "*.c")

list( FILTER DnDMonsterViewer_SOURCES EXCLUDE REGEX ".*main.c$" )

add_library( DnDMonsterViewerLib ${DnDMonsterViewer_SOURCES} )

target_link_libraries( DnDMonsterViewerLib PUBLIC "ncurses" )
target_include_directories( DnDMonsterViewerLib PUBLIC "." )
set_property(TARGET DnDMonsterViewerLib PROPERTY C_STANDARD 99)

add_executable( DnDMonsterViewerExe main.c )

target_link_libraries( DnDMonsterViewerExe PUBLIC DnDMonsterViewerLib )
set_property(TARGET DnDMonsterViewerExe PROPERTY C_STANDARD 99)
