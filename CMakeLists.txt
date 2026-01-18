cmake_minimum_required(VERSION 3.20)

project(OpenGlFrameWork)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC) # For MSVC
    add_compile_options(/W2 /MP)
else() #For GCC or Clang
    add_compile_options(-Wall)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    "Source/*.cpp" 
    "Source/*.c"
    "Source/*.h" 
    "Source/*.hpp"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" FILES ${SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCES})

# For better includes not /src/game.h but /game.h
target_include_directories(${PROJECT_NAME} PRIVATE Source)

set(LIBS_DIR "${CMAKE_SOURCE_DIR}/Libs")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${LIBS_DIR}/SDL2/include"
    "${LIBS_DIR}/SDL2Image/include"
    "${LIBS_DIR}/SDL2Mixer/include"
    "${LIBS_DIR}/SDL2_ttf/include"
    "${LIBS_DIR}/glad/include"  
    "${LIBS_DIR}/glm"
	"Source/Core"
    "Source/Rendering"
    "Source/GameLoop"
    "Source/Programs"
    "Source/Externals"
)

target_link_directories(${PROJECT_NAME} PRIVATE
    "${LIBS_DIR}/SDL2/lib/"
    "${LIBS_DIR}/SDL2Image/lib/"
    "${LIBS_DIR}/SDL2Mixer/lib/"
    "${LIBS_DIR}/SDL2_ttf/lib/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
    SDL2_ttf
    opengl32  
)

file(GLOB ALL_DLLS "${CMAKE_SOURCE_DIR}/DLLS/*.dll")
foreach(DLL ${ALL_DLLS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endforeach()

#For knowing when Textures Fonts on so on are
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")