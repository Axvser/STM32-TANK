using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Controller.ViewModels;

namespace Controller.Views;

public partial class MainView : UserControl
{
    public MainView()
    {
        InitializeComponent();
        if (DataContext is MainViewModel { Tank: { } tv })
        {
            tv.CanMonoBehaviour = true;
        }
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
}