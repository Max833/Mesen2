﻿using ReactiveUI.Fody.Helpers;
using System.Collections.Generic;

namespace Mesen.Config
{
	public class EventViewerConfig : BaseWindowConfig<EventViewerConfig>
	{
		[Reactive] public bool ShowSettingsPanel { get; set; } = true;

		[Reactive] public double ImageScale { get; set; } = 1;

		[Reactive] public bool RefreshOnBreakPause { get; set; } = true;
		[Reactive] public bool AutoRefresh { get; set; } = true;
		[Reactive] public RefreshTimingConfig RefreshTiming { get; set; } = new();
		
		[Reactive] public List<int> ColumnWidths { get; set; } = new();

		[Reactive] public bool ShowToolbar { get; set; } = true;

		[Reactive] public SnesEventViewerConfig SnesConfig { get; set; } = new SnesEventViewerConfig();
		[Reactive] public NesEventViewerConfig NesConfig { get; set; } = new NesEventViewerConfig();
		[Reactive] public GbEventViewerConfig GbConfig { get; set; } = new GbEventViewerConfig();
		[Reactive] public PceEventViewerConfig PceConfig { get; set; } = new PceEventViewerConfig();
	}
}
