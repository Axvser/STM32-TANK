using System;
using System.Diagnostics;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using VeloxDev.Core.Mono;
using VeloxDev.Core.MVVM;

namespace Controller.ViewModels;

[MonoBehaviour(60)]
public partial class TankViewModel
{
    /// <summary>
    /// 以 60FPS 检测是否发生设备状态更新,若是则发送数据到 ESP-01S
    /// </summary>
    partial void Update()
    {
        if (!IsContextChanged) return;
        IsContextChanged = false;
        PostCommand.Execute(null);
    }

    /* Fields
     * 做一些必要的缓存以提升高频传输的性能
     */

    private const string EspIp = "192.168.43.132";
    private const int EspPort = 8080;
    private TcpClient? _tcpclient = new TcpClient();
    private readonly StringBuilder _msgbuilder = new StringBuilder();
    private readonly byte[] _sendBuffer = new byte[128];
    private NetworkStream? _networkStream;

    /* Properties
     * 有关设备状态的属性
     * 发送数据格式如下,保留两位小数
     * [leftTrack,rightTrack,turretH,turretV,firePower]
     */

    [VeloxProperty] private double _leftTrack = 0d;
    [VeloxProperty] private double _rightTrack = 0d;
    [VeloxProperty] private double _turretH = 0d;
    [VeloxProperty] private double _turretV = 0d;
    [VeloxProperty] private bool _fire = false;
    [VeloxProperty] private bool _isContextChanged = false;

    partial void OnLeftTrackChanged(double oldValue, double newValue)
    {
        IsContextChanged = true;
    }

    partial void OnRightTrackChanged(double oldValue, double newValue)
    {
        IsContextChanged = true;
    }

    partial void OnTurretHChanged(double oldValue, double newValue)
    {
        IsContextChanged = true;
    }

    partial void OnTurretVChanged(double oldValue, double newValue)
    {
        IsContextChanged = true;
    }

    partial void OnFireChanged(bool oldValue, bool newValue)
    {
        IsContextChanged = true;
    }

    /// <summary>
    /// 上报坦克当前状态给到 ESP-01S 模块，使用 TCP 协议
    /// </summary>
    /// <param name="parameter">无需填写</param>
    /// <param name="ct">取消令牌</param>
    [VeloxCommand]
    private async Task Post(object? parameter, CancellationToken ct)
    {
        try
        {
            // 1. 检查连接状态，必要时重连
            if (_tcpclient?.Connected != true)
            {
                await ReconnectAsync(ct).ConfigureAwait(false);
            }

            // 2. 构建消息正文（复用 StringBuilder）
            _msgbuilder.Clear();
            _msgbuilder.Append("CSharpST{")
                .Append(LeftTrack.ToString("0.00")).Append(',')
                .Append(RightTrack.ToString("0.00")).Append(',')
                .Append((15 + (TurretH * (35 - 15))).ToString("0.00")).Append(',')
                .Append((15 + (TurretV * (35 - 15))).ToString("0.00")).Append(',')
                .Append((Fire ? 0.1 : 0.01).ToString("0.00"))
                .Append("}CSharpED");

            // 3. 直接编码到预分配缓冲区
            var byteCount = Encoding.UTF8.GetBytes(
                _msgbuilder.ToString(),
                0, _msgbuilder.Length,
                _sendBuffer, 0);

            // 4. 发送数据（复用 NetworkStream）
            await ((_networkStream ??= _tcpclient?.GetStream())!)
                .WriteAsync(_sendBuffer.AsMemory(0, byteCount), ct)
                .ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Send failed: {ex.Message}");
            await TryReconnectAsync(ct).ConfigureAwait(false);
        }
    }

    private async Task ReconnectAsync(CancellationToken ct)
    {
        DisposeConnection();
        _tcpclient = new TcpClient();
        await _tcpclient.ConnectAsync(EspIp, EspPort, ct).ConfigureAwait(false);
        _networkStream = _tcpclient.GetStream();
    }

    private async Task TryReconnectAsync(CancellationToken ct)
    {
        try
        {
            await ReconnectAsync(ct).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Reconnect failed: {ex.Message}");
        }
    }

    private void DisposeConnection()
    {
        _networkStream?.Dispose();
        _tcpclient?.Dispose();
        _networkStream = null;
        _tcpclient = null;
    }
}