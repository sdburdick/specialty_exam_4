#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class Propegator final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(Propegator, mixr::base::IComponent)

		public:
            Propegator();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}