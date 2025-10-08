using System;
using System.Collections.Generic;
using Avalonia.Input;
using VeloxDev.Core.Mono;
using VeloxDev.Core.MVVM;
using VeloxDev.Core.WeakTypes;

namespace Controller.ViewModels;

/// <summary>
/// 接收来自 EDC 系统的更新帧
/// </summary>
public delegate void EDCFrameHandler(double leftVelocity, double rightVelocity);

/// <summary>
/// Electronic Differential Control (电子差速控制)
/// - 所有运动均为平滑渐变
/// - 小油门/大油门 (Shift) 行为不同 (幅值与响应时间)
/// - W/A/S/D 支持任意组合（互抵为 0）
/// - Ctrl 开启定点旋转（忽略 W/S），否则为差速（同向）转向
/// </summary>
[MonoBehaviour]
public partial class EDCViewModel
{
    public readonly WeakDelegate<EDCFrameHandler> FixedUpdate = new();

    /* 基础物理参数 */
    [VeloxProperty] private double _minVelocity = -1.0;
    [VeloxProperty] private double _maxVelocity = 1.0;

    /* 当前状态（实际输出） */
    [VeloxProperty] private double _leftVelocity;
    [VeloxProperty] private double _rightVelocity;

    /* 平滑与仿真参数（可在运行时调整） */
    [VeloxProperty] private double _velocitySmoothTime = 0.45; // 基础时间常数（秒），越大越慢
    [VeloxProperty] private double _accelSharpness = 1.3;      // 加速灵敏度 (>1 加速更快)
    [VeloxProperty] private double _brakeSharpness = 2.8;      // 制动 / 反向灵敏度 (>1 更快)
    [VeloxProperty] private double _ackermannTurnScale = 0.7;  // 差速转向幅度系数 (0..1)
    [VeloxProperty] private double _pointTurnSpeed = 0.6;      // 定点旋转基础速率 (0..1)

    /* 坦克几何参数 */
    [VeloxProperty] private double _wheelbase = 0.23;
    [VeloxProperty] private double _trackWidth = 0.24;

    /* 控制输入状态（键盘） */
    private readonly HashSet<Key> _activeKeys = [];
    // 逻辑输入（合成）
    private double _forwardInput; // [-1,1] = W(+) - S(-)
    private double _turnInput;    // [-1,1] = D(+) - A(-)

    /* 模式状态 */
    private enum ControlMode { Ackermann, PointTurn }
    private enum ThrottleMode { Normal, Sport }

    private ControlMode _currentMode = ControlMode.Ackermann;
    private ControlMode _targetMode = ControlMode.Ackermann;
    private ThrottleMode _throttleMode = ThrottleMode.Normal;

    private double _modeTransitionProgress = 1.0;

    // 订阅键盘
    public void SubscribeToKeyboardEvents(InputElement element)
    {
        element.KeyDown += OnKeyDown;
        element.KeyUp += OnKeyUp;
    }

    private void OnKeyDown(object? sender, KeyEventArgs e)
    {
        _activeKeys.Add(e.Key);
        UpdateControlInputs();
    }

    private void OnKeyUp(object? sender, KeyEventArgs e)
    {
        _activeKeys.Remove(e.Key);
        UpdateControlInputs();
    }

    /// <summary>
    /// 将原始按键状态合成为前进/转向控制量
    /// 支持任意组合（W+S -> 0，A+D -> 0），并决定油门/转向模式
    /// </summary>
    private void UpdateControlInputs()
    {
        var shiftPressed = _activeKeys.Contains(Key.LeftShift) || _activeKeys.Contains(Key.RightShift);
        var ctrlPressed = _activeKeys.Contains(Key.LeftCtrl) || _activeKeys.Contains(Key.RightCtrl);
        var altPressed = _activeKeys.Contains(Key.LeftAlt) || _activeKeys.Contains(Key.RightAlt);

        // Throttle mode by Shift
        _throttleMode = shiftPressed ? ThrottleMode.Sport : ThrottleMode.Normal;

        // 合成前进输入：W - S
        var forward = 0.0;
        if (_activeKeys.Contains(Key.W)) forward += 1.0;
        if (_activeKeys.Contains(Key.S)) forward -= 1.0;
        _forwardInput = Math.Clamp(forward, -1.0, 1.0);

        // 合成转向输入：D - A
        var turn = 0.0;
        if (_activeKeys.Contains(Key.D)) turn += 1.0;
        if (_activeKeys.Contains(Key.A)) turn -= 1.0;
        _turnInput = Math.Clamp(turn, -1.0, 1.0);

        // Ctrl 决定是否点转
        _targetMode = ctrlPressed ? ControlMode.PointTurn : ControlMode.Ackermann;

        // 空格急停（立即把目标输入置零，但实际速度仍由平滑器渐变至0）
        if (_activeKeys.Contains(Key.Space))
        {
            _forwardInput = 0.0;
            _turnInput = 0.0;
            _targetMode = ControlMode.Ackermann;
        }

        // Alt+R 重置
        if (altPressed && _activeKeys.Contains(Key.R))
            ResetTransform();
    }

