#pragma once

#include <mixr/base/IComponent.hpp>

namespace mixr {
	namespace crfs {
		class NetPacketGenerator final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(NetPacketGenerator, mixr::base::IComponent)

		public:
            NetPacketGenerator();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}