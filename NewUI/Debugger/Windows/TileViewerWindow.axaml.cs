#pragma warning disable CS8618 // Non-nullable field must contain a non-null value when exiting constructor. Consider declaring as nullable.

using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media.Imaging;
using Avalonia.Threading;
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Mesen.Debugger.Controls;
using Mesen.Debugger.ViewModels;
using Avalonia.Platform;
using Mesen.Interop;
using System.ComponentModel;

namespace Mesen.Debugger.Windows
{
	public class TileViewerWindow : Window
	{
		private NotificationListener _listener;
		private TileViewerViewModel _model;
		private PictureViewer _picViewer;
		private WriteableBitmap _viewerBitmap;

		public TileViewerWindow()
		{
			InitializeComponent();
#if DEBUG
            this.AttachDevTools();
#endif
		}

		private void InitializeComponent()
		{
			AvaloniaXamlLoader.Load(this);
		}

		protected override void OnOpened(EventArgs e)
		{
			base.OnOpened(e);

			if(Design.IsDesignMode) {
				return;
			}

			//Renderer.DrawFps = true;
			_picViewer = this.FindControl<PictureViewer>("picViewer");
			_picViewer.Source = _viewerBitmap;
			InitBitmap();

			_listener = new NotificationListener();
			_listener.OnNotification += listener_OnNotification;
		}
		
		protected override void OnClosing(CancelEventArgs e)
		{
			_listener?.Dispose();
			base.OnClosing(e);
		}

		private void InitBitmap()
		{
			_viewerBitmap = new WriteableBitmap(new PixelSize(_model.ColumnCount * 8, _model.RowCount * 8), new Vector(96, 96), PixelFormat.Bgra8888, AlphaFormat.Premul);
		}

		protected override void OnDataContextChanged(EventArgs e)
		{
			if(this.DataContext is TileViewerViewModel model) {
				_model = model;
			} else {
				throw new Exception("Unexception model");
			}
		}

		private void listener_OnNotification(NotificationEventArgs e)
		{
			if(e.NotificationType != ConsoleNotificationType.PpuFrameDone) {
				return;
			}

			_model.UpdatePaletteColors();

			byte[] source = DebugApi.GetMemoryState(_model.MemoryType);

			if(_viewerBitmap.PixelSize.Width != _model.ColumnCount * 8 || _viewerBitmap.PixelSize.Height != _model.RowCount * 8) {
				InitBitmap();
			}

			using(var framebuffer = _viewerBitmap.Lock()) {
				DebugApi.GetTileView(new GetTileViewOptions() {
					Format = _model.TileFormat,
					Width = _model.ColumnCount,
					Height = _model.RowCount,
					Palette = _model.SelectedPalette,
					Layout = _model.TileLayout,
					StartAddress = _model.StartAddress,
					Background = _model.TileBackground
				}, source, source.Length, _model.PaletteColors, framebuffer.Address);
			}

			Dispatcher.UIThread.Post(() => {
				_picViewer.Source = _viewerBitmap;
				_picViewer.InvalidateVisual();
			});
		}
	}
}