# cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="DiscPath.../vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake_minimum_required(VERSION 3.20)

project(FrameworkPracDyp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC) # For MSVC
    add_compile_options(/W4 /MP /wd4458)
else() #For GCC or Clang
    add_compile_options(-Wall)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    "Source/*.cpp" 
    "Source/*.c"
    "Source/*.h" 
    "Source/*.hpp"
)

# Scanning source folder for inludes
set(SOURCE_SUBDIRS "")
foreach(FILE_PATH ${SOURCES})
    get_filename_component(DIR_PATH ${FILE_PATH} DIRECTORY)
    list(APPEND SOURCE_SUBDIRS ${DIR_PATH})
endforeach()
list(REMOVE_DUPLICATES SOURCE_SUBDIRS)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" FILES ${SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCES})

# For better includes not /src/game.h but /game.h
target_include_directories(${PROJECT_NAME} PRIVATE Source)

set(LIBS_DIR "${CMAKE_SOURCE_DIR}/Libs")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${LIBS_DIR}/glad/include"  
    "${LIBS_DIR}/glm"
	${SOURCE_SUBDIRS}
)


find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
    OpenGL::GL
)

#For knowing when Textures Fonts on so on are
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")