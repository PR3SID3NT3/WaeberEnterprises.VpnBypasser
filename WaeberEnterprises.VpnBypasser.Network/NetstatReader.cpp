#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdint.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace System;
using namespace System::Net::Sockets;

namespace WaeberEnterprises
{
	namespace VpnBypasser
	{
		namespace Network
		{
			public ref class NetstatReader
			{
			private:
				PMIB_TCPTABLE_OWNER_PID ReadExtendedTcpTable() {
					
					DWORD dwRetVal;
					DWORD dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);
					PMIB_TCPTABLE_OWNER_PID pTcpPidOwnerTable = (PMIB_TCPTABLE_OWNER_PID) malloc(sizeof(MIB_TCPTABLE_OWNER_PID));

					if ((dwRetVal = GetExtendedTcpTable(pTcpPidOwnerTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == ERROR_INSUFFICIENT_BUFFER) {
						free(pTcpPidOwnerTable);
						pTcpPidOwnerTable = (PMIB_TCPTABLE_OWNER_PID) malloc(dwSize);
						GetExtendedTcpTable(pTcpPidOwnerTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
						if (pTcpPidOwnerTable == NULL) {
							throw gcnew InsufficientMemoryException("Error allocating memory for GetExtendedTcpTable();");
						}
					}

					return pTcpPidOwnerTable;
				}

				PMIB_UDPTABLE_OWNER_PID ReadExtendedUdpTable() {

					DWORD dwRetVal;
					DWORD dwSize = sizeof(MIB_UDPTABLE_OWNER_PID);
					PMIB_UDPTABLE_OWNER_PID pUdpPidOwnerTable = (PMIB_UDPTABLE_OWNER_PID) malloc(sizeof(MIB_UDPTABLE_OWNER_PID));

					if ((dwRetVal = GetExtendedUdpTable(pUdpPidOwnerTable, &dwSize, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0)) == ERROR_INSUFFICIENT_BUFFER) {
						free(pUdpPidOwnerTable);
						pUdpPidOwnerTable = (PMIB_UDPTABLE_OWNER_PID) malloc(dwSize);
						GetExtendedUdpTable(pUdpPidOwnerTable, &dwSize, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0);
						if (pUdpPidOwnerTable == NULL) {
							throw gcnew InsufficientMemoryException("Error allocating memory for GetExtendedUdpTable();");
						}
					}

					return pUdpPidOwnerTable;
				}
			public:
				int GetPidByPortAndProtocol(ProtocolType protocol, int port){
					if (protocol == ProtocolType::Tcp) {
						u_int i = 0;
						PMIB_TCPTABLE_OWNER_PID pTcpPidOwnerTable = this->ReadExtendedTcpTable();
							
						for (; i < pTcpPidOwnerTable->dwNumEntries; i++) {
							MIB_TCPROW_OWNER_PID entry = pTcpPidOwnerTable->table[i];
							if (ntohs(entry.dwLocalPort) == port) {
								return entry.dwOwningPid;
							}
						}
					} else if (protocol == ProtocolType::Udp) {
						u_int i = 0;
						PMIB_UDPTABLE_OWNER_PID pUdpPidOwnerTable = this->ReadExtendedUdpTable();
														
						for (i; i < pUdpPidOwnerTable->dwNumEntries; i++) {
							MIB_UDPROW_OWNER_PID entry = pUdpPidOwnerTable->table[i];
							if (ntohs(entry.dwLocalPort) == port) {
								return entry.dwOwningPid;
							}
						}
					} else {
						throw gcnew NotSupportedException(String::Format("Protocol: {0} is not supported!", protocol));
					}

					return 0;
				}
			};
		}
	}
}

