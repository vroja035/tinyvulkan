/*
	GLTF loading logic.
*/
#pragma once
#include <tv_types.h>
#include <unordered_map>
#include <filesystem>
#include "tv_descriptors.h"

#include <fastgltf/tools.hpp>

struct GLTFMaterial {
    MaterialInstance data;
};

struct Bounds {
    glm::vec3 origin;
    float sphereRadius;
    glm::vec3 extents;
};

struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
    Bounds bounds;
    std::shared_ptr<GLTFMaterial> material;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
};
//forward declaration
class TinyVulkan;

// Contains all resources needed forhandling a GLTF file
struct LoadedGLTF : public IRenderable {

    // storage for all the data on a given glTF file
    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
    std::unordered_map<std::string, AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

    // nodes that dont have a parent, for iterating through the file in tree order
    std::vector<std::shared_ptr<Node>> topNodes;

    std::vector<VkSampler> samplers;

    DescriptorAllocator descriptorPool;

    AllocatedBuffer materialDataBuffer;

    TinyVulkan* creator;

    ~LoadedGLTF() { clearAll(); };

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);

private:

    void clearAll();
};

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(TinyVulkan* engine, std::string_view filePath);
std::optional<AllocatedImage> load_image(TinyVulkan* engine, fastgltf::Asset& asset, fastgltf::Image& image);