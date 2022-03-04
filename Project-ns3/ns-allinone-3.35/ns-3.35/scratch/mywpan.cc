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
#include "ns3/applications-module.h"
#include "ns3/ipv6-flow-classifier.h"
#include "ns3/flow-monitor-helper.h"
#include <ns3/lr-wpan-error-model.h>
using namespace ns3;

static Ptr<OutputStreamWrapper> cWndStream;
static Ptr<OutputStreamWrapper> ssThreshStream;
static bool firstCwnd = true;
static bool firstSshThr = true;
static uint32_t cWndValue;
static uint32_t ssThreshValue;

int tracedNode=1;

static void CwndTracer (uint32_t oldval, uint32_t newval) {
    if (firstCwnd) {
        *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
        firstCwnd = false;
    }
    *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
    cWndValue = newval;

    if (!firstSshThr) *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
}

static void SsThreshTracer (uint32_t oldval, uint32_t newval) {
    if (firstSshThr) {
        *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
        firstSshThr = false;
    }
    *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
    ssThreshValue = newval;

    if (!firstCwnd) *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
}

static void TraceCwnd (std::string cwnd_tr_file_name) {
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/"+std::to_string(tracedNode)+"/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}

static void TraceSsThresh (std::string ssthresh_tr_file_name) {
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/"+std::to_string(tracedNode)+"/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
}

bool tracing = false;
uint16_t nSourceNodes=1;
uint32_t nWsnNodes;
uint16_t sinkPort=9;
uint32_t mtu_bytes = 180;
uint32_t tcp_adu_size;
uint64_t data_mbytes = 0;
double start_time = 0;
double duration = 100.0;
double stop_time;
bool sack = true;
std::string recovery = "ns3::TcpClassicRecovery";
std::string congestionAlgo = "TcpNewReno";
std::string filePrefix;

Ptr<LrWpanErrorModel>  lrWpanError;

void initialize(int argc, char** argv) {
  CommandLine cmd (__FILE__);
  cmd.AddValue ("tracing", "turn on log components", tracing);
  cmd.AddValue ("nSourceNodes", "turn on log components", nSourceNodes);
  cmd.AddValue ("tracedNode", "turn on log components", tracedNode);
  cmd.Parse (argc, argv);

  if( tracing ) {
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
  }

  nWsnNodes = nSourceNodes + 1;
  tracedNode = std::max(1, tracedNode);
  filePrefix = "wpan-sourceCount" + std::to_string(nSourceNodes);

  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName (recovery)));

  congestionAlgo = "ns3::" + congestionAlgo;
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", 
                      TypeIdValue (TypeId::LookupByName (congestionAlgo)));

  // 2 MB of TCP buffer
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));

  Header* temp_header = new Ipv6Header ();
  uint32_t ip_header = temp_header->GetSerializedSize ();
  delete temp_header;
  temp_header = new TcpHeader ();
  uint32_t tcp_header = temp_header->GetSerializedSize ();
  delete temp_header;
  tcp_adu_size = mtu_bytes - 20 - (ip_header + tcp_header);

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));

  stop_time = start_time + duration;

  lrWpanError = CreateObject<LrWpanErrorModel> ();

  std::cout << "------------------------------------------------------\n"; 
  std::cout << "Source Count: " << nSourceNodes << "\n"; 
  std::cout << "------------------------------------------------------\n"; 
}

int main (int argc, char** argv) {
  initialize(argc, argv);

  Packet::EnablePrinting ();

  NodeContainer wsnNodes;
  wsnNodes.Create (nWsnNodes);

  NodeContainer wiredNodes;
  wiredNodes.Create (1);
  wiredNodes.Add (wsnNodes.Get (0));

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (80),
                                 "DeltaY", DoubleValue (80),
                                 "GridWidth", UintegerValue (10),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wsnNodes);

  LrWpanHelper lrWpanHelper;
  NetDeviceContainer lrwpanDevices = lrWpanHelper.Install (wsnNodes);

  lrWpanHelper.AssociateToPan (lrwpanDevices, 0);

  InternetStackHelper internetv6;
  internetv6.Install (wsnNodes);
  internetv6.Install (wiredNodes.Get (0));

  SixLowPanHelper sixLowPanHelper;
  NetDeviceContainer sixLowPanDevices = sixLowPanHelper.Install (lrwpanDevices);

  CsmaHelper csmaHelper;
  NetDeviceContainer csmaDevices = csmaHelper.Install (wiredNodes);

  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:cafe::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wiredDeviceInterfaces;
  wiredDeviceInterfaces = ipv6.Assign (csmaDevices);
  wiredDeviceInterfaces.SetForwarding (1, true);
  wiredDeviceInterfaces.SetDefaultRouteInAllNodes (1);

  ipv6.SetBase (Ipv6Address ("2001:f00d::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wsnDeviceInterfaces;
  wsnDeviceInterfaces = ipv6.Assign (sixLowPanDevices);
  wsnDeviceInterfaces.SetForwarding (0, true);
  wsnDeviceInterfaces.SetDefaultRouteInAllNodes (0);

  for (uint32_t i = 0; i < sixLowPanDevices.GetN (); i++) {
    Ptr<NetDevice> dev = sixLowPanDevices.Get (i);
    dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
    dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
  }

  for( uint32_t i=1; i<=nSourceNodes; i++ ) {
    BulkSendHelper sourceApp ("ns3::TcpSocketFactory",
                              Inet6SocketAddress (wiredDeviceInterfaces.GetAddress (0, 1), 
                              sinkPort));
    sourceApp.SetAttribute ("SendSize", UintegerValue (tcp_adu_size));
    sourceApp.SetAttribute ("MaxBytes", UintegerValue (data_mbytes * 1000000));
    ApplicationContainer sourceApps = sourceApp.Install (wsnNodes.Get (i));
    sourceApps.Start (Seconds (start_time));
    sourceApps.Stop (Seconds (stop_time));

    PacketSinkHelper sinkApp ("ns3::TcpSocketFactory",
    Inet6SocketAddress (Ipv6Address::GetAny (), sinkPort));
    sinkApp.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
    ApplicationContainer sinkApps = sinkApp.Install (wiredNodes.Get(0));
    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (Seconds (stop_time));

    sinkPort++;
  }

  if (tracing) {
    AsciiTraceHelper ascii;
    lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream (filePrefix + ".tr"));
    lrWpanHelper.EnablePcapAll (filePrefix, false);

    csmaHelper.EnableAsciiAll (ascii.CreateFileStream (filePrefix + ".tr"));
    csmaHelper.EnablePcapAll (filePrefix, false);

    Simulator::Schedule (Seconds (0.00001), &TraceCwnd, filePrefix + "-cwnd.data");
    Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, filePrefix + "-ssth.data");
  }

  FlowMonitorHelper flowHelper;
  flowHelper.InstallAll ();

  Simulator::Stop (Seconds (stop_time));
  Simulator::Run ();

  flowHelper.SerializeToXmlFile (filePrefix + ".flowmonitor", true, true);

  Simulator::Destroy ();

  return 0;
}

