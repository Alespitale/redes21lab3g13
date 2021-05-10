#ifndef TRANSPORTTX
#define TRANSPORTTX

#include <string.h>
#include <omnetpp.h>

#include "Feedback_m.h"

using namespace omnetpp;

class TransportTx: public cSimpleModule {
private:
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cOutVector sentPacket;

    int ReceiverBuffer; // Receiver window
    bool congestion; // If middle queue is full between receiver's buffer and transmitter's buffer
    bool generationStopped; //
    int burst; // Packets sent successfully in a row
public:
    TransportTx();
    virtual ~TransportTx();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(TransportTx);

TransportTx::TransportTx() {
    endServiceEvent = NULL;
}

TransportTx::~TransportTx() {
    cancelAndDelete(endServiceEvent);
}

void TransportTx::initialize() {
    buffer.setName("buffer");
    endServiceEvent = new cMessage("endService");
    congestion = false;
    ReceiverBuffer = 1;
}

void TransportTx::finish() {
}

void TransportTx::handleMessage(cMessage *msg) {
    // If the message is kind 2, it means there is at least one impact on congestion or flow control issues
    if (msg->getKind() == 2) {
        Feedback * f = (Feedback *)msg;
        ReceiverBuffer = f->getReceiverBuffer();
        congestion = f->getCongestion();
        delete(msg);
    } else if (msg == endServiceEvent) {
        if(ReceiverBuffer > 0 && !congestion) {
            // If receiver has available storage space (Receiver's buffer)
            // and the network is not congested
            if(!buffer.isEmpty()) {
                cPacket * pkt = (cPacket *)buffer.pop();
                send(pkt, "toOut$o");
                serviceTime = pkt->getDuration();
                scheduleAt(simTime() + serviceTime, endServiceEvent);
                // When it sends a message we assume this message will arrive to destination and
                // the transmitter will have at least one space to store packets
                ReceiverBuffer--;
                sentPacket.record(1);
            }
        } else if (ReceiverBuffer <= 0 || congestion) {
            // If there is network congestion or there's not space available in the receiver's buffer,
            // the message is delayed to avoid packet loss
            scheduleAt(simTime() + 0.5, endServiceEvent);
        }
    } else {
        if(buffer.getLength() >= par("bufferSize").intValue()) {
            // packet loss
            delete(msg);
            packetDropVector.record(1);
        } else {
            buffer.insert(msg);
            bufferSizeVector.record(buffer.getLength());
            if(!endServiceEvent->isScheduled()) {
                scheduleAt(simTime() + 0, endServiceEvent);
            }
        }
    }

}

#endif /* TRANSPORTTX */
