/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015, IMDEA Networks Institute
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Hany Assasa <hany.assasa@gmail.com>
 * Editor: Asif Ajrof <1705092@ugrad.cse.buet.ac.bd>
.*
 * This is a simple example to test TCP over 802.11n (with MPDU aggregation enabled).
 *
 * Network topology:
 *
 *   Ap    STA
 *   *      *
 *   |      |
 *   n1     n2
 *
 * In this example, an HT station sends TCP packets to the access point.
 * We report the total throughput received during a window of 100ms.
 * The user can specify the application data rate and choose the variant
 * of TCP i.e. congestion control algorithm to use.
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-module.h"

NS_LOG_COMPONENT_DEFINE ("wifi-tcp");

using namespace ns3;

Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */


// class MyApp : public Application
// {
// public:
//   MyApp ();
//   virtual ~MyApp ();

//   /**
//    * Register this type.
//    * \return The TypeId.
//    */
//   static TypeId GetTypeId (void);
//   void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

// private:
//   virtual void StartApplication (void);
//   virtual void StopApplication (void);

//   void ScheduleTx (void);
//   void SendPacket (void);

//   Ptr<Socket>     m_socket;
//   Address         m_peer;
//   uint32_t        m_packetSize;
//   uint32_t        m_nPackets;
//   DataRate        m_dataRate;
//   EventId         m_sendEvent;
//   bool            m_running;
//   uint32_t        m_packetsSent;
// };

// MyApp::MyApp ()
//   : m_socket (0),
//     m_peer (),
//     m_packetSize (0),
//     m_nPackets (0),
//     m_dataRate (0),
//     m_sendEvent (),
//     m_running (false),
//     m_packetsSent (0)
// {
// }

// MyApp::~MyApp ()
// {
//   m_socket = 0;
// }

// /* static */
// TypeId MyApp::GetTypeId (void)
// {
//   static TypeId tid = TypeId ("MyApp")
//     .SetParent<Application> ()
//     .SetGroupName ("Tutorial")
//     .AddConstructor<MyApp> ()
//     ;
//   return tid;
// }

// void
// MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
// {
//   m_socket = socket;
//   m_peer = address;
//   m_packetSize = packetSize;
//   m_nPackets = nPackets;
//   m_dataRate = dataRate;
// }

// void
// MyApp::StartApplication (void)
// {
//   m_running = true;
//   m_packetsSent = 0;
//   m_socket->Bind ();
//   m_socket->Connect (m_peer);
//   SendPacket ();
// }

// void
// MyApp::StopApplication (void)
// {
//   m_running = false;

//   if (m_sendEvent.IsRunning ())
//     {
//       Simulator::Cancel (m_sendEvent);
//     }

//   if (m_socket)
//     {
//       m_socket->Close ();
//     }
// }

// void
// MyApp::SendPacket (void)
// {
//   Ptr<Packet> packet = Create<Packet> (m_packetSize);
//   m_socket->Send (packet);

//   if (++m_packetsSent < m_nPackets)
//     {
//       ScheduleTx ();
//     }
// }

// void
// MyApp::ScheduleTx (void)
// {
//   if (m_running)
//     {
//       Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
//       m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
//     }
// }


void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}

