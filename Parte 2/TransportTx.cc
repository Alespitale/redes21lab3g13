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
    generationStopped = false;
    burst = 0;
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
                // Counter to increase sending speed in case the transmitter sends
                // successful messages in a row
                burst++;
                sentPacket.record(1);
            }
        } else if (ReceiverBuffer == 0 || congestion) {
            // If there is network congestion or there's not space available in the receiver's buffer,
            // the message is delayed to avoid packet loss
            scheduleAt(simTime() + 0.1, endServiceEvent);
            burst = 0;
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

    // If transmitter's buffer is full, it slows down packet generation speed
    if(buffer.getLength() >= par("bufferSize").intValue()) {
        cSimulation * sim = sim->getActiveSimulation();
        cModule * gen = sim->getModuleByPath("Network.nodeTx.gen");
        gen->par("generationInterval").setDoubleValue(gen->par("generationInterval").doubleValue() * 1.4);
        generationStopped = true;
    }
    // if 20 message was sent without packet loss and generationStopped is false,
    // packet generation speed will increase.
    if(!generationStopped && burst >= 20) {
        cSimulation * sim = sim->getActiveSimulation();
        cModule * gen = sim->getModuleByPath("Network.nodeTx.gen");
        gen->par("generationInterval").setDoubleValue(gen->par("generationInterval").doubleValue() / 1.2);
        burst = 0;
    }
    // If the packet generation speed has been slow down and 10% of transmitter's buffer space is freed
    // then packet generation speed increases again
    if (generationStopped && (buffer.getLength() <= par("bufferSize").intValue() * 0.9)) {
        generationStopped = false;
        cSimulation * sim = sim->getActiveSimulation();
        cModule * gen = sim->getModuleByPath("Network.nodeTx.gen");
        gen->par("generationInterval").setDoubleValue(gen->par("generationInterval").doubleValue() / 1.2);
    }
}

#endif /* TRANSPORTTX */
