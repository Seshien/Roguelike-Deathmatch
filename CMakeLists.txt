cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(RogueLikeDeathmatch)

################################################################################
# Sub-projects
################################################################################
add_subdirectory("Client RLD")
add_subdirectory("Server RLD")


