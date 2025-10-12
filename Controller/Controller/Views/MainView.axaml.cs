using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using CefNet.Avalonia;
using Controller.Tools;
using Controller.ViewModels;
using VeloxDev.Core.Mono;

namespace Controller.Views;

[MonoBehaviour]
public partial class MainView : UserControl
{
    private static readonly bool Ope = OperatingSystem.IsWindows();
    private readonly MainViewModel _viewModel = new();
    private readonly WebView _webview = new() { Focusable = false, IsVisible = false };
    private readonly ScaleTransform _webscale = new ScaleTransform(1, 1);
    private readonly TranslateTransform _webtranslate = new TranslateTransform(0, 0);
    private readonly HashSet<Key> _keyPressed = [];
    private readonly HashSet<KeyModifiers> _keyModifierPressed = [];

    public MainView()
    {
        InitializeComponent();
        InitializeWebView(_viewModel.Tank.CameIp);
        DataContext = _viewModel;
        TcpView.DataContext = _viewModel.Tank;
        EdcView.DataContext = _viewModel.Tank.EDC;
        _viewModel.Tank.CanMonoBehaviour = true;
        _viewModel.Tank.EDC.SubscribeToKeyboardEvents(this);
        KeyDown += User_KeyDown;
        KeyUp += User_KeyUp;
        _viewModel.Tank.EDC.CanMonoBehaviour = true;
    }

    partial void Update()
    {
        CheckKeyCombinations();
    }

    private void InitializeWebView(string url)
    {
        if (!Ope) return;
        _webview.InitialUrl = url;
        Browser.Children.Add(_webview);
        _webview.RenderTransformOrigin = RelativePoint.TopLeft;
        _webview.RenderTransform = new TransformGroup() { Children = [_webtranslate, _webscale] };
        Loaded += async (s, e) =>
        {
            await OpenWebCamera();
            CanMonoBehaviour = true;
        };
    }

    private async Task OpenWebCamera()
    {
        _webview.IsVisible = false;
        await Task.Delay(1000);
        _webview.HideScrollbar();
        await Task.Delay(200);
        _webview.ClickElementById("toggle-stream");
        await Task.Delay(200);
        _webview.ClickElementById("nav-toggle");
        _webview.IsVisible = true;
    }

    private void User_KeyDown(object? sender, KeyEventArgs e)
    {
        // 记录按下的普通键
        _keyPressed.Add(e.Key);

        // 记录按下的修饰键
        if (e.KeyModifiers != KeyModifiers.None)
        {
            _keyModifierPressed.Add(e.KeyModifiers);
        }
    }

    private void User_KeyUp(object? sender, KeyEventArgs e)
    {
        // 移除释放的普通键
        _keyPressed.Remove(e.Key);

        // 移除释放的修饰键
        if (e.KeyModifiers != KeyModifiers.None)
        {
            _keyModifierPressed.Remove(e.KeyModifiers);
        }

        _viewModel.Tank.Fire = false;
    }

    private void CheckKeyCombinations()
    {
        var alt = _keyModifierPressed.Contains(KeyModifiers.Alt);

        // 缩放操作
        if (alt && _keyPressed.Contains(Key.OemPlus))
        {
            _webscale.ScaleX += 0.1;
            _webscale.ScaleY += 0.1;
        }

        if (alt && _keyPressed.Contains(Key.OemMinus))
        {
            _webscale.ScaleX -= 0.1;
            _webscale.ScaleY -= 0.1;
        }

        // 平移操作（支持斜向移动）
        int deltaX = 0, deltaY = 0;
        if (alt && _keyPressed.Contains(Key.Left)) deltaX -= 10;
        if (alt && _keyPressed.Contains(Key.Right)) deltaX += 10;
        if (alt && _keyPressed.Contains(Key.Up)) deltaY -= 10;
        if (alt && _keyPressed.Contains(Key.Down)) deltaY += 10;

        _webtranslate.X += deltaX;
        _webtranslate.Y += deltaY;

        var keyJ = _keyPressed.Contains(Key.J);
        var keyQ = _keyPressed.Contains(Key.Q);
        var keyE = _keyPressed.Contains(Key.E);
        var keyK = _keyPressed.Contains(Key.K);
        var keyL = _keyPressed.Contains(Key.L);

        // 开火检测
        _viewModel.Tank.Fire = keyJ;

        // 左右转检测
        _viewModel.Tank.TurretH = (keyQ, keyE) switch
        {
            (true, false) => 32.5,
            (false, true) => 20,
            _ => 25
        };

        // 上下抬检测
        _viewModel.Tank.TurretV += (keyK, keyL) switch
        {
            (true, false) => 0.1,
            (false, true) => -0.1,
            _ => 0
        };
    }
}