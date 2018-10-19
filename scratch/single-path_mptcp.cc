/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
*
*This is test code to resarching for mptcp throughput on ns3.
*Written by rosev.
*29/Nov./2016(Sometimes updates.)
*
*Network Topology.
*
             (TCP sink)
     n1 <----------------->n2
           5Mbps,20ms,0.01

*    192.168.0.1 -------->192.168.0.2
*
*Flow from n1 to n2 using TcpBulkSendApplication.
*/

#include <iostream>
#include <string>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/netanim-module.h"
#include "ns3/mp-tcp-bulk-send-helper.h"
#include "ns3/mp-tcp-packet-sink.h"
#include "ns3/log.h"
#include "ns3/buffer.h"

//#define SEED_NUMBER 20
//#define DATARATE1 "5Mbps"
//#define DELAY "20ms"
//#define PACKET_LOSS_1 0.01
//#define TIME 30
//#define SIM_TIME 30
#define PACKET_SIZE 512

#define send_buffersize 1*1024*1024
#define receive_buffersize 1*1024*1024

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MpTcpPacketSink-test");

int main(int argc, char *argv[]){

	LogComponentEnable("MpTcpSocketBase", LOG_INFO);
	LogComponentEnable("MpTcpPacketSink", LOG_INFO);
	
	//Variable Argumment Block

	string DATARATE1;
	string DELAY1;
	double PACKET_LOSS_1;
	double TIME = 30.0;
	double SIM_TIME = 30.0;
	unsigned int SEED_NUMBER;
	//unsigned int PACKET_SIZE;
	//unsigned int send_buffersize;
	//unsigned int receive_buffersize;
	
	CommandLine cmd;
	cmd.AddValue("DATARATE1","Set DataRate1",DATARATE1);
	cmd.AddValue("DELAY1","Set Delay1",DELAY1);
	cmd.AddValue("PACKET_LOSS_1","Set Packet loss rate1",PACKET_LOSS_1);
	//cmd.AddValue("TIME","Set time",TIME);
	//cmd.AddValue("SIM_TIME","Set SIM_TIME",SIM_TIME);
	cmd.AddValue("SEED_NUMBER","Set SEED Number",SEED_NUMBER);
	//cmd.AddValue("PACKET_SIZE","Set Packet size",PACKET_SIZE);
	//cmd.AddValue("send_buffersize","Set send buffer size",send_buffersize);
	//cmd.AddValue("receive_buffersize","Set receive buffer size",receive_buffersize); 
	cmd.Parse(argc,argv);

	//set randomness seed number

	SeedManager::SetSeed(SEED_NUMBER);
	SeedManager::SetRun (SEED_NUMBER);

	//default configrations

	Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
	Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(PACKET_SIZE));
	Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
	//Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
	//Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(queueSize));
	Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
	Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(8)); // Sink
	//Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("RTT_Compensator"));
	//Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));
	Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(send_buffersize));//Byte Unit
	Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(receive_buffersize));//Byte Unit

	//ns-3 topology block

	//Create 2 nodes.
	NodeContainer nodes;
	nodes.Create(2);//MPTCP node(Proxy servers)

	//Declear point to point
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue(DATARATE1));
	pointToPoint.SetChannelAttribute("Delay", StringValue(DELAY1));

	//Declear net devices
	NetDeviceContainer devices1;
	devices1 = pointToPoint.Install(nodes);	

	//Install stack
	InternetStackHelper stack;
	stack.InstallAll();

	//Set IPv4 Address
	Ipv4AddressHelper ipv4_1;
	ipv4_1.SetBase("192.168.0.0", "255.255.255.0","0.0.0.1");
	Ipv4InterfaceContainer i1 = ipv4_1.Assign(devices1);

	NS_LOG_INFO ("Initialize Global Routing.");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	uint16_t port = 80;

	AddressValue remoteAddress (InetSocketAddress (i1.GetAddress(1, 0), port));

	//Set MPTCP
	MpTcpBulkSendHelper source1("ns3::TcpSocketFactory", Address());
	source1.SetAttribute("Remote",remoteAddress);
	source1.SetAttribute("MaxBytes", UintegerValue(0));

	//Set application
	ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));
	sourceApps1.Start(Seconds(0.0));
	sourceApps1.Stop(Seconds(TIME));

	//set socket
	Address sinkAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
	MpTcpPacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkAddress);
	ApplicationContainer sinkApps = sinkHelper.Install(nodes.Get(1));
	sinkApps.Start(Seconds(0.0));
	sinkApps.Stop(Seconds(TIME));

	// set error model
	Ptr<RateErrorModel> em1 = CreateObject<RateErrorModel>();
	em1->SetAttribute("ErrorRate",DoubleValue(PACKET_LOSS_1));
	em1->SetAttribute("ErrorUnit",EnumValue(ns3::RateErrorModel::ERROR_UNIT_PACKET));
	em1->SetAttribute("RanVar",StringValue("ns3::UniformRandomVariable[Min=0.0,max=1.0]"));
	
	//install "Receive" errmodel
	devices1.Get(1)->SetAttribute("ReceiveErrorModel",PointerValue(em1));
	
	//Creating PCAP files for Wireshark
	pointToPoint.EnablePcapAll("SIM_DATA");
	
	//Creating NetAnim xml file
	//AnimationInterface anim ("SIM_DATA.xml");

	//Simulator implimentation
	NS_LOG_INFO ("Run Simulation.");
	Simulator::Stop(Seconds(SIM_TIME));
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO ("Done.");

	return 0;
}
