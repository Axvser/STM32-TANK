using System;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using CefNet.Avalonia;
using Controller.ViewModels;

namespace Controller.Views;

public partial class MainView : UserControl
{
    private readonly MainViewModel? _mainView = new();
    
    public MainView()
    {
        InitializeComponent();
        DataContext = _mainView;
        WebView webview = new() { Focusable = true };
        webview.InitialUrl = "192.168.43.69";
        Browser.Children.Add(webview);
    }

    private void Fire_On(object? sender, PointerPressedEventArgs e)
    {
        if (DataContext is MainViewModel { Tank: { } tv })
        {
            tv.Fire = true;
        }
    }

    private void Fire_Off(object? sender, PointerReleasedEventArgs e)
    {
        if (DataContext is MainViewModel { Tank: { } tv })
        {
            tv.Fire = false;
        }
    }

    private void Button_OnClick(object? sender, RoutedEventArgs e)
    {
        if (DataContext is MainViewModel { Tank: { } tv })
        {
            tv.Fire = !tv.Fire;
        }
    }

    private void User_KeyDowm(object? sender, KeyEventArgs e)
    {
        
    }

    private void User_KeyUp(object? sender, KeyEventArgs e)
    {
        
    }
}