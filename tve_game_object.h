#pragma once

#include "tve_model.h"

//std
#include <memory>


namespace tve {

	struct Transform2dComponent {
		glm::vec2 translation{}; //(position offset)
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c,s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
			return rotMatrix * scaleMat; 
		}
	};

	class TveGameObject {
	public:
		using id_t = unsigned int;

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
		Transform2dComponent transform2d{};

	private:
		TveGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};

} // namespace tve