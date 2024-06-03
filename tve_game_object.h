/*
	
	Euler Angles used to represent rotations in 3d

	Proper Euler Angles, same axes for first and 3rd elemental rotations
	Tait-Bryan Angles, 3 distinct axes (more common for game engine)

	Intrinsic vs Extrinsic rotations
	Instrinsic -> coordinate system moves with the moving body changing axes orientation
	Read right to left for intrinsic
	Read left to right for extrinsic

*/

#pragma once

#include "tve_model.h"

#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>


namespace tve {

	struct TransformComponent {
		glm::vec3 translation{}; //(position offset)
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class TveGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, TveGameObject>;

		static TveGameObject createGameObject() {
			static id_t currentId = 0;
			return TveGameObject{ currentId++ };
		}

		TveGameObject(const TveGameObject&) = delete;
		TveGameObject& operator = (const TveGameObject&) = delete;
		TveGameObject(TveGameObject&&) = default;
		TveGameObject& operator = (TveGameObject&&) = default;

		const id_t getId() { return id; }

		std::shared_ptr<TveModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		TveGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};

} // namespace tve