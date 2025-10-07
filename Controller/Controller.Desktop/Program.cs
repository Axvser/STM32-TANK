using System;
using System.IO;
using Avalonia;
using Avalonia.Controls;
using CefNet;

namespace Controller.Desktop;

sealed class Program
{
    // Initialization code. Don't use any Avalonia, third-party APIs or any
    // SynchronizationContext-reliant code before AppMain is called: things aren't initialized
    // yet and stuff might break.
    [STAThread]
    public static void Main(string[] args)
    {
        if (Design.IsDesignMode == false)
        {
            var settings = new CefSettings();
            settings.MultiThreadedMessageLoop = true;
            settings.ResourcesDirPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "cef", "Resources");
            settings.LocalesDirPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "cef", "Resources", "locales");

            var app = new CefNetApplication();
          
            app.Initialize(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "cef", "Release"), settings);

        }
        BuildAvaloniaApp()
            .StartWithClassicDesktopLifetime(args);
    }

    // Avalonia configuration, don't remove; also used by visual designer.
    public static AppBuilder BuildAvaloniaApp()
        => AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .WithInterFont()
            .LogToTrace();
}