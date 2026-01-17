#include "core/ReflectionManager.h"

//reads off the input vector files to build out a reflection scheme for emissions when they contact a surface

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(ReflectionManager, "ReflectionManager")
		EMPTY_SLOTTABLE(ReflectionManager)
		//EMPTY_SERIALIZER(ReflectionManager)

        ReflectionManager::ReflectionManager() {
			STANDARD_CONSTRUCTOR()
		}
        void ReflectionManager::copyData(const ReflectionManager& org, const bool cc)
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
        void ReflectionManager::deleteData() {
            BaseClass::deleteData();
        }
		void ReflectionManager::reset() {
			BaseClass::reset();
		}

		void ReflectionManager::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void ReflectionManager::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}