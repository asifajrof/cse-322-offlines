/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    | 
// n5   n6   n7   n0 


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  // uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = true;

  CommandLine cmd (__FILE__);
  // cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  // NodeContainer p2pNodes;
  // p2pNodes.Create (2);

  // PointToPointHelper pointToPoint;
  // pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  // pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // NetDeviceContainer p2pDevices;
  // p2pDevices = pointToPoint.Install (p2pNodes);

  // NodeContainer csmaNodes;
  // csmaNodes.Add (p2pNodes.Get (1));
  // csmaNodes.Create (nCsma);

  // CsmaHelper csma;
  // csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  // csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  // NetDeviceContainer csmaDevices;
  // csmaDevices = csma.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  // NodeContainer wifiApNode = p2pNodes.Get (0);

  // --------- creating ap node -------- //
  NodeContainer wifiApNode;
  wifiApNode.Create(1);





  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());





  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);






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
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);





  InternetStackHelper stack;
  // stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);




  Ipv4AddressHelper address;

  // address.SetBase ("10.1.1.0", "255.255.255.0");
  // Ipv4InterfaceContainer p2pInterfaces;
  // p2pInterfaces = address.Assign (p2pDevices);

  // address.SetBase ("10.1.2.0", "255.255.255.0");
  // Ipv4InterfaceContainer csmaInterfaces;
  // csmaInterfaces = address.Assign (csmaDevices);

  // address.SetBase ("10.1.3.0", "255.255.255.0");
  // address.Assign (staDevices);
  // address.Assign (apDevices);

  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces =  address.Assign(staDevices);

  // address.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterfaces =  address.Assign(apDevices);




  UdpEchoServerHelper echoServer (1234);

  // ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  ApplicationContainer serverApps = echoServer.Install (wifiApNode.Get(0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  // UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  UdpEchoClientHelper echoClient (apInterfaces.GetAddress (0), 1234);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));


  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes.Get (nWifi-1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));




  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();




  Simulator::Stop (Seconds (10.0));

  if (tracing)
    {
      phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
      phy.EnablePcapAll("wireless");

      // pointToPoint.EnablePcapAll ("p2p");
      // phy.EnablePcap ("wireless", apDevices.Get (0));
      // csma.EnablePcap ("csma", csmaDevices.Get (0), true);
    }


  // ----- generating trace files ------- //

  AsciiTraceHelper ascii;
  phy.EnableAsciiAll(ascii.CreateFileStream("phy.tr"));
  // csma.EnableAsciiAll(ascii.CreateFileStream("csma.tr"));
  // pointToPoint.EnableAsciiAll(ascii.CreateFileStream("p2p.tr"));


  // ------ generating xml file (for NetAnim) --------- //

  // AnimationInterface anim("test_anim.xml");
  // anim.SetConstantPosition(wifiStaNodes.Get(0), 10.0, 10.0);
  // anim.SetConstantPosition(wifiApNode.Get(0), 20.0, 20.0);



  // ------- flow monitor declaration ------------- //

  FlowMonitorHelper flowmon;                              
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();        


  Simulator::Run ();



  // ------------------------ Network Performance Calculation ------------------------ //

  uint32_t sentPackets = 0;         // added
  uint32_t receivedPackets = 0;     // added
  uint32_t lostPackets = 0;         // added

  int j = 0;
  float avgThroughput = 0;
  Time jitter;
  Time delay;

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier());
  std::map<FlowId, FlowMonitor::FlowStats> stats =  monitor->GetFlowStats();

  for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); iter++)
  {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);

    NS_LOG_UNCOND("\nFlow Id: " << iter->first);
    NS_LOG_UNCOND("Src Addr: " << t.sourceAddress);
    NS_LOG_UNCOND("Dst Addr: " << t.destinationAddress);
    NS_LOG_UNCOND("Sent Packets: " << iter->second.txPackets);
    NS_LOG_UNCOND("Received Packets: " << iter->second.rxPackets);
    // NS_LOG_UNCOND("Lost Packets: " << iter->second.txPackets - iter->second.rxPackets);
    NS_LOG_UNCOND("Lost Packets: " << iter->second.lostPackets);
    NS_LOG_UNCOND("Packet Delivery Ratio: " << iter->second.rxPackets*100/iter->second.txPackets << "%");
    // NS_LOG_UNCOND("Packet Loss Ratio: " << (iter->second.txPackets - iter->second.rxPackets)*100/iter->second.txPackets << "%");
    NS_LOG_UNCOND("Packet Loss Ratio: " << (iter->second.lostPackets)*100/iter->second.txPackets << "%");
    NS_LOG_UNCOND("Delay: " << iter->second.delaySum);
    NS_LOG_UNCOND("Jitter: " << iter->second.jitterSum);
    NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 /(iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds())/1024 << " kbps");


    sentPackets += iter->second.txPackets;
    receivedPackets += iter->second.rxPackets;
    // lostPackets += (iter->second.txPackets - iter->second.rxPackets);
    lostPackets += (iter->second.lostPackets);
    avgThroughput += iter->second.rxBytes * 8.0 /(iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds())/1024;
    delay += iter->second.delaySum;
    jitter += iter->second.jitterSum;

    j++;
  }

  avgThroughput = avgThroughput/j;
  NS_LOG_UNCOND("\n--------------- Simulation Stats ------------"<<std::endl);
  NS_LOG_UNCOND("Total sent packets: " << sentPackets);
  NS_LOG_UNCOND("Total Received Packets: " << receivedPackets);
  NS_LOG_UNCOND("Total Lost Packets: " << lostPackets);
  NS_LOG_UNCOND("Packet Loss Ratio: " << lostPackets*100/sentPackets << "%");
  NS_LOG_UNCOND("Packet Delivery Ratio: " << receivedPackets * 100 /sentPackets << "%");
  NS_LOG_UNCOND("Average Throughput: " << avgThroughput << " kbps");
  NS_LOG_UNCOND("End to end delay: "<< delay);
  NS_LOG_UNCOND("End to end jitter delay: "<< jitter);
  NS_LOG_UNCOND("Total Flow ID: " << j);

  monitor->SerializeToXmlFile("test_flow.xml", true, true);


  Simulator::Destroy ();
  return 0;
}
