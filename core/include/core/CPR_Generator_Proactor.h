#pragma once

#include <mixr/base/IComponent.hpp>
#include "CRFS_Packet_for_Receivers.h"

#include "shared/asio-1.36.0/asio.hpp"
#include "shared/asio-1.36.0/asio/post.hpp"
#include "shared/asio-1.36.0/asio/bind_executor.hpp"
#include "shared/asio-1.36.0/asio/strand.hpp"

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

            long messageCount{ 0 };
            std::chrono::steady_clock::time_point lastQueueWarning{};
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
            bool setNumUdpThreads(const mixr::base::Integer* const num);
            bool shutdownNotification();

        private:

            std::vector<std::thread> udpThreads;
            unsigned int numThreads{ 1 };

            
            
            asio::io_context io_context;
            asio::executor_work_guard<asio::io_context::executor_type> asio_work_guard{ io_context.get_executor() };
            asio::strand<asio::io_context::executor_type> strand{ io_context.get_executor() };

            asio::ip::udp::endpoint udp_endpoint;
            std::string interface_ip = "127.0.0.1";
            unsigned short udp_port{ 0 };
            std::unique_ptr <udp::socket> socket_ptr;
            std::vector<std::unique_ptr<ProactorClient>> myClients;

        };
    }
}