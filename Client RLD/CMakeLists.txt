set(PROJECT_NAME ClientRLD)

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
    "src/Client.h"
    "src/Damage.h"
    "src/Item.h"
    "src/Map.h"
    "src/OurPlayerInfo.h"
    "src/PlayerInfo.h"
    "src/Tile.h"
    "src/UIBar.h"
    "src/UIButton.h"
)
source_group("Pliki nagłówkowe" FILES ${Pliki_nagwkowe})

set(Pliki_rdowe
    "../Utilities/Config.cpp"
    "../Utilities/Contact.cpp"
    "../Utilities/Event.cpp"
    "../Utilities/Logger.cpp"
    "../Utilities/Network.cpp"
    "../Utilities/Parser.cpp"
    "src/Client RLD.cpp"
    "src/Client.cpp"
    "src/Damage.cpp"
    "src/Item.cpp"
    "src/Map.cpp"
    "src/OurPlayerInfo.cpp"
    "src/PlayerInfo.cpp"
    "src/Tile.cpp"
    "src/UIBar.cpp"
    "src/UIButton.cpp"
)
source_group("Pliki źródłowe" FILES ${Pliki_rdowe})

set(ALL_FILES
    ${Pliki_nagwkowe}
    ${Pliki_rdowe}
)

set(SFML_STATIC_LIBRARIES TRUE)

add_subdirectory("SFML")
################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

################################################################################
# Include directories
################################################################################
#target_include_directories(${PROJECT_NAME} PRIVATE
#   "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include"
#)

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
    "$<$<CONFIG:Debug>:"
        "sfml-window-d;"
        "sfml-system-d;"
        "sfml-audio-d;"
        "sfml-graphics-d"
    ">"
    "$<$<CONFIG:Release>:"
        "sfml-window;"
        "sfml-system;"
        "sfml-audio;"
        "sfml-graphics"
    ">"
    "Ws2_32;"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

#target_link_directories(${PROJECT_NAME} PRIVATE
#    "${CMAKE_CURRENT_SOURCE_DIR}/SFML/lib"
#)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/data/
                ${CMAKE_CURRENT_BINARY_DIR}/data/)
#add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#                ${CMAKE_CURRENT_SOURCE_DIR}/bin/
#                ${CMAKE_CURRENT_BINARY_DIR}/)
