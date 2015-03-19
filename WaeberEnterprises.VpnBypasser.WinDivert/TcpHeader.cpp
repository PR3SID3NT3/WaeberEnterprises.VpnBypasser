#include "stdafx.h"

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
					{
						rawPacket = gcnew array<System::Byte>(packetLen);
					}

					int srcPort;
					int dstPort;
					array<System::Byte> ^ rawPacket;
			};
		}
	}
}