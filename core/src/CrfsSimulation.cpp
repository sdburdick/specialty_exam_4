#include "core/CrfsSimulation.h"

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(CrfsSimulation, "CrfsSimulation")
		EMPTY_SLOTTABLE(CrfsSimulation)
		//EMPTY_SERIALIZER(CrfsSimulation)

        CrfsSimulation::CrfsSimulation() {
			STANDARD_CONSTRUCTOR()
		}
        void CrfsSimulation::copyData(const CrfsSimulation& org, const bool cc)
        {
            if (cc) {
                //Resource Allocation : The object knows it must allocate fresh memory for any internal components rather than just pointing to org memory.
            }
            else {
                //need to delete existing pointers to prevent memory leaks
            }
            // 1. Call the base class copyData first!
            BaseClass::copyData(org);

            // 2. Copy local
            // this->myVariable = org.myVariable;   
        }
        void CrfsSimulation::deleteData() {
            BaseClass::deleteData();
        }

		void CrfsSimulation::reset() {
			BaseClass::reset();
		}


		void CrfsSimulation::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void CrfsSimulation::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}