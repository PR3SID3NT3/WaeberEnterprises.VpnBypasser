using namespace System;

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
					{}

					int srcPort;
					int dstPort;
					array<System::Char> ^ rawPacket;
			};
		}
	}
}