#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"


// Default Network Topology
//
//   Wifi 10.1.3.0
//                     AP
//  *    *    *    *   *
//  |    |    |    |   |    10.1.1.0
// n6   n7   n8   n9   n0 -------------- n1   n2   n3   n4   n5 
//                        point-to-point  |    |    |    |    |
//                                        *    *    *    *    * 
//                                       AP
//                                           Wifi 10.1.2.0
#define uint_32 uint
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CustomNetworkExample");

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
}


Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */

void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}

int main(int argc, char *argv[]){
    uint nWifiL = 4;
    uint nWifiR = 4;

    uint payloadSize = 1042;                       /* Transport layer payload size in bytes. */
    std::string dataRate = "100Mbps";                  /* Application layer datarate. */
    std::string tcpVariant = "ns3::TcpNewReno";             /* TCP variant type. */
    // std::string phyRate = "HtMcs7";                    /* Physical layer bitrate. */
    double simulationTime = 5;                        /* Simulation time in seconds. */
    // bool pcapTracing = false;     

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpVariant));
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));


    //try and create nodes
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install (p2pNodes);

    // csma.SetDeviceAttribute("RateErrorModel", );
    


    NodeContainer wifiStaNodesL, wifiStaNodesR;
    wifiStaNodesL.Create (nWifiL);
    wifiStaNodesR.Create (nWifiR);

    NodeContainer wifiApNodeL = p2pNodes.Get (0);
    NodeContainer wifiApNodeR = p2pNodes.Get (1);

    // std::cout << "Hello\n";
    YansWifiChannelHelper LwifiChannel = YansWifiChannelHelper::Default ();
    YansWifiChannelHelper RwifiChannel = YansWifiChannelHelper::Default ();
    YansWifiPhyHelper LwifiPhy;
    YansWifiPhyHelper RwifiPhy;
    RwifiPhy.SetErrorRateModel("ns3::NistErrorRateModel");
    // std::cout << "Hello2\n";

    LwifiPhy.SetChannel (LwifiChannel.Create ());
    RwifiPhy.SetChannel (RwifiChannel.Create ());
    

    WifiMacHelper LwifiMac;
    WifiMacHelper RwifiMac;

    WifiHelper LwifiHelper;
    WifiHelper RwifiHelper;
    
    LwifiHelper.SetRemoteStationManager ("ns3::AarfWifiManager");
    RwifiHelper.SetRemoteStationManager ("ns3::AarfWifiManager");

    Ssid ssid = Ssid ("ns-3-ssid");
    LwifiMac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
    RwifiMac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
    
    NetDeviceContainer LstaDevices;
    NetDeviceContainer RstaDevices;
    LstaDevices = LwifiHelper.Install(LwifiPhy, LwifiMac, wifiStaNodesL);
    RstaDevices = RwifiHelper.Install(RwifiPhy, RwifiMac, wifiStaNodesR);


    LwifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

    RwifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
    
    NetDeviceContainer LapDevices;
    NetDeviceContainer RapDevices;
    
    LapDevices = LwifiHelper.Install(LwifiPhy, LwifiMac, wifiApNodeL);
    RapDevices = RwifiHelper.Install(RwifiPhy, RwifiMac, wifiApNodeR);


    //set mobility of sta devices
    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (0.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (5.0),
                                    "DeltaY", DoubleValue (10.0),
                                    "GridWidth", UintegerValue (3),
                                    "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.Install (wifiStaNodesL);
    mobility.Install (wifiStaNodesR);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    mobility.Install (wifiApNodeL);    
    mobility.Install (wifiApNodeR);    

    //IP Layer

    
    InternetStackHelper stack;   
    stack.Install(wifiStaNodesL);
    stack.Install(wifiApNodeL);

    stack.Install(wifiApNodeR);
    stack.Install(wifiStaNodesR);

    //set address
    Ipv4AddressHelper address;

    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign (p2pDevices); //Address between Routers

    address.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer staInterfacesR = address.Assign (RstaDevices);
    Ipv4InterfaceContainer apInterfacesR = address.Assign (RapDevices);


    address.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer staInterfacesL = address.Assign (LstaDevices);
    Ipv4InterfaceContainer apInterfacesL = address.Assign (LapDevices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    /*std::vector<Ptr<PacketSink>> sinksR;
    sinksR.reserve(5);
    for(uint i = 0; i< 5; i++){
        uint16_t port = 8000+i;
        PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
        if(i==0){
            ApplicationContainer sinkApp = sinkHelper.Install (wifiApNodeR);
            Ptr<PacketSink> packetSink = sinkApp.Get(0)->GetObject<PacketSink>;
        }
        else{

        }
    }*/
    uint16_t port = 8080;
    Address sinkAddress(InetSocketAddress(staInterfacesR.GetAddress(0), port));
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApp = sinkHelper.Install (wifiStaNodesR.Get(0));
    sink = StaticCast<PacketSink> (sinkApp.Get (0));

    // std::cout << "Sink: " << sink->GetNode() << std::endl;

    sinkApp.Start (Seconds (0.));
    sinkApp.Stop (Seconds(simulationTime));

    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (wifiStaNodesL.Get (0), TcpSocketFactory::GetTypeId ());

    Ptr<MyApp> app = CreateObject<MyApp> ();
    app->Setup (ns3TcpSocket, sinkAddress, payloadSize, 50000, DataRate (dataRate));
    wifiStaNodesL.Get (0)->AddApplication (app);
    app->SetStartTime (Seconds (1.));
    app->SetStopTime (Seconds (simulationTime));
    Simulator::Schedule (Seconds (1.1), &CalculateThroughput);

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("customnet.cwnd");
    ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

    PcapHelper pcapHelper;
    Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("customnet.pcap", std::ios::out, PcapHelper::DLT_PPP);
    p2pDevices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));
    
    Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();
    double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * simulationTime));

    Simulator::Destroy ();
    std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;


    return 0;


}

