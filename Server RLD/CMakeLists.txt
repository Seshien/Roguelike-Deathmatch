set(PROJECT_NAME ServerRLD)

################################################################################
# Source groups
################################################################################
set(Pliki_nagwkowe
    "../Utilities/Config.h"
    "../Utilities/Contact.h"
    "../Utilities/Event.h"
    "../Utilities/Logger.h"
    "../Utilities/Network.h"
    "../Utilities/Parser.h"
    "src/Game.h"
    "src/Item.h"
    "src/ItemObject.h"
    "src/Map.h"
    "src/Player.h"
    "src/PlayerObject.h"
    "src/Server.h"
    "src/SpawnableObject.h"
    "src/Tile.h"
)
source_group("Pliki nagłówkowe" FILES ${Pliki_nagwkowe})

set(Pliki_rdowe
    "../Utilities/Config.cpp"
    "../Utilities/Contact.cpp"
    "../Utilities/Event.cpp"
    "../Utilities/Logger.cpp"
    "../Utilities/Network.cpp"
    "../Utilities/Parser.cpp"
    "src/Game.cpp"
    "src/ItemObject.cpp"
    "src/Map.cpp"
    "src/Player.cpp"
    "src/PlayerObject.cpp"
    "src/Server RLD.cpp"
    "src/Server.cpp"
    "src/SpawnableObject.cpp"
    "src/Tile.cpp"
)
source_group("Pliki źródłowe" FILES ${Pliki_rdowe})

set(ALL_FILES
    ${Pliki_nagwkowe}
    ${Pliki_rdowe}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "WIN32;"
    "_CONSOLE;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "Ws2_32"
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")


