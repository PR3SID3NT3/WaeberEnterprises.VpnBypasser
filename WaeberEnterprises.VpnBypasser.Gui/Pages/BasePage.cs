using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace WaeberEnterprises.VpnBypasser.Gui.Pages
{
	/// <summary>
	/// Dis mami isch gay.
	/// </summary>
	public class BasePage
	{
		/// <summary>
		/// Sorts a list of string.
		/// </summary>
		protected IList<string> Sort(IList<string> sortable, ListSortDirection sortDirection)
		{
			return sortDirection == ListSortDirection.Ascending ? sortable.OrderBy(s => s).ToList() : sortable.OrderByDescending(s => s).ToList();
		}
	}
}
