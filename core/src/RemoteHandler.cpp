#include "core/RemoteHandler.h"

//not there yet - pivoting.  baseline start for a tcp connection

namespace mixr {
    namespace crfs {
        IMPLEMENT_SUBCLASS(RemoteHandler, "RemoteHandler")
        EMPTY_SLOTTABLE(RemoteHandler)
            //EMPTY_SERIALIZER(RemoteHandler)

        RemoteHandler::RemoteHandler() : io_context(){
              
            STANDARD_CONSTRUCTOR()
            tcp_acceptor = std::make_unique<asio::ip::tcp::acceptor>(io_context);
            
            //todo: push this up to station
            background_thread = std::thread(&RemoteHandler::runNetworkThread, this);
		}
        void RemoteHandler::copyData(const RemoteHandler& org, const bool cc)
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
        void RemoteHandler::deleteData() {
            keep_running = false;
            io_context.stop();
            if (background_thread.joinable()) background_thread.join();
            BaseClass::deleteData();

        }
		void RemoteHandler::reset() {
			BaseClass::reset();
		}

       
        void RemoteHandler::runNetworkThread() {
            startAccept(); // Set up the first async accept
            while (keep_running) {
                try {
                    //io_context.run(); // Process all async events
                }
                catch (std::exception& e) {
                    std::cerr << "Asio Thread Error: " << e.what() << std::endl;
                }
            }
        }
        void RemoteHandler::startAccept() {
            auto new_socket = std::make_shared<tcp::socket>(io_context);
            //async lambda for new connections
            tcp_acceptor->async_accept(*new_socket, [this, new_socket](std::error_code ec) {
                if (!ec) {
                    std::cout << "Subscriber connected!" << std::endl;
                    // Transfer ownership to our active socket member
                    this->active_socket = new_socket; 
                }
                
                startAccept();//listen for more connections
            });
        }
        void RemoteHandler::publish(const std::string& message) {
            std::lock_guard<std::mutex> lock(queue_mutex);
            message_queue.push_back(message);
        }
		void RemoteHandler::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (!message_queue.empty() && active_socket && active_socket->is_open()) {
                std::string msg = message_queue.front();
                message_queue.pop_front();

                // Async write so it doesn't block the Mixr thread
                asio::async_write(*active_socket, asio::buffer(msg),
                    [&](std::error_code ec, std::size_t bytesSent) {
                        
                        if (ec) std::cerr << "Write failed: " << ec.message() << std::endl;
                        else {
                            std::cout << "Sent: " << msg << std::endl;
                        }
                    });
            }
		}


		void RemoteHandler::updateTC(const double dt) {
            static double timer = 0.0;
            static int published = 1;
            timer += dt;
            if (timer > 1.0) {
                timer = 0.0;
                publish(std::to_string(published++));
            }

			BaseClass::updateTC(dt);
		}
	}
}