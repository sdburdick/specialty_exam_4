#include "core/CPR_Generator.h"
#include "mixr/base/String.hpp"
#include "mixr/base/numeric/Integer.hpp"
#include "mixr/base/PairStream.hpp"


//builds and sends the CRFS Packet for Receivers, or CPR.  Determines what energy stimulated each definied receiver -
//what power, what frequency, what angle, and what time of arrival

namespace mixr {
    namespace crfs {
        IMPLEMENT_SUBCLASS(CPR_Generator, "CPR_Generator")

            //EMPTY_SERIALIZER(CPR_Generator)
            BEGIN_SLOTTABLE(CPR_Generator)
            "interfaceIpString",  //outgoing IP interface on this computer.  Needs to be able to connect to the net you are transmitting to
            "interfaceHostOutgoingPort", //outgoing port on this computer.  does not match the connected clients, just needs to be free and usable
            "clients",
            END_SLOTTABLE(CPR_Generator)

        BEGIN_SLOT_MAP(CPR_Generator)
            ON_SLOT(1, setSlotInterfaceIpString, mixr::base::String)
            ON_SLOT(2, setSlotInterfaceHostOutgoingPort, mixr::base::Integer)
            ON_SLOT(3, setClients, mixr::base::PairStream)
            return _ok;
        }
        
        bool CPR_Generator::setSlotInterfaceIpString(const mixr::base::String* const name) {
            bool returnVal = false;
            if (name != nullptr) {
                interface_ip = name->c_str();
                returnVal = true;
            }
            return returnVal;
        }
        bool CPR_Generator::setSlotInterfaceHostOutgoingPort(const mixr::base::Integer* const port) {
            bool returnVal = false;
            if (port != nullptr) {
                udp_port = port->asInt();
                returnVal = true;
            }
            return returnVal;
        }
        bool CPR_Generator::setClients(const mixr::base::PairStream* const inputfile_clients)
        {
            bool returnVal = true;
            if (!inputfile_clients) {
                returnVal = false;
                return returnVal;
            }

            myClients.clear();

            const mixr::base::IList::Item* clientList = (inputfile_clients->getFirstItem());
            while (clientList != nullptr) {
                const mixr::base::Pair* ipPair = dynamic_cast<const mixr::base::Pair*>(clientList->getValue());
                if (!ipPair) {
                    std::cerr << "No ipPair in CPR_Generator\n"; 
                    returnVal = false;
                    break;
                }

                const mixr::base::PairStream* ps = dynamic_cast<const mixr::base::PairStream*>(ipPair->object());
                if (!ps) {
                    std::cerr << "No ps in CPR_Generator\n";
                    returnVal = false;
                    break;
                }

                const mixr::base::IList::Item* ipItem = ps->getFirstItem();
                if (!ipItem) {
                    std::cerr << "No ipItem in CPR_Generator\n";
                    returnVal = false;
                    break;
                }

                const mixr::base::Pair* ipAddressPair = dynamic_cast<const mixr::base::Pair*>(ipItem->getValue());
                if (!ipAddressPair) {
                    std::cerr << "No ipAddressPair in CPR_Generator\n";
                    returnVal = false;
                    break;
                }

                const auto* ipStr = dynamic_cast<const mixr::base::String*>(ipAddressPair->object());
                if (!ipStr) {
                    std::cerr << "Expected IP string\n";
                    returnVal = false;
                    break;
                }
                std::string ip = ipStr->c_str();

                ipItem = ipItem->getNext();
                if (!ipItem) {
                    std::cerr << "No ipItem2 in CPR_Generator\n";
                    returnVal = false;
                    break;
                }

                const mixr::base::Pair* portObjPair = dynamic_cast<const mixr::base::Pair*>(ipItem->getValue());
                if (!portObjPair) {
                    std::cerr << "No portObjPair in CPR_Generator\n";
                    returnVal = false;
                    break;
                }

                const auto* portObj = dynamic_cast<const mixr::base::INumber*>(portObjPair->object());
                if (!portObj) {
                    std::cerr << "Expected port number\n";
                    returnVal = false;
                    break;
                }
                int port = portObj->asInt();

                if (returnVal) {
                    add_client(udp::endpoint(asio::ip::make_address(ip), port));
                }
                clientList = clientList->getNext();
            }

            return returnVal;
        }

        CPR_Generator::CPR_Generator() : io_context(){
            STANDARD_CONSTRUCTOR()
                
		}
        void CPR_Generator::copyData(const CPR_Generator& org, const bool cc)
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
        void CPR_Generator::deleteData() {
            //cleanup asio threading
            asio_work_guard.reset();
            io_context.stop();
            if (udpThread->joinable()) {
                udpThread->join();
            }
            if (socket_ptr) {
                std::error_code ec;
                socket_ptr->close(ec);
                socket_ptr.reset();
            }
            BaseClass::deleteData();

        }
		void CPR_Generator::reset() {
			BaseClass::reset();
            asio_work_guard.reset();
            if (!io_context.stopped()) io_context.stop();
            if (udpThread && udpThread->joinable()) udpThread->join();
            io_context.restart();
            asio_work_guard.emplace(asio::make_work_guard(io_context));

            udp_endpoint = std::make_shared<asio::ip::udp::endpoint>(asio::ip::make_address(interface_ip), udp_port);
            udp_endpoint2 = std::make_shared<asio::ip::udp::endpoint>(asio::ip::make_address(interface_ip), udp_port+1);
            socket_ptr = std::make_unique<asio::ip::udp::socket>(io_context, *udp_endpoint);
            socket_ptr2 = std::make_unique<asio::ip::udp::socket>(io_context, *udp_endpoint2);
            udpThread = std::make_unique<std::thread>(std::thread(&CPR_Generator::runNetworkThread, this));
		}


