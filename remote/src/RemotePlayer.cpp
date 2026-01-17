#include "remote/RemotePlayer.h"

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(RemotePlayer, "RemotePlayer")
		EMPTY_SLOTTABLE(RemotePlayer)
		//EMPTY_SERIALIZER(RemotePlayer)

        RemotePlayer::RemotePlayer() {
			STANDARD_CONSTRUCTOR()
		}
        void RemotePlayer::copyData(const RemotePlayer& org, const bool cc)
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
        void RemotePlayer::deleteData() {
            BaseClass::deleteData();
        }

		void RemotePlayer::reset() {
			BaseClass::reset();
		}


		void RemotePlayer::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}


		void RemotePlayer::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}