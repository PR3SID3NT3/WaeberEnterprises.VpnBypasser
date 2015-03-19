using System;

using WaeberEnterprises.VpnBypasser.Core.WinDivert;

namespace WaeberEnterprises.VpnBypasser.Core.Extensions
{
	/// <summary>
	/// Used to create filters in the windivert filter language.
	/// </summary>
	public static class FilterCreator
	{
		// TODO EXCEPTION HANDLING

		/// <summary>
		/// Used if and-operations are required.
		/// </summary>
		public static Filter And(this Filter filter)
		{
			filter.FilterString = TrimEndWhiteSpace(filter.FilterString);
			filter.FilterString += " and ";
			return filter;
		}

		/// <summary>
		/// Appends a specific string to the filter.
		/// </summary>
		/// <param name="filter">The filter to update.</param>
		/// <param name="stringToAppend">The string to append.</param>
		public static Filter AppendString(this Filter filter, string stringToAppend)
		{
			filter.FilterString += stringToAppend;
			return filter;
		}

		/// <summary>
		/// Overrides the existing filter with the defaults. Namely "tcp and udp".
		/// </summary>
		public static Filter MakeDefaultFilter(this Filter filter)
		{
			filter.FilterString = "tcp and udp";
			return filter;
		}

		/// <summary>
		/// Used if or-operations are required.
		/// </summary>
		public static Filter Or(this Filter filter)
		{
			filter.FilterString = TrimEndWhiteSpace(filter.FilterString);
			filter.FilterString += " or ";
			return filter;
		}

		private static string TrimEndWhiteSpace(string str)
		{
			return str.TrimEnd(new[] { ' ' });
		}
	}
}
