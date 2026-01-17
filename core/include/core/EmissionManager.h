#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class EmissionManager final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(EmissionManager, mixr::base::IComponent)

		public:
            EmissionManager();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}