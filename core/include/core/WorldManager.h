#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class WorldManager final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(WorldManager, mixr::base::IComponent)

		public:
            WorldManager();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}