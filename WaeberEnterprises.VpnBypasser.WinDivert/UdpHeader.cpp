#include "stdafx.h"

namespace WaeberEnterprises
{
	namespace VpnBypasser 
	{
		namespace WinDivert
		{
			public ref class UdpHeader
			{
				public:
					UdpHeader(int packetLen)
					{
						rawPacket = gcnew array<System::Char>(packetLen);
					}

					int srcPort;
					int dstPort;
					array<System::Char> ^ rawPacket;
			};
		}
	}
}