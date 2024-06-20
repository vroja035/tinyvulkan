
# Add source to this project's executable.
add_executable (tinyvulkanengine 
  src/main.cpp
  include/vk_types.h
  src/vk_initializers.cpp
  include/vk_initializers.h
  include/vk_images.h
  src/vk_images.cpp 
  include/vk_descriptors.h
  src/vk_descriptors.cpp
  include/vk_pipelines.h
  src/vk_pipelines.cpp
  include/vk_engine.h
  src/vk_engine.cpp
  include/vk_loader.h
  src/vk_loader.cpp
  src/camera.cpp
  include/camera.h
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