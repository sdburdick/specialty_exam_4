#include "core/EmissionManager.h"

//generates emissions based on emitters that are declared in the system

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(EmissionManager, "EmissionManager")
		EMPTY_SLOTTABLE(EmissionManager)
		//EMPTY_SERIALIZER(EmissionManager)

        EmissionManager::EmissionManager() {
			STANDARD_CONSTRUCTOR()
		}

        void EmissionManager::copyData(const EmissionManager& org, const bool cc)
        {
            BaseClass::copyData(org, cc);
            if (cc) {
                //Resource Allocation : The object knows it must allocate fresh memory for any internal components rather than just pointing to org memory.
            }
            else {
                //need to delete existing pointers to prevent memory leaks
            }
            // this->myVariable = org.myVariable;
        }
        void EmissionManager::deleteData() {
            BaseClass::deleteData();
        }
		void EmissionManager::reset() {
			BaseClass::reset();
		}

		void EmissionManager::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}

		void EmissionManager::updateTC(const double dt) {
			BaseClass::updateTC(dt);
		}
	}
}