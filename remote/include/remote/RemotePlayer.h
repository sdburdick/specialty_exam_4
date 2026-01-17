#pragma once

#include <mixr/simulation/IPlayer.hpp>

namespace mixr {
	namespace crfs {
		class RemotePlayer final : public mixr::simulation::IPlayer {
			DECLARE_SUBCLASS(RemotePlayer, mixr::simulation::IPlayer)

		public:
            RemotePlayer();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};

		};
	}
}