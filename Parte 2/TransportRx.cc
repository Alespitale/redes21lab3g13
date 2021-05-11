#ifndef TRANSPORTRX
#define TRANSPORTRX

#include <string.h>
#include <omnetpp.h>

#include "Feedback_m.h"

using namespace omnetpp;

class TransportRx: public cSimpleModule {
private:
    cQueue buffer;
    cMessage * endServiceEvent;
    simtime_t serviceTime;
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cOutVector recivedPacket;

    bool congestion;
public:
    TransportRx();
    virtual ~TransportRx();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage * msg);
};

Define_Module(TransportRx);

TransportRx::TransportRx() {
    endServiceEvent = NULL;
}

TransportRx::~TransportRx() {
    cancelAndDelete(endServiceEvent);
}

void TransportRx::initialize() {
    buffer.setName("RxBuffer");
    endServiceEvent = new cMessage("endServiceEvent");
    congestion = false;
}

void TransportRx::finish() {
}

void TransportRx::handleMessage(cMessage * msg) {
        // If the message is kind 2, it means there is network congestion
        if(msg->getKind() == 2) {
            Feedback * pkt = (Feedback *)msg;
            congestion = pkt->getCongestion();
        } else {  // Otherwise it's a "endServiceEvent" message or a data's message
            // if msg is signaling an endServiceEvent
            if (msg == endServiceEvent) {
                // if packet in buffer, send next one
                if (!buffer.isEmpty()) {
                    // dequeue packet
                    cPacket *pkt = (cPacket*) buffer.pop();
                    // send packet
                    send(pkt, "toApp");
                    // start new service
                    serviceTime = pkt->getDuration();
                    scheduleAt(simTime() + serviceTime, endServiceEvent);
                }
            } else { // if msg is a data packet
                // check buffer limit
                if (buffer.getLength() >= par("bufferSize").intValue()) { // If buffer is full
                    // drop packet
                    delete msg;
                    packetDropVector.record(1);
                } else {
                    // enqueue the packet
                    buffer.insert(msg);
                    bufferSizeVector.record(buffer.getLength());
                    recivedPacket.record(1);
                    // if the server is idle
                    if (!endServiceEvent->isScheduled()) {
                        // start the service now
                        scheduleAt(simTime() + 0, endServiceEvent);
                    }
                }
            }
            // We always send a message with necessary network information to decrease
            // packet loss
            Feedback * f = new Feedback();
            f->setKind(2);
            f->setByteLength(20);
            f->setReceiverBuffer(par("bufferSize").intValue() - buffer.getLength() - 5);
            f->setCongestion(congestion);
            send(f,"toOut$o");
        }
}

#endif
