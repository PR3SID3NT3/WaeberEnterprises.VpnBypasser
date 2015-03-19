using System;
using System.Collections.Generic;
using System.Linq;

namespace WaeberEnterprises.VpnBypasser.Core
{
	/// <summary>
	/// Stores all informations about which application is currently bypassing the vpn connection.
	/// </summary>
	public static class BypassApplicationInformationHolder
	{
		private static readonly Dictionary<int, string> _processPidNameList = new Dictionary<int, string>();

		/// <summary>
		/// Adds an application to the internal list.
		/// </summary>
		public static void AddBypassingApplication(int pid, string processName)
		{
			if (!ProcessBypassing(pid))
			{
				_processPidNameList.Add(pid, processName);
			}
		}

		/// <summary>
		/// True if the specific pid is already bypassing the vpn connection.
		/// </summary>
		public static bool ProcessBypassing(int pid)
		{
			return _processPidNameList.Any(i => i.Key == pid);
		}

		/// <summary>
		/// Removes an application from the internal list.
		/// </summary>
		public static void RemoveBypassingApplication(int pid, string processName)
		{
			if (ProcessBypassing(pid))
			{
				_processPidNameList.Remove(pid);
			}
		}
	}
}
