#pragma once

#include <mixr/base/IComponent.hpp>
#include "CRFS_Packet_for_Receivers.h"

#include "shared/asio-1.36.0/asio.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <string>

using mixr::base::String;
using mixr::base::Integer;
using mixr::base::PairStream;

using asio::ip::udp;

namespace mixr {
	namespace crfs {
        struct Client {
            asio::ip::udp::endpoint endpoint;
            CPR_Packet packet;
        };

        class CPR_Generator final : public mixr::base::IComponent {
            DECLARE_SUBCLASS(CPR_Generator, mixr::base::IComponent)

        public:
            CPR_Generator();

            void updateTC(const double dt) override;
            void updateData(const double dt)override;
            void reset() override;


            void add_client(const udp::endpoint& ep);
            void tick();



        protected:
            bool setSlotInterfaceIpString(const mixr::base::String* const name);
            bool setSlotInterfaceHostOutgoingPort(const mixr::base::Integer* const port);
            bool setClients(const mixr::base::PairStream* const inputfile_clients);
            bool setNanoSecondMsgInterval(const mixr::base::Integer* const sleeptime);

        private:

            asio::io_context io_context;
            float compute_value_for(const Client& c);

            void runNetworkThread();

            std::unique_ptr <std::thread> udpThread;

            std::shared_ptr<asio::ip::udp::endpoint> udp_endpoint;

            std::string interface_ip = "127.0.0.1";
            //std::string interface_ip = "192.168.4.47"; 
            unsigned short udp_port = 5100;

            std::chrono::nanoseconds nanosecInterval{ 1'000'000 }; //1,000,000 translates to 1,000 Hz.  10 mil would be 100 Hz
            std::unique_ptr <udp::socket> socket_ptr;
            std::vector<Client> clients_;
            uint32_t seq_;

            
		};
	}
}