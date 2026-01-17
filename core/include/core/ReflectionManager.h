#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class ReflectionManager final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(ReflectionManager, mixr::base::IComponent)

		public:
            ReflectionManager();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}