# ====== Define Sources ======
file(GLOB_RECURSE SHADER_SRC "*.glsl" "*.txt" "*.vert" "*.frag" "*.geom" "*.comp")

# ====== Add Library ======
add_library(Shaders STATIC ${SHADER_SRC})

# ====== Include Directories ======
target_include_directories(Shaders
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ====== Link Interface Libraries ======
target_link_libraries(Shaders PRIVATE GLAD)

# ====== Set Folder Properties for Visual Studio ======
# This nests 'render' under 'Graphics' in the solution
set_target_properties(Shaders PROPERTIES FOLDER "Graphics")
