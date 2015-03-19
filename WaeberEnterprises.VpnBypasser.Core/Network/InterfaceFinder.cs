using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;

namespace WaeberEnterprises.VpnBypasser.Core.Network
{
	public class InterfaceFinder
	{
		public void bla()
		{
			foreach(var iface in NetworkInterface.GetAllNetworkInterfaces().Where(i => i.OperationalStatus == OperationalStatus.Up))
			{
				IPInterfaceProperties ipProperties = iface.GetIPProperties();
				if (ipProperties != null)
				{
					var ipv4Properties = ipProperties.GetIPv4Properties();
					if (ipv4Properties != null)
					{}
				}
			}
		}
	}
}
