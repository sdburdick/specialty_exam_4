#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class EmitterSettingsManager final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(EmitterSettingsManager, mixr::base::IComponent)

		public:
            EmitterSettingsManager();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}