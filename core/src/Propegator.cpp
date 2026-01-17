#include "core/Propegator.h"

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(Propegator, "MyGeneric")
		EMPTY_SLOTTABLE(Propegator)
		//EMPTY_SERIALIZER(Propegator)

		Propegator::Propegator() {
			STANDARD_CONSTRUCTOR()
		}
        void Propegator::copyData(const Propegator& org, const bool cc)
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
        void Propegator::deleteData() {
            BaseClass::deleteData();
        }
		void Propegator::reset() {
			BaseClass::reset();
		}

		void Propegator::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void Propegator::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}