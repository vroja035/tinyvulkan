
# Add source to this project's executable.
add_executable (tinyvulkanengine 
  "src/main.cpp"
  "include/tv_types.h"
  "src/tv_initializers.cpp"
  "include/tv_initializers.h"
  "include/tv_images.h"
  "src/tv_images.cpp" 
  "include/tv_descriptors.h"
  "src/tv_descriptors.cpp"
  "include/tv_pipelines.h"
  "src/tv_pipelines.cpp"
  "include/tv_engine.h"
  "src/tv_engine.cpp"
  "include/tv_loader.h"
  "src/tv_loader.cpp"
  "src/tv_camera.cpp"
  "include/tv_camera.h"
)

set_property(TARGET tinyvulkanengine PROPERTY CXX_STANDARD 20)
target_compile_definitions(tinyvulkanengine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(tinyvulkanengine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

#target_link_libraries(engine PUBLIC vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 vkbootstrap imgui fastgltf::fastgltf)
target_link_libraries(tinyvulkanengine PUBLIC vma glm Vulkan::Vulkan stb_image SDL2::SDL2 vkbootstrap imgui fastgltf::fastgltf)

target_precompile_headers(tinyvulkanengine PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp>  <glm/vec4.hpp> <vulkan/vulkan.h>)

add_custom_command(TARGET tinyvulkanengine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tinyvulkanengine> $<TARGET_FILE_DIR:tinyvulkanengine>
  COMMAND_EXPAND_LISTS
  )