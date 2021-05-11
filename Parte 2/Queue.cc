#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>

#include "Feedback_m.h"

using namespace omnetpp;

class Queue: public cSimpleModule {
private:
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;

    bool congestion;
public:
    Queue();
    virtual ~Queue();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Queue);

Queue::Queue() {
    endServiceEvent = NULL;
}

Queue::~Queue() {
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize() {
    buffer.setName("buffer");
    endServiceEvent = new cMessage("endService");
    congestion = false;
}

void Queue::finish() {
}

void Queue::handleMessage(cMessage *msg) {

    // if msg is signaling an endServiceEvent
    if (msg == endServiceEvent) {
        // if packet in buffer, send next one
        if (!buffer.isEmpty()) {
            // dequeue packet
            cPacket *pkt = (cPacket*) buffer.pop();
            // send packet
            send(pkt, "out");
            // start new service
            serviceTime = pkt->getDuration();
            scheduleAt(simTime() + serviceTime, endServiceEvent);
        }
    } else { // if msg is a data packet
        // If queue's buffer fills up, it will send a message to indicate
        // there is a network congestion issue
        if (buffer.getLength() == par("bufferSize").intValue() -1 && !congestion) {
            Feedback * f = new Feedback();
            f->setKind(2);
            f->setByteLength(20);
            f->setCongestion(true);
            buffer.insertBefore(buffer.front(), f); // first packet to send
            congestion = true;
        }
        // check buffer limit
        if (buffer.getLength() >= par("bufferSize").intValue()) {
            // drop packet
            delete msg;
            packetDropVector.record(1);
        } else {
            // enqueue the packet
            buffer.insert(msg);
            bufferSizeVector.record(buffer.getLength());
            // if the server is idle
            if (!endServiceEvent->isScheduled()) {
                // start the service now
                scheduleAt(simTime() + 0, endServiceEvent);
            }
        }
    }
    // it sends a message to indicate network congestion issue was solved when 25%
    // of queue's buffer space is freed
    if (congestion && (buffer.getLength() <= par("bufferSize").intValue() * 0.75)) {
        congestion = !congestion;
        Feedback * f = new Feedback();
        f->setKind(2);
        f->setByteLength(20);
        f->setCongestion(false);
        buffer.insertBefore(buffer.front(), f); // first packet to send
    }

}

#endif /* QUEUE */
