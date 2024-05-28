/*
	
	Graphics Pipeline: sequence of operations that take the vertices and texture of meshes all the way
	to pixels and render the targets.

	(start)
	-Vertex/index buffer
		
	Input assembler* // collects raw vertex data and uses an index buffer to 
					// repeat elements w/o duplicating the vertex data itself
						
	Vertex shader // run for every vertex and applies transformations to turn 
				  // vertex positions from model space to screen space
				  // also passes per-vertex data down the pipeline
				  // eg. position.
	
	Tessellation // subdivides geometry into primitives to increase mesh quality
		
	Geometry shader // run on every primitive and can discard it or output more primitives
					// than came in. similar to tessellation but not widely used nowadays.
		
	Rasterization* // discretizes primitives into fragments to fill the framebuffer. 
				  // any fragments that fall outside the screen are discarded and 
				  // the attributes outputted by the vertex shader are interpolated across the fragments.
				  // fragments out of depth also discarded through depth test
		
	Fragment shader // invoked on every surviving fragment and determines which framebuffer the fragments
					// are written to and which color and depth values. it can do this using the 
					// interpolated data from the vertex shader, which can include things like texture 
					// coordinates and normals for lighting
		
	Color blending* // applies operations to mix different fragments that map to the same pixel in the
				   // framebuffer. fragments can simply overwrite each other, add up or be mixed based 
				   // upon transparency
		
	-FrameBuffer
	(end)

	- *FF --> Fixed Function Stages:
		- These allow us to tweak operations using params, but their behaviour is predefined.
	
	- The others are programmable:
		- Allow us to uploead our own code to the GPU and manage their behaviour.
		- Eg. using fragment shaders to implement texturing and lighting to ray tracing.
		- Parallel computing
	
	- Graphics pipeline in Vulkan is almost completely immutable
		- Must recreate pipeline from scratch if you want to change shaders, bind different framebuffers
		  or change the blend function
		- Disadvantage:
			- Must have a number of diff pipelines that represent all of the diff combinations of states
			  used in your rendering operation
		- Advantages:
			- Operations are known in advance so the driver can optimize for it better
	
	- Shader stages (vertex shaders, geometry shaders, fragment shaders)
		- stage
		- module
		- entry point

	- Vertex input
		- binding description
			- binding no.
			- stride
			- input rate
		- attribute descriptions
			- location number
			- binding
			- format
			- offset

	- Input assembler
		- topoogy
		- type

	- Viewport & Scissor come before Rasterization stage

*/

#pragma once

#include "tve_device.h"

#include <string>
#include <vector>

namespace tve {

	struct PipelineConfigInfo {
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator = (const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class TvePipeline {
	public:
		TvePipeline(
			TveDevice& device, 
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);
		~TvePipeline();

		TvePipeline(const TvePipeline&) = delete;
		TvePipeline& operator = (const TvePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	private:
		static std::vector<char> readFile(const std::string& filepath);
		
		void createGraphicsPipeline(
			const std::string& vertFilepath, 
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		TveDevice& tveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
} // namespace tve
