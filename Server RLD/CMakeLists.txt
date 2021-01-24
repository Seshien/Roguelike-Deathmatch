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

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ServerRLD)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
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
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Gy
        >
        /permissive-;
        /std:c++17;
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "Ws2_32"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")



























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

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ServerRLD)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
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
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Gy
        >
        /permissive-;
        /std:c++17;
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "Ws2_32"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

