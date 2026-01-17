#pragma once

#include <mixr/base/IComponent.hpp>
#include "CRFS_Packet_for_Receivers.h"

#include "shared/asio-1.36.0/asio.hpp"

using asio::ip::udp;
using mixr::base::String;
using mixr::base::Integer;

namespace mixr {
	namespace crfs {
		class CPR_Receiver final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(CPR_Receiver, mixr::base::IComponent)

		public:
            CPR_Receiver();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;

            auto getMessageCount() {
                return message_count.load();
            }

        protected:
            bool setSlotInterfaceIpString(const mixr::base::String* const name);
            bool setSlotInterfaceListenPort(const mixr::base::Integer* const port);

		private:
			bool initialized{};

            
            asio::io_context io_context;
            //must use unique pointer because Mixr factory instantiates,
            //leaving udp_socket uninstantiated
            std::shared_ptr<asio::ip::udp::endpoint> udp_endpoint;
            std::unique_ptr<asio::ip::udp::socket> udp_socket;


            std::string interface_ip = "127.0.0.1";
            //std::string interface_ip = "192.168.4.50"; 
            unsigned short udp_port = 5001; 

            std::unique_ptr<std::thread> workerThread;
            std::unique_ptr<std::thread> monitor_thread;
            
            bool keep_running{ true };
            std::atomic<uint32_t> message_count{ 0 };
		};
	}
}