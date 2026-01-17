#pragma once

#include <mixr/base/IComponent.hpp>
#include "shared/asio-1.36.0/asio.hpp"
#include <thread>
#include <deque>
#include <mutex>
#include <string>

using asio::ip::tcp;

namespace mixr {
	namespace crfs {
		class RemoteHandler final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(RemoteHandler, mixr::base::IComponent)

		public:
            RemoteHandler();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;
            void publish(const std::string& message);

		private:
            void runNetworkThread();
            void startAccept();

            asio::io_context io_context;
            std::unique_ptr<asio::ip::tcp::acceptor> tcp_acceptor;
            
            std::shared_ptr<tcp::socket> active_socket;

            std::thread background_thread;
            bool keep_running{ true };

            std::deque<std::string> message_queue;
            std::mutex queue_mutex;
		};
	}
}