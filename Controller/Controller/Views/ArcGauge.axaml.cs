using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Controls.Shapes;
using Avalonia.Media;

namespace Controller.Views;

public class ArcGauge : TemplatedControl
{
    private Grid? _bdGrid;
    private readonly List<TextBlock> _textLabels = new();

    public ArcGauge()
    {
        SetCurrentValue(ValueProperty, 0d);
        SetCurrentValue(MinValueProperty, 0d);
        SetCurrentValue(MaxValueProperty, 100d);
    }

    protected override void OnApplyTemplate(TemplateAppliedEventArgs e)
    {
        base.OnApplyTemplate(e);

        _bdGrid = e.NameScope.Find<Grid>("PartBdGrid");
        if (_bdGrid == null) return;
        InitTick();
        Refresh();
    }

    private void InitTick()
    {
        if (_bdGrid == null) return;

        // Clear existing ticks and labels
        foreach (var child in _bdGrid.Children.OfType<Line>().ToList())
        {
            _bdGrid.Children.Remove(child);
        }

        // Draw major ticks
        for (var i = 0; i < 9; i++)
        {
            var line = new Line
            {
                StartPoint = new Point(0, 0),
                EndPoint = new Point(0, 12),
                HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Center,
                Stroke = Brushes.White,
                StrokeThickness = 2,
                RenderTransformOrigin = RelativePoint.Center,
                RenderTransform = new RotateTransform { Angle = -140 + i * 35 }
            };
            _bdGrid.Children.Add(line);
        }

        // Draw minor ticks
        for (var i = 0; i < 8; i++)
        {
            var start = -140 + 35 * i + 3.5;
            for (var j = 0; j < 9; j++)
            {
                var line = new Line
                {
                    StartPoint = new Point(0, 0),
                    EndPoint = new Point(0, 6),
                    Stroke = Brushes.White,
                    StrokeThickness = 1,
                    HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Center,
                    RenderTransformOrigin = RelativePoint.Center,
                    RenderTransform = new RotateTransform { Angle = start + j * 3.5 }
                };
                _bdGrid.Children.Add(line);
            }
        }

        DrawText();
    }

    private void DrawText()
    {
        if (_bdGrid == null) return;

        // 移除旧标签
        foreach (var label in _textLabels)
        {
            _bdGrid.Children.Remove(label);
        }

        _textLabels.Clear();

        // 计算每个标签的角度和值
        for (var i = 0; i < 9; i++)
        {
            // 计算当前刻度对应的值（线性插值）
            var value = MinValue + (MaxValue - MinValue) * (i / 8.0);
            var text = value.ToString("0.##"); // 格式化显示（保留2位小数）

            // 计算标签角度（和指针逻辑一致）
            var normalizedValue = value - MinValue;
            var process = normalizedValue / (MaxValue - MinValue);
            var angle = process * 280 - 140; // 范围 -140° 到 140°

            var textBlock = new TextBlock
            {
                Text = text,
                HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Center,
                RenderTransformOrigin = RelativePoint.Center, // 围绕自身中心旋转
                RenderTransform = new RotateTransform { Angle = angle },
                Margin = new Thickness(12),
                Foreground = Brushes.White
            };
            _bdGrid.Children.Add(textBlock);
            _textLabels.Add(textBlock);
        }
    }

    [Category("值设定")]
    public double Value
    {
        get => (double)GetValue(ValueProperty);
        set => SetValue(ValueProperty, value);
    }

    public static readonly StyledProperty<double> ValueProperty =
        AvaloniaProperty.Register<ArcGauge, double>(nameof(Value), coerce: OnValueChanged);

    [Category("值设定")]
    public double MinValue
    {
        get => (double)GetValue(MinValueProperty);
        set => SetValue(MinValueProperty, value);
    }

    public static readonly StyledProperty<double> MinValueProperty =
        AvaloniaProperty.Register<ArcGauge, double>(nameof(MinValue), coerce: OnValueChanged);

    [Category("值设定")]
    public double MaxValue
    {
        get => (double)GetValue(MaxValueProperty);
        set => SetValue(MaxValueProperty, value);
    }

    public static readonly StyledProperty<double> MaxValueProperty =
        AvaloniaProperty.Register<ArcGauge, double>(nameof(MaxValue), coerce: OnValueChanged);

    public double Angle
    {
        get => (double)GetValue(AngleProperty);
        set => SetValue(AngleProperty, value);
    }

    public static readonly StyledProperty<double> AngleProperty =
        AvaloniaProperty.Register<ArcGauge, double>(nameof(Angle));

    private static double OnValueChanged(AvaloniaObject @object, double value)
    {
        if (@object is ArcGauge gauge)
        {
            gauge.Refresh();
        }

        return value;
    }

    private void Refresh()
    {
        if (Value > MaxValue)
        {
            Angle = 140;
        }
        else if (Value < MinValue)
        {
            Angle = -140;
        }
        else
        {
            var range = MaxValue - MinValue;
            var normalizedValue = Value - MinValue;
            var process = normalizedValue / range;
            var tAngle = process * 280 - 140;
            Angle = tAngle;
        }
    }
}