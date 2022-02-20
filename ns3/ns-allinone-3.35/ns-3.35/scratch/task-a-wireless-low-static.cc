/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Universita' di Firenze, Italy
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
 * Author: Tommaso Pecorella <tommaso.pecorella@unifi.it>
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"

#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include <ns3/lr-wpan-error-model.h>
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv6-flow-classifier.h"

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

int main (int argc, char** argv)
{
  bool verbose = false;

  Packet::EnablePrinting ();

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanMac", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanPhy", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanNetDevice", LOG_LEVEL_ALL);
      LogComponentEnable ("SixLowPanNetDevice", LOG_LEVEL_ALL);
    }
  
  uint32_t nWsnNodes = 4;
  uint32_t n_total_flows = 4;                         /* number of total flows */
  uint32_t payloadSize = 128;                       /* Transport layer payload size in bytes. */
  uint32_t n_packets = 50000;
  std::string dataRate = "100Mbps";
  double simulationTime = 30;                        /* Simulation time in seconds. */
  std::string tcpVariant = "ns3::TcpNewReno";             /* TCP variant type. */
  std::string recovery = "ns3::TcpClassicRecovery";
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));
  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType", TypeIdValue (TypeId::LookupByName (recovery)));
  // coverage area
  double deltaX = 80;
  double deltaY = 80;
  uint32_t gridWidth = 10;
  double coverageMaxRange = 100;

  // Config::SetDefault ("ns3::LogDistancePropagationLossModel::Exponent", DoubleValue (5)); 

  NodeContainer wsnNodes;
  wsnNodes.Create (nWsnNodes);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (deltaX),
                                 "DeltaY", DoubleValue (deltaY),
                                 "GridWidth", UintegerValue (gridWidth),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wsnNodes);

  Config::SetDefault ("ns3::RangePropagationLossModel::MaxRange", DoubleValue (coverageMaxRange));
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<RangePropagationLossModel> propModel = CreateObject<RangePropagationLossModel> ();
  channel->AddPropagationLossModel (propModel);

  LrWpanHelper lrWpanHelper;
  lrWpanHelper.SetChannel (channel);
  // Add and install the LrWpanNetDevice for each node
  NetDeviceContainer lrwpanDevices = lrWpanHelper.Install (wsnNodes);
  // Config::Set("/NodeList/1/DeviceList/0/$ns3::LrWpanNetDevice/Channel/PropagationLossModel/$ns3::RangePropagationLossModel", DoubleValue(1));

  // Fake PAN association and short address assignment.
  // This is needed because the lr-wpan module does not provide (yet)
  // a full PAN association procedure.
  lrWpanHelper.AssociateToPan (lrwpanDevices, 0);

  InternetStackHelper internetv6;
  internetv6.Install (wsnNodes);

  SixLowPanHelper sixLowPanHelper;
  NetDeviceContainer sixLowPanDevices = sixLowPanHelper.Install (lrwpanDevices);

  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:f00d::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wsnDeviceInterfaces;
  wsnDeviceInterfaces = ipv6.Assign (sixLowPanDevices);
  wsnDeviceInterfaces.SetForwarding (0, true);
  wsnDeviceInterfaces.SetDefaultRouteInAllNodes (0);

  for (uint32_t i = 0; i < sixLowPanDevices.GetN (); i++)
    {
      Ptr<NetDevice> dev = sixLowPanDevices.Get (i);
      dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
      dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
    }
  
  for(uint32_t i=0; i<n_total_flows; ++i){
    uint16_t sinkPort = 8080 + i;
    uint32_t index = i%nWsnNodes;
    if(index == 0){
      index = nWsnNodes-1;
    }
    Address sinkAddress (Inet6SocketAddress (wsnDeviceInterfaces.GetAddress (0, 1), sinkPort));
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), sinkPort));
    ApplicationContainer sinkApp = sinkHelper.Install (wsnNodes.Get(0));

    /* Start Applications */
    sinkApp.Start (Seconds (0.));
    sinkApp.Stop (Seconds(simulationTime));

    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (wsnNodes.Get (index), TcpSocketFactory::GetTypeId ());
    Ptr<MyApp> serverApp = CreateObject<MyApp> ();
    serverApp->Setup(ns3TcpSocket, sinkAddress, payloadSize, n_packets, DataRate(dataRate));
    wsnNodes.Get(index)->AddApplication(serverApp);

    /* Start Applications */
    serverApp->SetStartTime (Seconds (1.));
    serverApp->SetStopTime (Seconds(simulationTime));
  }

  Simulator::Stop (Seconds (simulationTime));

  FlowMonitorHelper flowmon;                              
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Run ();

  monitor->SerializeToXmlFile("task-a-2-flow.xml", true, true);

  Simulator::Destroy ();
  return 0;
}