    /// <summary>
    /// 主更新 - 60FPS 调用
    /// </summary>
    partial void Update()
    {
        UpdateModeTransition();

        // 1) 先计算目标轮速（尚未平滑）
        double targetLeft, targetRight;
        if (_currentMode == ControlMode.PointTurn)
            CalculatePointTurnTargets(out targetLeft, out targetRight);
        else
            CalculateAckermannTargets(out targetLeft, out targetRight);

        // 2) 平滑地从当前速度变到目标速度（指数平滑）
        ApplyDynamicSmoothTransition(ref targetLeft, ref targetRight);

        // 3) 防打滑/限差处理
        ApplyAntiSlipControl(ref targetLeft, ref targetRight);

        // 4) 更新实际输出
        _leftVelocity = targetLeft;
        _rightVelocity = targetRight;

        // 5) 广播帧回调
        FixedUpdate.Invoke([_leftVelocity, _rightVelocity]);
    }

    /// <summary>
    /// 平滑模式过渡（保留，虽然当前逻辑直接切换mode，但保留渐变进度可扩展）
    /// </summary>
    private void UpdateModeTransition()
    {
        const double transitionSpeed = 0.08;
        if (_currentMode != _targetMode)
        {
            _modeTransitionProgress -= transitionSpeed;
            if (_modeTransitionProgress <= 0)
            {
                _currentMode = _targetMode;
                _modeTransitionProgress = 0.0;
            }
        }
        else if (_modeTransitionProgress < 1.0)
        {
            _modeTransitionProgress = Math.Min(1.0, _modeTransitionProgress + transitionSpeed);
        }
    }

    /// <summary>
    /// 将前进输入映射为目标推进速度（受小/大油门限制与油门曲线影响）
    /// Normal (小油门): 最大绝对值 0.6，曲线更平缓
    /// Sport (大油门): 最大绝对值 1.0，曲线更激进
    /// </summary>
    private double MapForwardInputToTarget(double input)
    {
        // 限幅
        var limit = _throttleMode == ThrottleMode.Sport ? 1.0 : 0.6;

        // 曲线：Normal -> cubic (更细腻) ; Sport -> sqrt (更暴力)
        double mapped;
        if (_throttleMode == ThrottleMode.Normal)
            mapped = Math.Pow(Math.Abs(input), 3) * Math.Sign(input); // 更平滑的小输入
        else
            mapped = Math.Sign(input) * Math.Sqrt(Math.Abs(input));   // 更敏感

        return Math.Clamp(mapped * limit, -limit, limit);
    }

    /// <summary>
    /// Ackermann 差速（同向）转向目标生成
    /// - 当 forward 小（接近 0）时，不做定点旋转（点旋转由 Ctrl + A/D 触发）
    /// - turnInput 在 [-1,1], positive => 右转（左轮更快）
    /// </summary>
    private void CalculateAckermannTargets(out double targetLeft, out double targetRight)
    {
        // 基础推进速度（由 W/S 决定）
        var baseSpeed = MapForwardInputToTarget(_forwardInput);

        // 若前进接近 0 或转向输入接近 0，则直行或轻微转
        if (Math.Abs(baseSpeed) < 0.01 || Math.Abs(_turnInput) < 0.01)
        {
            targetLeft = baseSpeed;
            targetRight = baseSpeed;
            return;
        }

        // 基于 turnInput 生成内外轮比例（保持同向）
        var t = Math.Clamp(_turnInput, -1.0, 1.0);
        var absT = Math.Abs(t);

        // turnSign: +1 右转（左轮更快）， -1 左转（右轮更快）
        var turnSign = Math.Sign(t);

        // scale 控制差速幅度，不超过 ackermannTurnScale
        var scale = _ackermannTurnScale * absT; // 0..ackermannTurnScale

        // 如果 baseSpeed 正（前进），左轮为外轮 (右转时), 若后退则符号相反仍保持同向
        if (baseSpeed > 0)
        {
            // 右转 (t>0): left faster, right slower
            if (turnSign > 0)
            {
                targetLeft = baseSpeed * (1.0 + scale);
                targetRight = baseSpeed * (1.0 - scale);
            }
            else // 左转 (t<0)
            {
                targetLeft = baseSpeed * (1.0 - scale);
                targetRight = baseSpeed * (1.0 + scale);
            }
        }
        else // baseSpeed < 0: 后退时同样的分配逻辑（两轮方向相同）
        {
            if (turnSign > 0)
            {
                targetLeft = baseSpeed * (1.0 + scale);
                targetRight = baseSpeed * (1.0 - scale);
            }
            else
            {
                targetLeft = baseSpeed * (1.0 - scale);
                targetRight = baseSpeed * (1.0 + scale);
            }
        }

        // 最终限幅
        targetLeft = Math.Clamp(targetLeft, _minVelocity, _maxVelocity);
        targetRight = Math.Clamp(targetRight, _minVelocity, _maxVelocity);
    }

