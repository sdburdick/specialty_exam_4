#include "remote/MyGeneric.h"

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(MyGeneric, "MyGeneric")
		EMPTY_SLOTTABLE(MyGeneric)
		//EMPTY_SERIALIZER(MyGeneric)

		MyGeneric::MyGeneric() {
			STANDARD_CONSTRUCTOR()
		}
        void MyGeneric::copyData(const MyGeneric& org, const bool cc)
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
        void MyGeneric::deleteData() {
            BaseClass::deleteData();
        }

		void MyGeneric::reset() {
			BaseClass::reset();
		}


		void MyGeneric::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void MyGeneric::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}