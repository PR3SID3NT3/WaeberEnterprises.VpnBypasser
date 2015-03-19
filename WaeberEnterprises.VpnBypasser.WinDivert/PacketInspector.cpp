// This is the main DLL file.

#include "stdafx.h"
#include "divert.h"
#include "TcpHeader.h"
#include "UdpHeader.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace WaeberEnterprises
{
	namespace VpnBypasser
	{
		namespace WinDivert
		{
			public delegate bool OnPacketArrivedEventHandler(TcpHeader ^, UdpHeader ^);

			public ref class PacketInspector
			{
			private:
				
				// WinDivert handle
				HANDLE divertHandle;

			public:
				// Public event for packet arrived
				event OnPacketArrivedEventHandler ^ OnPacketArriveEvent;

				~PacketInspector()
				{}

				void Start(String ^ winDivertFilter) {

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
						divertHandle = DivertOpen((char *)Marshal::StringToHGlobalAnsi("true").ToPointer(), (DIVERT_LAYER)0, 404, DIVERT_FLAG_SNIFF); // Todo Filter Priority
						if (divertHandle == INVALID_HANDLE_VALUE)
						{
							throw gcnew ApplicationException("Error: INVALID_HANDLE_VALUE");
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
							// Todo Log Error
							continue;
						}

						// Parse Packet
						DivertHelperParsePacket(rawPacket, packetLen, pinnedIpv4Header, pinnedIpv6Header, pinnedIcmpHeader, pinnedIcmpV6Header, pinnedTcpHeader, pinnedUdpHeader, NULL, (UINT*)pinnedPayloadLength);

						// icmp packets are not handled
						if (icmpHeader != nullptr || icmpv6Header != nullptr)
						{
							continue;
						}

						// only ip packets are handled
						if(ipv4Header == nullptr && ipv6Header == nullptr)
						{
							continue;
						}

						// only tcp or udp packets are handled					
						if (tcpHeader == nullptr && udpHeader == nullptr)
						{
							continue;
						}
					
						TcpHeader ^ managedTcpHeader;
						if (tcpHeader != nullptr)
						{
							managedTcpHeader = gcnew TcpHeader(packetLen);

							Marshal::Copy(IntPtr((char *)&rawPacket), managedTcpHeader->rawPacket, 0, packetLen);

							managedTcpHeader->srcPort = tcpHeader->SrcPort;
							managedTcpHeader->dstPort = tcpHeader->DstPort;
						} 
						else
						{
							managedTcpHeader = nullptr;	
						}

						UdpHeader ^ managedUdpHeader;
						if (udpHeader != nullptr)
						{
							managedUdpHeader = gcnew UdpHeader(packetLen);

							// Copy packet data
							Marshal::Copy(IntPtr((byte *)&rawPacket), managedUdpHeader->rawPacket, 0, packetLen);

							managedUdpHeader->srcPort = udpHeader->SrcPort;
							managedUdpHeader->dstPort = udpHeader->DstPort;
						} 
						else
						{
							managedUdpHeader = nullptr;	
						}

						// Set packet event (return value is used to syncronize data)
						bool retVal = OnPacketArriveEvent(managedTcpHeader, managedUdpHeader);
					}
				}

				void UpdateFilter(String ^ winDivertFilter) {

				}
			};
		}
	}
}