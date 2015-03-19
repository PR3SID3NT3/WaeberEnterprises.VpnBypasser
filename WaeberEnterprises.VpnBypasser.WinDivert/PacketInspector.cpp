// This is the main DLL file.

#include "stdafx.h"
#include "divert.h"
#include "TcpHeader.h"
#include "UdpHeader.h"
#include <stdint.h>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Runtime::InteropServices;

using namespace WaeberEnterprises::VpnBypasser::Network;
using namespace WaeberEnterprises::VpnBypasser::Routing;
using namespace WaeberEnterprises::VpnBypasser::Core::WinDivert;
using namespace WaeberEnterprises::VpnBypasser::Core::Extensions;

namespace WaeberEnterprises
{
	namespace VpnBypasser
	{
		namespace WinDivert
		{
			public ref class PacketInspector
			{
			private:
				// WinDivert handle
				HANDLE divertHandle;

				// We dont want to initialize this objects for every packet
				NetstatReader ^ _netstatReader;
				RoutingManager ^ _routingManager;

				int GetApplicationByPort(USHORT port, ProtocolType protocolType) {
					return _netstatReader->GetPidByPortAndProtocol(port, protocolType);
				}
			public:
				PacketInspector() {
					_netstatReader = gcnew NetstatReader();
					_routingManager = gcnew RoutingManager();
				}

				void Start(Filter ^ initialFilter) {

					Filter ^ filter = initialFilter;
					if (filter == nullptr) {
						filter = FilterCreator::MakeDefaultFilter(gcnew Filter());
					}

					// Length of payload
					int payloadLength;

					// TCP/UDP headers
					PDIVERT_TCPHDR tcpHeader;
					PDIVERT_UDPHDR udpHeader;

					// IP headers
					PDIVERT_IPHDR ipv4Header;
					PDIVERT_IPV6HDR ipv6Header;

					// ICMP headers
					PDIVERT_ICMPHDR icmpHeader;
					PDIVERT_ICMPV6HDR icmpv6Header;

					if (divertHandle == NULL) {
						divertHandle = DivertOpen((char *)Marshal::StringToHGlobalAnsi(filter->FilterString).ToPointer(), (DIVERT_LAYER)0, 404, DIVERT_FLAG_DROP); // TODO Filter Priority
						if (divertHandle == INVALID_HANDLE_VALUE)
						{
							throw gcnew ApplicationException(String::Format("Error: {0}->Start-->INVALID_HANDLE_VALUE", this->GetType()));
						}
					}

					UINT packetLen;
					DIVERT_ADDRESS addr;
					char rawPacket[0xFFFF];

					// make native pointers
					pin_ptr<int> pinnedPayloadLength = &payloadLength;

					pin_ptr<PDIVERT_TCPHDR> pinnedTcpHeader = &tcpHeader;
					pin_ptr<PDIVERT_UDPHDR> pinnedUdpHeader = &udpHeader;

					pin_ptr<PDIVERT_IPHDR> pinnedIpv4Header = &ipv4Header;
					pin_ptr<PDIVERT_IPV6HDR> pinnedIpv6Header = &ipv6Header;
				
					pin_ptr<PDIVERT_ICMPHDR> pinnedIcmpHeader = &icmpHeader;
					pin_ptr<PDIVERT_ICMPV6HDR> pinnedIcmpV6Header = &icmpv6Header;

					while(true)
					{
						if (!DivertRecv(divertHandle, rawPacket, sizeof(rawPacket), &addr, &packetLen))
						{
							// TODO Log Error
							continue;
						}

						// only outgoing packets need to be checked
						if (addr.Direction == DIVERT_DIRECTION_INBOUND) 
						{
							continue;
						}

						// Parse Packet
						DivertHelperParsePacket(rawPacket, packetLen, pinnedIpv4Header, pinnedIpv6Header, pinnedIcmpHeader, pinnedIcmpV6Header, pinnedTcpHeader, pinnedUdpHeader, NULL, (UINT*)pinnedPayloadLength);

						// Icmp packets are not handled
						if (icmpHeader != nullptr || icmpv6Header != nullptr)
						{
							continue;
						}

						// Only ipv4 packets are handled at the moment
						if(ipv4Header == nullptr)
						{
							continue;
						}

						// Only tcp and udp packets are handled					
						if (tcpHeader == nullptr && udpHeader == nullptr)
						{
							continue;
						}

						if (tcpHeader != nullptr) 
						{
							DWORD pid = this->GetApplicationByPort(tcpHeader->SrcPort, ProtocolType::Tcp);
							if (pid == 0) {
								continue;
							}

							// TODO BLOCK PACKETS FOR ATLEAST 1 SECOND UNTIL THE ROUTE ENTRY HAS BEEN ADDED
						}
						
						if (udpHeader != nullptr) 
						{
							DWORD pid = this->GetApplicationByPort(udpHeader->SrcPort, ProtocolType::Tcp);
							if (pid == 0) {
								continue;
							}

							// TODO BLOCK PACKETS FOR ATLEAST 1 SECOND UNTIL THE ROUTE ENTRY HAS BEEN ADDED
							// 
						}

						// TODO REINJECT PACKET
					}
				}

				//void Start(Filter ^ initialFilter) {

				//	Filter ^ filter = initialFilter;
				//	if (filter == nullptr) {
				//		filter = FilterCreator::MakeDefaultFilter(gcnew Filter());
				//	}

				//	// Length of payload
				//	int payloadLength;

				//	// TCP/UDP headers
				//	PDIVERT_TCPHDR tcpHeader;
				//	PDIVERT_UDPHDR udpHeader;

				//	// IP headers
				//	PDIVERT_IPHDR ipv4Header;
				//	PDIVERT_IPV6HDR ipv6Header;

				//	// ICMP headers
				//	PDIVERT_ICMPHDR icmpHeader;
				//	PDIVERT_ICMPV6HDR icmpv6Header;

				//	if (divertHandle == NULL) {
				//		divertHandle = DivertOpen((char *)Marshal::StringToHGlobalAnsi(filter->FilterString).ToPointer(), (DIVERT_LAYER)0, 404, DIVERT_FLAG_SNIFF); // Todo Filter Priority
				//		if (divertHandle == INVALID_HANDLE_VALUE)
				//		{
				//			throw gcnew ApplicationException(String::Format("Error: {0}->Start-->INVALID_HANDLE_VALUE", this->GetType()));
				//		}
				//	}

				//	UINT packetLen;
				//	DIVERT_ADDRESS addr;
				//	char rawPacket[0xFFFF];

				//	// make native pointers
				//	pin_ptr<int> pinnedPayloadLength = &payloadLength;

				//	pin_ptr<PDIVERT_TCPHDR> pinnedTcpHeader = &tcpHeader;
				//	pin_ptr<PDIVERT_UDPHDR> pinnedUdpHeader = &udpHeader;

				//	pin_ptr<PDIVERT_IPHDR> pinnedIpv4Header = &ipv4Header;
				//	pin_ptr<PDIVERT_IPV6HDR> pinnedIpv6Header = &ipv6Header;
				//
				//	pin_ptr<PDIVERT_ICMPHDR> pinnedIcmpHeader = &icmpHeader;
				//	pin_ptr<PDIVERT_ICMPV6HDR> pinnedIcmpV6Header = &icmpv6Header;

				//	while(true)
				//	{
				//		if (!DivertRecv(divertHandle, rawPacket, sizeof(rawPacket), &addr, &packetLen))
				//		{
				//			// Todo Log Error
				//			continue;
				//		}

				//		// Parse Packet
				//		DivertHelperParsePacket(rawPacket, packetLen, pinnedIpv4Header, pinnedIpv6Header, pinnedIcmpHeader, pinnedIcmpV6Header, pinnedTcpHeader, pinnedUdpHeader, NULL, (UINT*)pinnedPayloadLength);

				//		// icmp packets are not handled
				//		if (icmpHeader != nullptr || icmpv6Header != nullptr)
				//		{
				//			continue;
				//		}

				//		// only ip packets are handled
				//		if(ipv4Header == nullptr && ipv6Header == nullptr)
				//		{
				//			continue;
				//		}

				//		// only tcp or udp packets are handled					
				//		if (tcpHeader == nullptr && udpHeader == nullptr)
				//		{
				//			continue;
				//		}
				//	
				//		TcpHeader ^ managedTcpHeader;
				//		if (tcpHeader != nullptr)
				//		{
				//			managedTcpHeader = gcnew TcpHeader(packetLen);

				//			Marshal::Copy(IntPtr((char *)&rawPacket), managedTcpHeader->rawPacket, 0, packetLen);

				//			managedTcpHeader->srcPort = tcpHeader->SrcPort;
				//			managedTcpHeader->dstPort = tcpHeader->DstPort;
				//		} 
				//		else
				//		{
				//			managedTcpHeader = nullptr;	
				//		}

				//		UdpHeader ^ managedUdpHeader;
				//		if (udpHeader != nullptr)
				//		{
				//			managedUdpHeader = gcnew UdpHeader(packetLen);

				//			// Copy packet data
				//			Marshal::Copy(IntPtr((byte *)&rawPacket), managedUdpHeader->rawPacket, 0, packetLen);

				//			managedUdpHeader->srcPort = udpHeader->SrcPort;
				//			managedUdpHeader->dstPort = udpHeader->DstPort;
				//		} 
				//		else
				//		{
				//			managedUdpHeader = nullptr;	
				//		}

				//		// Set packet event (return value is used to syncronize data)
				//		bool retVal = OnPacketArriveEvent(managedTcpHeader, managedUdpHeader);
				//	}
				//}

				//void UpdateFilter(String ^ winDivertFilter) {

				//}
			};
		}
	}
}