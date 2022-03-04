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
 * This is a simple example to test TCP over 802.11n.
 *
 * Network topology:
 *
 *   Wifi 10.1.3.0
 *                     AP
 *  *    *    *    *   *
 *  |    |    |    |   |    10.1.1.0
 * n6   n7   n8   n9   n0 -------------- n1   n2   n3   n4   n5 
 *                        point-to-point  |    |    |    |    |
 *                                        *    *    *    *    * 
 *                                       AP
 *                                           Wifi 10.1.2.0
 *
 */

#include "ns3/config.h"
#include "ns3/string.h"
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

#include "ns3/core-module.h"

#include "ns3/tcp-socket-base.h"

#include "ns3/flow-monitor-module.h"

NS_LOG_COMPONENT_DEFINE ("task-b-modification-test");

using namespace ns3;

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
  // NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */


int
main (int argc, char *argv[])
{
  uint32_t payloadSize = 1024;                       /* Transport layer payload size in bytes. */
  uint32_t n_packets = 50000;                        /* total Number of packets per flow. */
  /* packet/sec -> 128 then datarate is 1 Mbps */
  std::string dataRate = "5Mbps";
  std::string p2pDataRate = "2Mbps";
  // std::string tcpVariant = "ns3::TcpNewReno";             /* TCP variant type. */
  std::string tcpVariant = "ns3::TcpPeach";             /* TCP variant type. */
  std::string recovery = "ns3::TcpClassicRecovery";
  double simulationTime = 20;                        /* Simulation time in seconds. */
  // bool pcapTracing = false;                           /* PCAP Tracing is enabled or not. */
  uint32_t n_half_nodes = 5;                       /* number of total nodes */
  uint32_t n_total_flows = 5;                         /* number of total flows */
  uint32_t n_routers = 2;
  // coverage area
  double deltaX = 1;
  double deltaY = 1;
  uint32_t gridWidth = 10;
  double coverageMaxRange = 20;
  // Select TCP variant
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
  /* Configure TCP Options */
  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType", TypeIdValue (TypeId::LookupByName (recovery)));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));


  NodeContainer p2pNodes;
  p2pNodes.Create (n_routers);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (p2pDataRate));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  p2pDevices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  NodeContainer staWifiNodes_1;
  staWifiNodes_1.Create (n_half_nodes);
  NodeContainer apWifiNode_1 = p2pNodes.Get (0);

  NodeContainer staWifiNodes_2;
  staWifiNodes_2.Create (n_half_nodes);
  NodeContainer apWifiNode_2 = p2pNodes.Get (1);

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel_1 = YansWifiChannelHelper::Default ();
  YansWifiChannelHelper wifiChannel_2 = YansWifiChannelHelper::Default ();

  wifiChannel_1.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(coverageMaxRange));
  wifiChannel_2.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(coverageMaxRange));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy_1;
  YansWifiPhyHelper wifiPhy_2;
  
  wifiPhy_2.SetErrorRateModel ("ns3::YansErrorRateModel");
  
  wifiPhy_1.SetChannel (wifiChannel_1.Create ());
  wifiPhy_2.SetChannel (wifiChannel_2.Create ());

  WifiMacHelper wifiMac_1;
  WifiMacHelper wifiMac_2;

  WifiHelper wifiHelper_1;
  WifiHelper wifiHelper_2;
  
  wifiHelper_1.SetRemoteStationManager ("ns3::AarfWifiManager");
  wifiHelper_2.SetRemoteStationManager ("ns3::AarfWifiManager");

  Ssid ssid = Ssid ("ns-3-ssid");
  /* Configure STA */
  wifiMac_1.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid),
                    "ActiveProbing", BooleanValue (false));
  wifiMac_2.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid),
                    "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices_1;
  NetDeviceContainer staDevices_2;
  staDevices_1 = wifiHelper_1.Install (wifiPhy_1, wifiMac_1, staWifiNodes_1);
  staDevices_2 = wifiHelper_2.Install (wifiPhy_2, wifiMac_2, staWifiNodes_2);

  /* Configure AP */
  wifiMac_1.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));

  wifiMac_2.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));
  
  NetDeviceContainer apDevice_1;
  NetDeviceContainer apDevice_2;

  apDevice_1 = wifiHelper_1.Install (wifiPhy_1, wifiMac_1, apWifiNode_1);
  apDevice_2 = wifiHelper_2.Install (wifiPhy_2, wifiMac_2, apWifiNode_2);

  /* Mobility model */
  MobilityHelper mobility;
  
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (0.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (deltaX),
                                    "DeltaY", DoubleValue (deltaY),
                                    "GridWidth", UintegerValue (gridWidth),
                                    "LayoutType", StringValue ("RowFirst"));

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
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface_1;
  apInterface_1 = address.Assign (apDevice_1);
  Ipv4InterfaceContainer staInterface_1;
  staInterface_1 = address.Assign (staDevices_1);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface_2;
  apInterface_2 = address.Assign (apDevice_2);
  Ipv4InterfaceContainer staInterface_2;
  staInterface_2 = address.Assign (staDevices_2);

  /* Populate routing table */
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  for(uint32_t i=0; i<n_total_flows; ++i){
    uint16_t sinkPort = 8080 + i;
    uint32_t index = i%n_half_nodes;
    Address sinkAddress (InetSocketAddress (staInterface_2.GetAddress (index), sinkPort));
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
    ApplicationContainer sinkApp = sinkHelper.Install (staWifiNodes_2.Get(index));
    sink = StaticCast<PacketSink> (sinkApp.Get (0));

    /* Start Applications */
    sinkApp.Start (Seconds (0.));
    sinkApp.Stop (Seconds(simulationTime));

    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (staWifiNodes_1.Get (index), TcpSocketFactory::GetTypeId ());
    Ptr<MyApp> serverApp = CreateObject<MyApp> ();
    serverApp->Setup(ns3TcpSocket, sinkAddress, payloadSize, n_packets, DataRate(dataRate));
    staWifiNodes_1.Get(index)->AddApplication(serverApp);

    /* Start Applications */
    serverApp->SetStartTime (Seconds (1.));
    serverApp->SetStopTime (Seconds(simulationTime));

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("./output-files/task-b-peach-" + std::to_string(i) + ".cwnd");
    ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
  }

  /* Start Simulation */
  Simulator::Stop (Seconds (simulationTime));

      // ------- flow monitor declaration ------------- //

  FlowMonitorHelper flowmon;                              
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Run ();

  monitor->SerializeToXmlFile("./output-files/task-b-peach-flow.xml", true, true);

  Simulator::Destroy ();
  
  return 0;
}
