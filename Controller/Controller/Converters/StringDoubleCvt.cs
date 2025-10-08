using System;
using System.Globalization;
using Avalonia;
using Avalonia.Data.Converters;

namespace Controller.Converters;

/// <summary>
/// 指明从 ViewModel 到 View 时，String 与 Double 的数据转换方向
/// </summary>
public enum StrDoubleDirection
{
    StrToDouble,
    DoubleToStr
}

public class StringDoubleCvt : AvaloniaObject, IValueConverter
{
    public static readonly StyledProperty<StrDoubleDirection> DirectionProperty = AvaloniaProperty.Register<StringDoubleCvt, StrDoubleDirection>(
        nameof(StrDoubleDirection));

    public StrDoubleDirection Direction
    {
        get => GetValue(DirectionProperty);
        set => SetValue(DirectionProperty, value);
    }

    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return Direction == StrDoubleDirection.StrToDouble
            ? ConvertStringToDouble(value)
            : ConvertDoubleToString(value);
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return Direction == StrDoubleDirection.StrToDouble
            ? ConvertDoubleToString(value)
            : ConvertStringToDouble(value);
    }

    private static object? ConvertDoubleToString(object? value)
    {
        const string failedStr = "?";
        return value is not double d ? failedStr : d.ToString("F3");
    }

    private static object? ConvertStringToDouble(object? value)
    {
        const double failednum = 0;
        if (value is not string s) return failednum;
        return double.TryParse(s, out var d) ? d : failednum;
    }
}