#include "core/WorldManager.h"

//moves and articulates players to establish the 'single truth' of the RF state

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(WorldManager, "WorldManager")
		EMPTY_SLOTTABLE(WorldManager)
		//EMPTY_SERIALIZER(WorldManager)

        WorldManager::WorldManager() {
			STANDARD_CONSTRUCTOR()
		}
        void WorldManager::copyData(const WorldManager& org, const bool cc)
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
        void WorldManager::deleteData() {
            BaseClass::deleteData();
        }
		void WorldManager::reset() {
			BaseClass::reset();
		}

		void WorldManager::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void WorldManager::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}