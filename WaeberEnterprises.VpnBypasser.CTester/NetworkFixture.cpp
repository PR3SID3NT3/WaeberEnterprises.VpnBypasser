#include "stdafx.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

using namespace System;
using namespace System::Net;
using namespace System::Threading;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;

using namespace NUnit::Framework;

using namespace WaeberEnterprises::VpnBypasser::Network;

namespace WaeberEnterprises
{
	namespace VpnBypasser
	{
		namespace CTester
		{
			[TestFixture]
			public ref class NetworkFixture
			{
			public:
				[Test]
				void GetPidByPortAndProtocol_GibtKorrektePidZurueckWennSocketTcp() 
				{
					Socket ^ socket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
					socket->Bind(gcnew IPEndPoint(IPAddress::Parse("0.0.0.0"), 666));
					socket->Listen(10);

					NetstatReader ^ netstatReader = gcnew NetstatReader();
					int pid = netstatReader->GetPidByPortAndProtocol(ProtocolType::Tcp, 666);

					Assert::That(pid, Is::EqualTo(Process::GetCurrentProcess()->Id));
				}

				[Test]
				void GetPidByPortAndProtocol_GibtKorrektePidZurueckWennSocketUdp() 
				{
					Socket ^ socket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Dgram, ProtocolType::Udp);
					socket->Bind(gcnew IPEndPoint(IPAddress::Parse("0.0.0.0"), 666));
					
					array<Byte> ^ arr = gcnew array<Byte>(10);
					socket->BeginReceive(arr, 0, 10, SocketFlags::None, nullptr, nullptr);

					NetstatReader ^ netstatReader = gcnew NetstatReader();
					int pid = netstatReader->GetPidByPortAndProtocol(ProtocolType::Udp, 666);

					Assert::That(pid, Is::EqualTo(Process::GetCurrentProcess()->Id));
				}
			};
		}
	}
}