    /// <summary>
    /// 定点旋转目标生成（忽略 W/S，左右反向）
    /// - 旋转速度与油门模式有关（小/大油门）
    /// - turnInput 决定旋转方向与强度
    /// </summary>
    private void CalculatePointTurnTargets(out double targetLeft, out double targetRight)
    {
        var t = Math.Clamp(_turnInput, -1.0, 1.0);
        var absT = Math.Abs(t);
        if (absT < 0.01)
        {
            // 没有转向输入时，保持速度向 0 渐变（通过平滑器实现）
            targetLeft = 0.0;
            targetRight = 0.0;
            return;
        }

        // rotation base scales with throttle mode limit
        var throttleLimit = _throttleMode == ThrottleMode.Sport ? 1.0 : 0.6;
        var baseRotation = _pointTurnSpeed * throttleLimit;

        // 强度按 absT 缩放（线性）
        var vel = baseRotation * absT;

        // 左正右负 为顺时针（取决你之前定义，这里以 turn>0 为右转 -> 左正 右负）
        var sign = Math.Sign(t);
        targetLeft = vel * sign;
        targetRight = -vel * sign;

        // 限幅
        targetLeft = Math.Clamp(targetLeft, _minVelocity, _maxVelocity);
        targetRight = Math.Clamp(targetRight, _minVelocity, _maxVelocity);
    }

    /// <summary>
    /// 平滑过渡（动态响应：加速/减速/反向敏捷度不同）
    /// 使用指数平滑： new = current + (target - current) * factor
    /// factor = 1 - exp(-dt / timeConstant)
    /// timeConstant 会根据油门模式、加速或制动/反向而变化
    /// </summary>
    private void ApplyDynamicSmoothTransition(ref double targetLeft, ref double targetRight)
    {
        const double deltaTime = 1.0 / 60.0;

        double DynamicLerp(double current, double target)
        {
            var delta = target - current;
            // 判断是否反向（当前与目标符号相反并且当前速度有一定幅值）
            var isReversing = Math.Sign(target) != Math.Sign(current) && Math.Abs(current) > 0.05;

            // 基础 time 常数
            var time = _velocitySmoothTime;

            // 小油门（Normal）更慢：乘以 1.2；大油门（Sport）更快：乘以 0.7
            if (_throttleMode == ThrottleMode.Normal) time *= 1.2;
            else time *= 0.7;

            // 根据当前行为调整时间常数
            if (isReversing)
            {
                // 制动/反向要更快响应（短时间常数）
                time /= Math.Max(1.0, _brakeSharpness);
            }
            else if (Math.Abs(target) > Math.Abs(current))
            {
                // 加速：更快
                time /= Math.Max(1.0, _accelSharpness);
            }
            else
            {
                // 减速但非反向：略慢
                time *= 1.0;
            }

            // 防止 time 非法或过小
            time = Math.Max(0.01, time);

            var factor = 1.0 - Math.Exp(-deltaTime / time);
            return current + delta * factor;
        }

        _leftVelocity = DynamicLerp(_leftVelocity, targetLeft);
        _rightVelocity = DynamicLerp(_rightVelocity, targetRight);

        // 将平滑后的值返回为“目标”（即下一步广播使用）
        targetLeft = Math.Clamp(_leftVelocity, _minVelocity, _maxVelocity);
        targetRight = Math.Clamp(_rightVelocity, _minVelocity, _maxVelocity);
    }

    /// <summary>
    /// 防打滑 / 差速限幅
    /// 限制两侧速度差不超过某一阈值，避免物理上不合理的分配
    /// </summary>
    private void ApplyAntiSlipControl(ref double targetLeft, ref double targetRight)
    {
        const double maxDiff = 0.9; // 允许较大差速（仿真可调）
        var diff = Math.Abs(targetLeft - targetRight);
        if (diff <= maxDiff) return;

        var ratio = maxDiff / diff;
        var avg = (targetLeft + targetRight) / 2.0;
        targetLeft = avg + (targetLeft - avg) * ratio;
        targetRight = avg + (targetRight - avg) * ratio;
    }

    private void EmergencyStop()
    {
        // 立刻将逻辑输入置零，速度仍由平滑器渐变到 0
        _forwardInput = 0.0;
        _turnInput = 0.0;
        _targetMode = ControlMode.Ackermann;
    }

    private void ResetTransform() => EmergencyStop();

    /// <summary>
    /// 可外部设置的定点旋转基准速度（0.1..1.0）
    /// </summary>
    public void SetPointTurnSpeed(double speed) =>
        _pointTurnSpeed = Math.Clamp(speed, 0.1, 1.0);

    /// <summary>
    /// 调试信息
    /// </summary>
    public (string mode, string throttle, double leftVel, double rightVel, double fwdInput, double turnInput) GetDebugInfo() =>
        (_currentMode.ToString(), _throttleMode.ToString(), _leftVelocity, _rightVelocity, _forwardInput, _turnInput);
}
