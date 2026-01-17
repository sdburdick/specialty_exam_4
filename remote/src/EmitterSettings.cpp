#include "remote/EmitterSettings.h"

namespace mixr {
	namespace crfs {
		IMPLEMENT_SUBCLASS(EmitterSettings, "EmitterSettings")
		EMPTY_SLOTTABLE(EmitterSettings)
		//EMPTY_SERIALIZER(EmitterSettings)

        EmitterSettings::EmitterSettings() : io_context(){
			STANDARD_CONSTRUCTOR()
            tcp_socket = std::make_unique<asio::ip::tcp::socket>(io_context);
		}
        void EmitterSettings::copyData(const EmitterSettings& org, const bool cc)
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
        void EmitterSettings::deleteData() {
            BaseClass::deleteData();
        }

		void EmitterSettings::reset() {
			BaseClass::reset();
		}


		void EmitterSettings::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
            
            asio::ip::tcp::endpoint endpoint(
                asio::ip::make_address_v4("127.0.0.1"),
                3000
            );
            tcp_socket->connect(endpoint);
            
		}


		void EmitterSettings::updateTC(const double dt) {
			
			BaseClass::updateTC(dt);
		}
	}
}