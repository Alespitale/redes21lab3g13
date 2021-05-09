simple Generator
{
    parameters:
        volatile int packetByteSize;
        volatile double generationInterval; // sec
        @display("i=block/source");
    gates:
        output out;
}

simple Queue
{
    parameters:
        volatile int bufferSize;
        volatile int serviceTime; // sec
        @display("i=block/queue;q=buffer");
    gates:
        input in;
        output out;
}

simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}

simple TransportTx {
    parameters:
        int bufferSize;
        @display("i=block/queue;q=buffer");
    gates:
        input toApp;
        inout toOut;
}

simple TransportRx {
    parameters:
        int bufferSize;
        @display("i=block/queue;q=buffer");
    gates:
        output toApp;
        inout toOut;
}

module NodeTx
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gateio;
    submodules:
        gen: Generator {
            parameters:
                @display("p=75,55");
        }
        traTx: TransportTx {
            parameters:
                @display("p=75,190");
        }
    connections:
        gen.out --> traTx.toApp;
        traTx.toOut$o --> gateio$o;
        gateio$i --> traTx.toOut$i;
}

module NodeRx
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gateio;
    submodules:
        sink: Sink {
            parameters:
                @display("p=75,50");
        }
        traRx: TransportRx {
            parameters:
                @display("p=75,190");
        }
    connections:
        gateio$i --> traRx.toOut$i;
        //case study 1
        traRx.toApp --> {  datarate = 0.5Mbps; } --> sink.in;
        //case study 2
        //traRx.toApp --> { datarate = 1Mbps; } --> sink.in;
        traRx.toOut$o --> gateio$o;
}

network Network
{
    @display("bgl=2");
    submodules:
        nodeTx: NodeTx {
            @display("p=30,30");
        }
        queuei: Queue {
            @display("p=130,30");
        }
        queuev: Queue {
            @display("p=130,130");
        }
        nodeRx: NodeRx {
            @display("p=230,30");
        }
    connections:
        nodeTx.gateio$o --> {  datarate = 1Mbps; delay = 100us; } --> queuei.in;
        //case study 1
        queuei.out --> {  datarate = 1Mbps; delay = 100us; } --> nodeRx.gateio$i;
        //case study 2
        //queue.out --> { datarate = 0.5Mbps; delay = 100us; } --> nodeRx.gateion$i;
        nodeRx.gateio$o --> queuev.in;
        queuev.out --> nodeTx.gateio$i;
}
