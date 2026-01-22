#include "core/CPR_Generator_Proactor.h"
#include "mixr/base/String.hpp"
#include "mixr/base/numeric/Integer.hpp"
#include "mixr/base/PairStream.hpp"


//builds and sends the CRFS Packet for Receivers, or CPR.  Determines what energy stimulated each definied receiver -
//what power, what frequency, what angle, and what time of arrival

namespace mixr {
    namespace crfs {
        IMPLEMENT_SUBCLASS(CPR_Generator_Proactor, "CPR_Generator_Proactor")

            //EMPTY_SERIALIZER(CPR_Generator_Proactor)
            BEGIN_SLOTTABLE(CPR_Generator_Proactor)
            "interfaceIpString",  //outgoing IP interface on this computer.  Needs to be able to connect to the net you are transmitting to
            "interfaceHostOutgoingPort", //outgoing port on this computer.  does not match the connected clients, just needs to be free and usable
            "clients",
            "udpThreads",
            END_SLOTTABLE(CPR_Generator_Proactor)

        BEGIN_SLOT_MAP(CPR_Generator_Proactor)
            ON_SLOT(1, setSlotInterfaceIpString, mixr::base::String)
            ON_SLOT(2, setSlotInterfaceHostOutgoingPort, mixr::base::Integer)
            ON_SLOT(3, setClients, mixr::base::PairStream)
            ON_SLOT(4, setNumUdpThreads, mixr::base::Integer)
            return _ok;
    }

    bool CPR_Generator_Proactor::setSlotInterfaceIpString(const mixr::base::String* const name) {
        bool returnVal = false;
        if (name != nullptr) {
            interface_ip = name->c_str();
            returnVal = true;
        }
        return returnVal;
    }
    bool CPR_Generator_Proactor::setSlotInterfaceHostOutgoingPort(const mixr::base::Integer* const port) {
        bool returnVal = false;
        if (port != nullptr) {
            udp_port = port->asInt();
            returnVal = true;
        }
        return returnVal;
    }
    bool CPR_Generator_Proactor::setClients(const mixr::base::PairStream* const inputfile_clients)
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
    bool CPR_Generator_Proactor::setNumUdpThreads(const mixr::base::Integer* const num) {
        bool returnVal = false;
        if (num != nullptr && num->asInt() > 0) {
            numThreads = num->asInt();
            returnVal = true;
        }
        return returnVal;
    }

    CPR_Generator_Proactor::CPR_Generator_Proactor() : io_context() {
        STANDARD_CONSTRUCTOR()
    }
    void CPR_Generator_Proactor::copyData(const CPR_Generator_Proactor& org, const bool cc)
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
    void CPR_Generator_Proactor::deleteData() {
        io_context.stop();
        for (auto& t : udpThreads) {
            if (t.joinable()) t.join();
        }
        udpThreads.clear();

        BaseClass::deleteData();

    }
    void CPR_Generator_Proactor::reset() {
        BaseClass::reset();
        udp_endpoint = asio::ip::udp::endpoint(asio::ip::make_address(interface_ip), udp_port);
        socket_ptr = std::make_unique<asio::ip::udp::socket>(io_context, udp_endpoint);

        for (auto& t : udpThreads) {
            if (t.joinable()) t.join();
        }
        udpThreads.clear();

        for (size_t i = 0; i < numThreads; ++i) {
            udpThreads.emplace_back([this]() { io_context.run(); });
        }
    }


    //this function needs to be tied to entities subscribing.
    void CPR_Generator_Proactor::add_client(const udp::endpoint& ep) {
        auto c = std::make_unique<ProactorClient>();
        c->endpoint = ep;
        myClients.push_back(std::move(c));
    }

    //Producer thread pushes packets into c->packets (TimeCritical)
    //Network thread calls send_next(c)
    //Each async send schedules the next one
    //Queue drains safely and sequentially

    void CPR_Generator_Proactor::transmit_CPR_for_client(ProactorClient* c, bool /*isNetThread*/)
    {
        asio::post(strand, [this, c]() {
            if (c->packets.empty()) {
                return;
            }

            auto packet_ptr = c->packets.front();
            c->packets.pop_front();

            packet_ptr->timestamp_ns =
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()
                ).count();

            socket_ptr->async_send_to(
                asio::buffer(packet_ptr.get(), sizeof(CPR_Packet)),
                c->endpoint,
                asio::bind_executor(
                    strand,
                    [this, c, packet_ptr](std::error_code ec, std::size_t /*bytes*/) {
                        if (ec) {
                            // socket closed, endpoint unreachable, etc.
                            return;
                        }

                        // send next packet (still serialized by strand)
                        transmit_CPR_for_client(c, true);
                    }
                )
            );
        });
    }

    void CPR_Generator_Proactor::updateData(const double dt) {
        // Update internal state
        BaseClass::updateData(dt);
    }

    void CPR_Generator_Proactor::updateTC(const double dt)
    {
        BaseClass::updateTC(dt);

        for (auto& c : myClients) {
            asio::post(
                strand,
                [this, client = c.get()]() {
                    auto pkt = std::make_shared<CPR_Packet>();
                    pkt->seq = ++client->messageCount;
                    client->packets.push_back(pkt);

                    // debug warning if queue grows too large
                    const auto now = std::chrono::steady_clock::now();
                    if (client->packets.size() > 1000) {
                        if (now - client->lastQueueWarning >= std::chrono::seconds(1)) {
                            client->lastQueueWarning = now;
                            std::cout
                                << "Warning: CPR Generator packets queued = "
                                << client->packets.size()
                                << std::endl;
                        }
                    }

                    if (client->packets.size() == 1) {
                        transmit_CPR_for_client(client, true);
                    }
                }
            );
        }
    }


}
}