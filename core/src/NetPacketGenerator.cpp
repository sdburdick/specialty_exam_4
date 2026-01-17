#include "core/NetPacketGenerator.h"

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(NetPacketGenerator, "NetPacketGenerator")
		EMPTY_SLOTTABLE(NetPacketGenerator)
		//EMPTY_SERIALIZER(NetPacketGenerator)

        NetPacketGenerator::NetPacketGenerator() {
			STANDARD_CONSTRUCTOR()
		}
        void NetPacketGenerator::copyData(const NetPacketGenerator& org, const bool cc)
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
        void NetPacketGenerator::deleteData() {
            BaseClass::deleteData();
        }
		void NetPacketGenerator::reset() {
			BaseClass::reset();
		}

		void NetPacketGenerator::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void NetPacketGenerator::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}