int
main (int argc, char *argv[])
{
  uint32_t payloadSize = 1472;                       /* Transport layer payload size in bytes. */
  uint32_t dataRate_int = 10;                        /* Application layer datarate. */
  std::string tcpVariant = "TcpNewReno";             /* TCP variant type. */
  std::string phyRate = "HtMcs7";                    /* Physical layer bitrate. */
  double simulationTime = 10;                        /* Simulation time in seconds. */
  bool pcapTracing = true;                           /* PCAP Tracing is enabled or not. */
  uint32_t n_total_nodes = 10;                       /* number of total nodes */
  uint32_t n_total_flows = 6;                         /* number of total flows */

  /* Command line argument parser setup. */
  CommandLine cmd (__FILE__);
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("dataRate", "Application data ate", dataRate_int);
  cmd.AddValue ("tcpVariant", "Transport protocol to use: TcpNewReno, "
                "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat ", tcpVariant);
  cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("pcap", "Enable/disable PCAP Tracing", pcapTracing);
  cmd.AddValue ("n_total_nodes", "number of total nodes", n_total_nodes);
  cmd.AddValue ("n_total_flows", "number of total nodes", n_total_flows);
  cmd.Parse (argc, argv);

  tcpVariant = std::string ("ns3::") + tcpVariant;
  // Select TCP variant
  if (tcpVariant.compare ("ns3::TcpWestwoodPlus") == 0)
    {
      // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
  else
    {
      TypeId tcpTid;
      NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    }

  /* Configure TCP Options */
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  /* data rate */
  std::string dataRate = std::to_string(dataRate_int) + "Mbps";                  /* Application layer datarate. */

  /* node and flow numbers */
  if (n_total_nodes%2 != 0){
    n_total_nodes += 1;
  }
  uint32_t n_routers = 2;
  n_total_nodes -= n_routers;
  uint32_t n_half_nodes = n_total_nodes / 2;

  if (n_total_flows%2 != 0){
    n_total_flows += 1;
  }
  uint32_t n_half_flows = n_total_flows / 2;
  std::cout<<n_half_flows<<std::endl;

  NodeContainer p2pNodes;
  p2pNodes.Create (n_routers);

  std::string p2p_dataRate = std::to_string(dataRate_int / 5) + "Mbps";
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (p2p_dataRate));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  wifiHelper.SetStandard (WIFI_STANDARD_80211n_5GHZ);

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.SetErrorRateModel ("ns3::YansErrorRateModel");
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode", StringValue (phyRate),
                                      "ControlMode", StringValue ("HtMcs0"));

  NodeContainer staWifiNodes_1;
  staWifiNodes_1.Create (n_half_nodes);
  // Ptr<Node> apWifiNode = networkNodes.Get (0);
  // Ptr<Node> staWifiNode = networkNodes.Get (1);
  NodeContainer apWifiNode_1 = p2pNodes.Get (0);

  NodeContainer staWifiNodes_2;
  staWifiNodes_2.Create (n_half_nodes);
  // Ptr<Node> apWifiNode = networkNodes.Get (0);
  // Ptr<Node> staWifiNode = networkNodes.Get (1);
  NodeContainer apWifiNode_2 = p2pNodes.Get (1);

  /* Configure AP */
  Ssid ssid_1 = Ssid ("network");
  wifiMac.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid_1));

  NetDeviceContainer apDevice_1;
  apDevice_1 = wifiHelper.Install (wifiPhy, wifiMac, apWifiNode_1);

  Ssid ssid_2 = Ssid ("network");
  wifiMac.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid_2));

  NetDeviceContainer apDevice_2;
  apDevice_2 = wifiHelper.Install (wifiPhy, wifiMac, apWifiNode_2);

  /* Configure STA */
  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid_1));

  NetDeviceContainer staDevices_1;
  staDevices_1 = wifiHelper.Install (wifiPhy, wifiMac, staWifiNodes_1);

  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid_2));

  NetDeviceContainer staDevices_2;
  staDevices_2 = wifiHelper.Install (wifiPhy, wifiMac, staWifiNodes_2);

  /* Mobility model */
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for(uint32_t i=0; i< n_half_nodes*2 + n_routers; ++i){
    positionAlloc->Add (Vector (0.0 + i, 0.0, 0.0));
  }
  
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  
  mobility.Install (staWifiNodes_1);
  mobility.Install (apWifiNode_1);

  mobility.Install (staWifiNodes_2);
  mobility.Install (apWifiNode_2);

  /* Internet stack */
  InternetStackHelper stack;
  stack.Install (staWifiNodes_1);
  stack.Install (apWifiNode_1);

  stack.Install (staWifiNodes_2);
  stack.Install (apWifiNode_2);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.0.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  Ipv4AddressHelper address_1;
  address_1.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface_1;
  apInterface_1 = address_1.Assign (apDevice_1);
  Ipv4InterfaceContainer staInterface_1;
  staInterface_1 = address_1.Assign (staDevices_1);

  Ipv4AddressHelper address_2;
  address_1.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface_2;
  apInterface_2 = address_2.Assign (apDevice_2);
  Ipv4InterfaceContainer staInterface_2;
  staInterface_2 = address_2.Assign (staDevices_2);

  /* Populate routing table */
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /* Install TCP Receiver on the access point */
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (staInterface_2.GetAddress (0), sinkPort));

  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApp = sinkHelper.Install (staWifiNodes_2.Get(0));
  sink = StaticCast<PacketSink> (sinkApp.Get (0));

  /* Install TCP/UDP Transmitter on the station */

  /* OnOffHelper(std::string protocol, ns3::Address address)
   * address – the address of the remote node to send traffic to.
   * Create an OnOffHelper to make it easier to work with OnOffApplications
   */
  // OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (staInterface_2.GetAddress (0), sinkPort)));
  OnOffHelper server ("ns3::TcpSocketFactory", sinkAddress);
  server.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  server.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
  ApplicationContainer serverApp = server.Install (staWifiNodes_1.Get(0));

  // Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (staWifiNodes_1.Get (0), TcpSocketFactory::GetTypeId ());

  // Ptr<MyApp> serverApp = CreateObject<MyApp> ();
  // serverApp->Setup(ns3TcpSocket, sinkAddress, 1040, 1000, DataRate(dataRate));
  // staWifiNodes_1.Get(0)->AddApplication(serverApp);

  /* Start Applications */
  sinkApp.Start (Seconds (0.0));
  serverApp.Start (Seconds (1.0));
  // serverApp->SetStartTime (Seconds (1.0));
  Simulator::Schedule (Seconds (1.1), &CalculateThroughput);

  /* Enable Traces */
  if (pcapTracing)
    {
      wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
      wifiPhy.EnablePcap ("AccessPoint1", apDevice_1);
      wifiPhy.EnablePcap ("AccessPoint2", apDevice_2);
      wifiPhy.EnablePcap ("Station1", staDevices_1);
      wifiPhy.EnablePcap ("Station2", staDevices_2);
    }

  /* Start Simulation */
  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();

  double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * simulationTime));

  Simulator::Destroy ();

  if (averageThroughput < 50)
    {
      NS_LOG_ERROR ("Obtained throughput is not in the expected boundaries!");
      exit (1);
    }
  std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;
  return 0;
}
