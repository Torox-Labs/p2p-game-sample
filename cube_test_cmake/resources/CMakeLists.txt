# ====== Define Sources ======
file(GLOB_RECURSE RESOURCE_SRC "*.nms" "*.tga" "*.png" "*.jpg" "*.glsl" "*.vert" "*.frag" "*.geom" "*.comp")

# ====== Add Library ======
add_library(Resources STATIC ${RESOURCE_SRC})

# ====== Include Directories ======
target_include_directories(Resources
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ====== Link Interface Libraries ======
target_link_libraries(Resources PRIVATE GLAD)

# ====== Set Folder Properties for Visual Studio ======
# This nests 'render' under 'Graphics' in the solution
set_target_properties(Resources PROPERTIES FOLDER "Graphics")
