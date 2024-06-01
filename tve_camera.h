/*
	More random notes

	eyes see in 2d; brain translates into 3d
	create objects by constructing them out of triangles
	project triangles into a flat plane from viewer prespective
	each pixel is like a little window with colors
	ray tracing -> rays projected into world and collide with objects
	
	image order rendering, each pixel is considered and on a per pixel basis we find any objects that could influence it
	object order renderering, faster, draw each object one after another, maps better to gpu, same instructions on different vertices
	
	vulkan's canonical viewing volumeL only what is inside the x(-1,1) y(-1,1) z(0,1) will be displayed
	
	orthographic projection: generalization of the view volume that allows us to specify whatever dimension and location but maintains overall shape
	and keeps direction and orientation fixed
	6 bounding blanes (left, bottom, near) (right, top, far)
	
	need to transfowm orthographic view volume into vulkan's canonical view volume
	combine two basic transformations
	1. translate center C of near the plane to origin c = ((r+l)/2 , (b+t)/2 , n)
	2. scale matrix to size of canonical
	this multiplication gives us the orthographic projection matrix (this differs between graphics API)
	objects that are within the orthographic view volume will be scaled and moved into canonical to be displayed
	dos not apply pespective need a furstrum
	captures viewiers line of sight as if they were looking through a rectangular window
	
	perspective matrix that transforms the frustrum and any object it contains into the orthographic view volume
	project each point into the viewing plane
	
	4x4 matrix that when applied to a position vector, the resulting x and y values equal the projected Xs and Ys coordinates on the viewing plane.
	
	Perspective transform -> orthographic transform -> Perspective Projection Matrix

	Camera transformation allows us to view the world from any view point and orientation.
	translate "camera" to rigin
	rotate "camera" to point in +z directionn
	mcam = rotate x translate
*/


#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //for vulkan
#include <glm/glm.hpp>

namespace tve {
	class TveCamera {
	public:
		void setOrthographicProjection(
			float left, 
			float right, 
			float top, 
			float bottomn,
			float near, 
			float far
		);

		void setPerspectiveProjection(
			float fovy,
			float aspect,
			float near,
			float far
		);

		void setViewDirection(
			glm::vec3 position,
			glm::vec3 direction,
			glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }
		);

		void setViewTarget(
			glm::vec3 position,
			glm::vec3 target,
			glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }
		);

		void setViewYXZ(
			glm::vec3 position,
			glm::vec3 rotation
		);

		const glm::mat4& getProjection() const { return projectionMatrix; }
		const glm::mat4& getView() const { return viewMatrix; }
	private:
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };
	};
}