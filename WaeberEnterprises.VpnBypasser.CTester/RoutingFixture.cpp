#include "stdafx.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

using namespace System;
using namespace System::Net;
using namespace System::Runtime::InteropServices;

using namespace WaeberEnterprises::VpnBypasser::Routing;
using namespace WaeberEnterprises::VpnBypasser::Core::Domain;

using namespace NUnit::Framework;

namespace WaeberEnterprises
{
	namespace VpnBypasser
	{
		namespace CTester
		{
			[TestFixture]
			public ref class RoutingFixture
			{
			public:
				[Test]
				void AddEntry_FuegtEntryDerWindowsRoutingTableHinzu() 
				{
					RouteEntry ^ routeEntryToAdd = gcnew RouteEntry();
					routeEntryToAdd->TargetNetworkAddress = gcnew IPAddress(IPAddress::Parse("8.8.8.8")->GetAddressBytes());
					routeEntryToAdd->TargetSubnetMask = gcnew IPAddress(IPAddress::Parse("255.255.255.255")->GetAddressBytes());
					routeEntryToAdd->TargetGatewayAddress = gcnew IPAddress(IPAddress::Parse("192.168.10.1")->GetAddressBytes());

					RoutingManager ^ routingManager = gcnew RoutingManager();

					// Add the entry
					DWORD retVal = routingManager->AddEntry(routeEntryToAdd);
					Assert::That(retVal, Is::EqualTo(0));

					// Verify the entry exists
					Assert::That(routingManager->Exists(routeEntryToAdd), Is::True);

					// Delete the entry
					retVal = routingManager->RemoveEntry(routeEntryToAdd);
					Assert::That(retVal, Is::EqualTo(0));

					// Entry shouldn't exist any more
					Assert::That(routingManager->Exists(routeEntryToAdd), Is::False);
				}
			};
		}
	}
}