#include "stdafx.h"

#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace System;
using namespace System::Net;
using namespace System::Runtime::InteropServices;
using namespace WaeberEnterprises::VpnBypasser::Core::Domain;

namespace WaeberEnterprises
{
	namespace VpnBypasser 
	{
		namespace Routing
		{
			public ref class RoutingManager
			{
			private:
				/// <summary>
				/// Returns the systems routing table.
				/// </summary>
				PMIB_IPFORWARDTABLE GetRoutingTable() {

					DWORD dwSize = 0;
					DWORD dwRetVal = 0;
					DWORD size = sizeof(MIB_IPFORWARDTABLE);
					PMIB_IPFORWARDTABLE pIpForwardTable = (PMIB_IPFORWARDTABLE) malloc(sizeof(MIB_IPFORWARDTABLE));

					// Get IpForwardTable
					GetIpForwardTable(pIpForwardTable, &dwSize, 0);
					pIpForwardTable = (MIB_IPFORWARDTABLE *) malloc(dwSize);
					if (pIpForwardTable == NULL || (dwRetVal = GetIpForwardTable(pIpForwardTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)) {
						throw gcnew System::InsufficientMemoryException("Error allocating memory for GetIpForwardTable();");
					}

					return pIpForwardTable;
				}

				// ERROR 5010: The object already exists

				/// <summary>
				/// Helper function for creating a MIB_IPFORWARDROW.
				/// </summary>
				PMIB_IPFORWARDROW CreateIpForwardRow(RouteEntry ^ routeEntryIdentifier) {
					
					PMIB_IPFORWARDROW ipForwardRow = (PMIB_IPFORWARDROW)malloc(sizeof(PMIB_IPFORWARDROW));

					//ipForwardRow->dwForwardMask = IPAddress::HostToNetworkOrder(System::BitConverter::ToInt32(routeEntryIdentifier->TargetSubnetMask->GetAddressBytes(), 0));

					ipForwardRow->dwForwardMask = inet_addr(static_cast<char*>(Marshal::StringToHGlobalAnsi(routeEntryIdentifier->TargetSubnetMask->ToString()).ToPointer()));
					ipForwardRow->dwForwardDest = inet_addr(static_cast<char*>(Marshal::StringToHGlobalAnsi(routeEntryIdentifier->TargetNetworkAddress->ToString()).ToPointer()));
					ipForwardRow->dwForwardNextHop = inet_addr(static_cast<char*>(Marshal::StringToHGlobalAnsi(routeEntryIdentifier->TargetGatewayAddress->ToString()).ToPointer()));
					
					// TODO
					ipForwardRow->dwForwardMetric1 = 10;

					// TODO
					ipForwardRow->dwForwardIfIndex = 3;

					// The dwForwardProto member must be set to MIB_IPPROTO_NETMGMT otherwise CreateIpForwardEntry will fail.
					ipForwardRow->dwForwardProto = MIB_IPPROTO_NETMGMT;

					return ipForwardRow;
				}
			public:
				/// <summary>
				/// Adds an entry to the systems routing table.
				/// </summary>
				DWORD AddEntry(RouteEntry ^ routeEntryIdentifier) {
					return CreateIpForwardEntry(CreateIpForwardRow(routeEntryIdentifier));
				}

				/// <summary>
				/// Removes an entry in the systems routing table.
				/// </summary>
				DWORD RemoveEntry(RouteEntry ^ routeEntryIdentifier) {
					return DeleteIpForwardEntry(CreateIpForwardRow(routeEntryIdentifier));
				}

				/// <summary>
				/// Checks if the given entry exists.
				/// </summary>
				bool Exists(RouteEntry ^ routeEntryIdentifier) {
					PMIB_IPFORWARDTABLE pIpForwardTable = this->GetRoutingTable();
					PMIB_IPFORWARDROW convertedRow = CreateIpForwardRow(routeEntryIdentifier);

					int i;
					for (i = 0; i < (int) pIpForwardTable->dwNumEntries; i++) {
						MIB_IPFORWARDROW entry = pIpForwardTable->table[i];

						bool valid = true;
						valid &= entry.dwForwardMask == convertedRow->dwForwardMask;
						valid &= entry.dwForwardDest == convertedRow->dwForwardDest;
						valid &= entry.dwForwardNextHop == convertedRow->dwForwardNextHop;

						if (valid) {
							return true;
						}
					}

					return false;
				}
				
				/// <summary>
				/// TO REMOVE.
				/// </summary>
				void PrintIpTable()
				{
					PMIB_IPFORWARDTABLE pIpForwardTable = this->GetRoutingTable();

					int i;
					struct in_addr IpAddr;
					char szDestIp[128];
					char szMaskIp[128];
					char szGatewayIp[128];
					int entries = pIpForwardTable->dwNumEntries;

					printf("\tNumber of entries: %d\n", (int) pIpForwardTable->dwNumEntries);
					for (i = 0; i < (int) pIpForwardTable->dwNumEntries; i++) {
						/* Convert IPv4 addresses to strings */
						IpAddr.S_un.S_addr =
							(u_long) pIpForwardTable->table[i].dwForwardDest;
						strcpy_s(szDestIp, sizeof (szDestIp), inet_ntoa(IpAddr));
						IpAddr.S_un.S_addr =
							(u_long) pIpForwardTable->table[i].dwForwardMask;
						strcpy_s(szMaskIp, sizeof (szMaskIp), inet_ntoa(IpAddr));
						IpAddr.S_un.S_addr =
							(u_long) pIpForwardTable->table[i].dwForwardNextHop;
						strcpy_s(szGatewayIp, sizeof (szGatewayIp), inet_ntoa(IpAddr));

						printf("\n\tRoute[%d] Dest IP: %s\n", i, szDestIp);
						printf("\tRoute[%d] Subnet Mask: %s\n", i, szMaskIp);
						printf("\tRoute[%d] Next Hop: %s\n", i, szGatewayIp);
						printf("\tRoute[%d] If Index: %ld\n", i,
								pIpForwardTable->table[i].dwForwardIfIndex);
						printf("\tRoute[%d] Type: %ld - ", i,
								pIpForwardTable->table[i].dwForwardType);
						switch (pIpForwardTable->table[i].dwForwardType) {
						case MIB_IPROUTE_TYPE_OTHER:
							printf("other\n");
							break;
						case MIB_IPROUTE_TYPE_INVALID:
							printf("invalid route\n");
							break;
						case MIB_IPROUTE_TYPE_DIRECT:
							printf("local route where next hop is final destination\n");
							break;
						case MIB_IPROUTE_TYPE_INDIRECT:
							printf
								("remote route where next hop is not final destination\n");
							break;
						default:
							printf("UNKNOWN Type value\n");
							break;
						}
						printf("\tRoute[%d] Proto: %ld - ", i,
								pIpForwardTable->table[i].dwForwardProto);
						switch (pIpForwardTable->table[i].dwForwardProto) {
						case MIB_IPPROTO_OTHER:
							printf("other\n");
							break;
						case MIB_IPPROTO_LOCAL:
							printf("local interface\n");
							break;
						case MIB_IPPROTO_NETMGMT:
							printf("static route set through network management \n");
							break;
						case MIB_IPPROTO_ICMP:
							printf("result of ICMP redirect\n");
							break;
						case MIB_IPPROTO_EGP:
							printf("Exterior Gateway Protocol (EGP)\n");
							break;
						case MIB_IPPROTO_GGP:
							printf("Gateway-to-Gateway Protocol (GGP)\n");
							break;
						case MIB_IPPROTO_HELLO:
							printf("Hello protocol\n");
							break;
						case MIB_IPPROTO_RIP:
							printf("Routing Information Protocol (RIP)\n");
							break;
						case MIB_IPPROTO_IS_IS:
							printf
								("Intermediate System-to-Intermediate System (IS-IS) protocol\n");
							break;
						case MIB_IPPROTO_ES_IS:
							printf("End System-to-Intermediate System (ES-IS) protocol\n");
							break;
						case MIB_IPPROTO_CISCO:
							printf("Cisco Interior Gateway Routing Protocol (IGRP)\n");
							break;
						case MIB_IPPROTO_BBN:
							printf("BBN Internet Gateway Protocol (IGP) using SPF\n");
							break;
						case MIB_IPPROTO_OSPF:
							printf("Open Shortest Path First (OSPF) protocol\n");
							break;
						case MIB_IPPROTO_BGP:
							printf("Border Gateway Protocol (BGP)\n");
							break;
						case MIB_IPPROTO_NT_AUTOSTATIC:
							printf("special Windows auto static route\n");
							break;
						case MIB_IPPROTO_NT_STATIC:
							printf("special Windows static route\n");
							break;
						case MIB_IPPROTO_NT_STATIC_NON_DOD:
							printf
								("special Windows static route not based on Internet standards\n");
							break;
						default:
							printf("UNKNOWN Proto value\n");
							break;
						}

						printf("\tRoute[%d] Age: %ld\n", i,
								pIpForwardTable->table[i].dwForwardAge);
						printf("\tRoute[%d] Metric1: %ld\n", i,
								pIpForwardTable->table[i].dwForwardMetric1);
					}
				}
			};
		}
	}
}