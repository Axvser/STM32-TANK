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
        _connectCounter++;
        if (!IsContextChanged && _connectCounter < 30) return;
        IsContextChanged = false;
        _connectCounter = 0;
        IsConnected = _tcpclient?.Connected == true;
        PostCommand.Execute(null);
    }

    /* Net Fields
     * 做一些必要的缓存以提升高频TCP传输的性能与可靠性
     */

    private int _connectCounter = 0; // 最多40个空闲帧后就必须向硬件发送一次报文，这能避免设备失联后导致的失控行为
    private TcpClient? _tcpclient = new TcpClient();
    private readonly StringBuilder _msgbuilder = new StringBuilder();
    private readonly byte[] _sendBuffer = new byte[128];
    private NetworkStream? _networkStream;

    /* Net Properties
     * 有关网络连接的属性
     */

    [VeloxProperty] private bool _isConnected = false;
    [VeloxProperty] private string _espIp = "192.168.43.132";
    [VeloxProperty] private int _espPort = 8080;
    [VeloxProperty] private string _cameIp = "192.168.43.69";

    /* Device Properties
     * 有关设备状态的属性
     * 发送数据格式如下,保留两位小数
     * [leftTrack,rightTrack,turretH,turretV,firePower]
     */

    [VeloxProperty] private double _leftTrack = 0d;
    [VeloxProperty] private double _rightTrack = 0d;
    [VeloxProperty] private double _turretH = 0.5;
    [VeloxProperty] private double _turretV = 0.25;
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
    /// <param name="parameter">外部传参</param>
    /// <param name="ct">取消令牌</param>
    [VeloxCommand]
    private async Task Post(object? parameter, CancellationToken ct)
    {
        try
        {
            // 1. 构建消息正文（复用 StringBuilder）
            _msgbuilder.Clear();
            _msgbuilder.Append("CSharpST{")
                .Append(LeftTrack.ToString("F2")).Append(',')
                .Append(RightTrack.ToString("F2")).Append(',')
                .Append((15 + (TurretH * (35 - 15))).ToString("F2"))
                .Append(',') // 25 静止，20向右，32.5向左 ( 360舵机，瞬时切换即可 )
                .Append((25 + (TurretV * (35 - 25))).ToString("F2"))
                .Append(',') // 27.5 水平，低于前倾，高于后倾 ( 非360舵机，建议逐渐变换 )
                .Append((Fire ? 0.1 : 0.01).ToString("F2"))
                .Append("}CSharpED");

            // 2. 直接编码到预分配缓冲区
            var byteCount = Encoding.UTF8.GetBytes(
                _msgbuilder.ToString(),
                0, _msgbuilder.Length,
                _sendBuffer, 0);

            // 3. 发送数据（复用 NetworkStream）
            await ((_networkStream ??= _tcpclient?.GetStream())!)
                .WriteAsync(_sendBuffer.AsMemory(0, byteCount), ct)
                .ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Send failed: {ex.Message}");
        }
    }

    /// <summary>
    /// 连接Tcp服务器
    /// </summary>
    /// <param name="parameter">外部传参</param>
    /// <param name="ct">取消令牌</param>
    [VeloxCommand]
    private async Task ConnectTcp(object? parameter, CancellationToken ct)
    {
        try
        {
            IsConnected = false;
            await ConnectTcpCommand.InterruptAsync();
            _tcpclient = new TcpClient();
            await _tcpclient.ConnectAsync(EspIp, EspPort, ct).ConfigureAwait(false);
            _networkStream = _tcpclient.GetStream();
            IsConnected = true;
        }
        catch (Exception e)
        {
            Debug.WriteLine(e);
            IsConnected = false;
        }
    }
}