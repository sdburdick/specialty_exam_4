#pragma once

#include <mixr/base/IComponent.hpp>
#include "CRFS_Packet_for_Receivers.h"

#include "shared/asio-1.36.0/asio.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include <deque>

using mixr::base::String;
using mixr::base::Integer;
using mixr::base::PairStream;

using asio::ip::udp;

namespace mixr {
    namespace crfs {
        struct ProactorClient {

            asio::ip::udp::endpoint endpoint;
            std::deque<std::shared_ptr<CPR_Packet>>packets{};
            bool packetSent{ false };
            std::mutex packet_mutex_;

            long messageCount{ 0 };

            ProactorClient() = default;
            ProactorClient(const ProactorClient&) = delete;
            ProactorClient& operator=(const ProactorClient&) = delete;
        };

        class CPR_Generator_Proactor final : public mixr::base::IComponent {
            DECLARE_SUBCLASS(CPR_Generator_Proactor, mixr::base::IComponent)

        public:
            CPR_Generator_Proactor();
            void updateTC(const double dt) override;
            void updateData(const double dt)override;
            void reset() override;
            void add_client(const udp::endpoint& ep);
            void transmit_CPR_for_client(ProactorClient* c);

        protected:
            bool setSlotInterfaceIpString(const mixr::base::String* const name);
            bool setSlotInterfaceHostOutgoingPort(const mixr::base::Integer* const port);
            bool setClients(const mixr::base::PairStream* const inputfile_clients);

        private:
            void runNetworkThread();

            std::unique_ptr <std::thread> udpThread;

            asio::io_context io_context;
            std::shared_ptr<asio::ip::udp::endpoint> udp_endpoint;
            std::string interface_ip = "127.0.0.1";
            unsigned short udp_port{ 0 };
            std::unique_ptr <udp::socket> socket_ptr;
            std::vector<std::unique_ptr<ProactorClient>> myClients;
        };
    }
}