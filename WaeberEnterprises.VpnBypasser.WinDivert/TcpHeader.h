using namespace System;

namespace WaeberEnterprises
{
	namespace VpnBypasser 
	{
		namespace WinDivert
		{
			public ref class TcpHeader
			{
				public:
					TcpHeader(int packetLen)
					{}

					int srcPort;
					int dstPort;
					array<System::Byte> ^ rawPacket;
			};
		}
	}
}
