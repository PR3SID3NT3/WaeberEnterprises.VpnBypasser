using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

using WaeberEnterprises.VpnBypasser.Gui.ProcessView;

namespace WaeberEnterprises.VpnBypasser.Gui
{
	/// <summary>
	/// Logic for the MainWindow.
	/// </summary>
	public partial class MainWindow
	{
		/*
		 * 
		 *  JEDE IP DIE VON EINEM PROCESS KOMMT DER NICHT GEROUTET WERDEN SOLL, WIRD MANUELL DER ROUTING TABLE
		 *  HINZUGEFUEGT, DAS PROGRAMM MERKT SICH WELCHE IP'S ALS AUSNAHMEN VON WELCHER EXE GEHANDELT WERDEN.
		 *  SOBALD DIE EXE GESCHLOSSEN WIRD, WERDEN ALLE ROUTING-EINTRÄGE ENTFERNT. DIES ERFORDERT WINDIVERT/WINPKFILTER
		 *  
		 * 
		 *  WINDIVERT -> FTW! yay :)
		 * 
		 * 
		 * 
		 */

		private readonly ObservableCollection<ProcessListItem> _processList = new ObservableCollection<ProcessListItem>();

		public MainWindow()
		{
			InitializeComponent();
		}

		public ObservableCollection<ProcessListItem> ProcessList
		{
			get { return _processList; }
		}

		private void GridViewColumnHeaderClickedHandler(object sender, RoutedEventArgs e)
		{
			ListSortDirection direction;
			GridViewColumnHeader headerClicked = e.OriginalSource as GridViewColumnHeader;

			if (headerClicked != null)
			{
				_processList.Clear();
				foreach (Process process in Process.GetProcesses().OrderBy(p => p.Id))
				{
					_processList.Add(new ProcessListItem { Name = process.ProcessName, Pid = process.Id });
				}
			}
		}
	}
}
