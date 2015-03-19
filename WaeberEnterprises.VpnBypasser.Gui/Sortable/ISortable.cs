using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WaeberEnterprises.VpnBypasser.Gui.Sortable
{
	public interface ISortable
	{
		void SortAscending();

		void SortDescending(ISortable sort);
	}
}
