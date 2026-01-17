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
            "nanoSecInterval"
            END_SLOTTABLE(CPR_Generator)

        BEGIN_SLOT_MAP(CPR_Generator)
            ON_SLOT(1, setSlotInterfaceIpString, mixr::base::String)
            ON_SLOT(2, setSlotInterfaceHostOutgoingPort, mixr::base::Integer)
            ON_SLOT(3, setClients, mixr::base::PairStream)
            ON_SLOT(4, setNanoSecondMsgInterval, mixr::base::Integer)
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

        bool CPR_Generator::setNanoSecondMsgInterval(const mixr::base::Integer* const sleeptime) {
            bool returnVal = false;
            if (sleeptime != nullptr) {
                nanosecInterval = std::chrono::nanoseconds(sleeptime->asInt());
                returnVal = true;
            }
            return returnVal;
        }

        CPR_Generator::CPR_Generator() : io_context(){
              
            STANDARD_CONSTRUCTOR()
            seq_ = 0;
            
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
            io_context.stop();
            if (udpThread->joinable()) udpThread->join();
            BaseClass::deleteData();

        }
		void CPR_Generator::reset() {
			BaseClass::reset();
            //this would have to be coordinated with the subscriber
            //add_client(udp::endpoint(asio::ip::make_address("127.0.0.1"), 5001));
            udp_endpoint = std::make_shared<asio::ip::udp::endpoint>(asio::ip::make_address(interface_ip), udp_port);
            socket_ptr = std::make_unique<asio::ip::udp::socket>(io_context, *udp_endpoint);
            udpThread = std::move(std::make_unique<std::thread>(std::thread(&CPR_Generator::runNetworkThread, this)));
		}


        //this function needs to be tied to entities subscribing.
        void CPR_Generator::add_client(const udp::endpoint& ep) {
            auto c = std::make_unique<Client>();
            c->endpoint = ep;
            myClients.push_back(std::move(c));
        }
        
        void CPR_Generator::runNetworkThread() {
            //todo: wait for reset


            SetThreadDescription(GetCurrentThread(), L"CPR_Generator IO Context");
            //we want an asynchronous send - we want to process as many CPR messages as possible
            

            // 100 Hz = 10 millisecond interval
            
            auto next_tick = std::chrono::steady_clock::now() + nanosecInterval;

            while (true) {
                // 1. Perform your networking work
                tick();

                // 2. Wait until exactly the next 10ms boundary
                std::this_thread::sleep_until(next_tick);

                // 3. Increment the goalpost by exactly 10ms
                next_tick += nanosecInterval;
            }
            
        }

        float CPR_Generator::compute_value_for(const Client& c) {
            // Example: client-specific data
            return static_cast<float>(seq_) * 0.01f;
        }

        void CPR_Generator::tick() {//initiator

            //precise time for send time, for sensor TDOA calculations
            const uint64_t now_ns = duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            for (auto& c : myClients) {//clients_ is a vector of UDP sensors listening for data
                //todo we need to account for all the packets generated since the last tick (real time thread and tick thread likely different rates)
                //can avoid sending repeats with packetSent flag, need to handle the case where real time is faster than network
                
                if (c->packetSent == false)
                {
                    //using a shared pointer so the object stays in scope for asio async
                    //and CRFS can keep updating the world - after the copy
                    std::shared_ptr<CPR_Packet> packet_ptr;
                    {
                        std::scoped_lock lock(c->packet_mutex_);;//RAII - unlocks when out of scope
                        packet_ptr = std::make_shared<CPR_Packet>(c->packet);
                        c->packetSent = true;
                    }//scoped for mutex

                    packet_ptr->seq = seq_;
                    packet_ptr->timestamp_ns = now_ns;

                    socket_ptr->async_send_to(
                        asio::buffer(packet_ptr.get(), sizeof(CPR_Packet)),
                        c->endpoint,
                        [](std::error_code /*ec*/, std::size_t /*bytes*/) {
                            //not bothering with the error code.
                            //not bothering with the number of bytes written.
                            //this is the completion handler as a lambda.  
                            //when this lambda finishes, packet_ptr is freed
                        }
                    );
                }
            }
            ++seq_;
        }

		void CPR_Generator::updateData(const double dt) {
			// Update internal state
			BaseClass::updateData(dt);
		}

		void CPR_Generator::updateTC(const double dt) {
            //tick();
			BaseClass::updateTC(dt);
            std::cout << "server\n";
            //real time thread - update all the values for the connected sensors:
            int i = 0;
            for (auto& c : myClients) {
                std::scoped_lock lock(c->packet_mutex_);;//RAII - unlocks when out of scope
                c->packet.freqStart = i++;//temp allows me to identify who is getting this packet
                c->packet.freqEnd = c->packet.freqEnd + 1;
                c->packetSent = false;
                //calculate all the values
            }

		}
	}
}