        //this function needs to be tied to entities subscribing.
        void CPR_Generator::add_client(const udp::endpoint& ep) {
            auto c = std::make_unique<Client>();
            c->endpoint = ep;
            myClients.push_back(std::move(c));
        }
        
        //async write queues
        //async accept loops
        //async read loops
        //async timers
        //async state machines

        //Producer thread pushes packets into c->packets (TimeCritical)
        //Network thread calls send_next(c)
        //Each async send schedules the next one
        //Queue drains safely and sequentially

        void CPR_Generator::transmit_CPR_for_client(Client* c) {//initiator
            //precise time for send time, for sensor TDOA calculations
            std::shared_ptr<CPR_Packet> packet_ptr;
            {
                std::lock_guard<std::mutex> lock(c->packet_mutex_);//RAII
                if (c->sending || c->packets.empty()) {
                    return;
                }
                c->sending = true;
                //can't just run through the deque - it needs to happen after the previous message was a success


                packet_ptr = c->packets.front();
                c->packets.pop_front();//remove it
            }
            //we have ownership of the packet, if it exists
            if (packet_ptr != nullptr) {
                packet_ptr->timestamp_ns = duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();;
                static bool temp = true;
                if (temp) {
                    temp = false;
                    socket_ptr->async_send_to(
                        asio::buffer(packet_ptr.get(), sizeof(CPR_Packet)),
                        c->endpoint,
                        /*lambda to be called when completed*/ [this, c, packet_ptr]//capture 'this' to call next transmit //capture c for "send next packet after completion" //capture the packet_ptr for scope
                        (std::error_code /*ec*/, std::size_t /*bytes*/) {
                            //not bothering with the error code.
                            //not bothering with the number of bytes written.
                            //this is the completion handler as a lambda.  
                            //when this lambda finishes, packet_ptr is freed
                            //send next packet after completion:
                            {
                                std::lock_guard<std::mutex> lock(c->packet_mutex_);
                                c->sending = false;
                            }
                            transmit_CPR_for_client(c);

                        }//packet_ptr descopes from the lambda
                    );
                }
                else {
                    temp = true;
                    socket_ptr2->async_send_to(
                        asio::buffer(packet_ptr.get(), sizeof(CPR_Packet)),
                        c->endpoint,
                        /*lambda to be called when completed*/ [this, c, packet_ptr]//capture 'this' to call next transmit //capture c for "send next packet after completion" //capture the packet_ptr for scope
                        (std::error_code /*ec*/, std::size_t /*bytes*/) {
                            //not bothering with the error code.
                            //not bothering with the number of bytes written.
                            //this is the completion handler as a lambda.  
                            //when this lambda finishes, packet_ptr is freed
                            //send next packet after completion:
                            {
                                std::lock_guard<std::mutex> lock(c->packet_mutex_);
                                c->sending = false;
                            }
                            transmit_CPR_for_client(c);

                        }//packet_ptr descopes from the lambda
                    );
                }
            }//packet_ptr descopes from the send thread
        }

        void CPR_Generator::runNetworkThread()
        {
#ifdef _WIN32
            SetThreadDescription(GetCurrentThread(), L"CPR_Generator runNetworkThread");
#endif
            // Run ASIO event loop
            io_context.run();
            io_context.run();
        }


		void CPR_Generator::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}

		void CPR_Generator::updateTC(const double dt) {
			BaseClass::updateTC(dt);
            
            //real time thread - update all the values for the connected sensors:
            int i = 0;//just counts off which client it is
            for (auto& c : myClients) {
                auto pkt = std::make_shared<CPR_Packet>();
                
                //just filling in a little unique data to show effectiveness and track missing packets
                pkt->seq = ++(c->messageCount);

                bool startAsio = false;
                {//RAII - unlocks when out of scope
                    std::lock_guard<std::mutex> lock(c->packet_mutex_);

                    if (!c->sending && c->packets.empty()) {
                        startAsio = true;
                    }

                    c->packets.push_back(std::move(pkt));
                    
                }
                
                //purely arbitrary warning
                if (c->packets.size() > 1000) {
                    std::cout << "Warning, CPR Generator packets queued grown to " << c->packets.size() << std::endl;
                }

                if (startAsio) {
                    // Wake the network thread 
                    asio::post(io_context, [this, client = c.get()] {
                        transmit_CPR_for_client(client);//initiator
                        });
                }
            }
		}
	}
}