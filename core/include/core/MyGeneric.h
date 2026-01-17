#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class MyGeneric final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(MyGeneric, mixr::base::IComponent)

		public:
			MyGeneric();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}