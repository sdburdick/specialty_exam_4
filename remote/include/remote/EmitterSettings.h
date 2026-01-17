#pragma once

#include <mixr/base/IComponent.hpp>
#include "shared/asio-1.36.0/asio.hpp"

using asio::ip::tcp;

namespace mixr {
	namespace crfs {
		class EmitterSettings final : public mixr::base::IComponent {
			DECLARE_SUBCLASS(EmitterSettings, mixr::base::IComponent)

		public:
            EmitterSettings();

			void updateTC(const double dt) override;
			void updateData(const double dt)override;
			void reset() override;



		private:
			bool initialized{};

            
            asio::io_context io_context;
            //must use unique pointer because Mixr factory instantiates,
            //leaving tcp_socket uninstantiated
            std::unique_ptr<asio::ip::tcp::socket> tcp_socket;

            const std::string tcp_host = "127.0.0.1";
            const std::string tcp_port = "3000";
		};
	}
}