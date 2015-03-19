using System;
using System.Net;

namespace WaeberEnterprises.VpnBypasser.Core.Domain
{
	/// <summary>
	/// Stores the information for identifying a route in the routing table.
	/// </summary>
	public class RouteEntry
	{
		/// <summary>
		/// The physical interface to use for the route.
		/// </summary>
		public int InterfaceIndex { get; set; }

		/// <summary>
		/// The primary gateway where all packets are routed to.
		/// </summary>
		public IPAddress TargetGatewayAddress { get; set; }

		/// <summary>
		/// The network address to identify the target packets.
		/// </summary>
		public IPAddress TargetNetworkAddress { get; set; }

		/// <summary>
		/// The subnet mask of the target network.
		/// </summary>
		public IPAddress TargetSubnetMask { get; set; }
	}
}
