#pragma once

#include <mixr/base/IComponent.hpp>
#include <mixr/simulation/Simulation.hpp>

namespace mixr {
	namespace crfs {
		class CrfsSimulation final : public mixr::simulation::ISimulation {
		    DECLARE_SUBCLASS(CrfsSimulation, mixr::simulation::ISimulation)

		public:
            CrfsSimulation();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

		private:
			bool initialized{};
		};
	}
}