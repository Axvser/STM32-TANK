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
    [VeloxProperty] private EDCViewModel _eDC = new();

    public TankViewModel()
    {
        EDC.FixedUpdate.AddHandler((vl, vr) =>
        {
            LeftTrack = vl;
            RightTrack = vr;
        });
    }

    /// <summary>
    /// 以 60FPS 检测是否发生设备状态更新,若是则发送数据到 ESP-01S
    /// </summary>
    partial void Update()
    {
        _connectCounter++;
        if (!IsContextChanged && _connectCounter < 20) return;
        IsContextChanged = false;
        _connectCounter = 0;
        IsConnected = _tcpclient?.Connected == true;
        PostCommand.Execute(null);
    }

    /* Net Fields
     * 做一些必要的缓存以提升高频TCP传输的性能与可靠性
     */

    private int _connectCounter = 0; // 最多20个空闲帧后就必须向硬件发送一次报文，这能避免设备失联后导致的失控行为
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
    [VeloxProperty] private double _turretH = 25; // 25 静止，20向右，32.5向左 ( 360舵机 )
    [VeloxProperty] private double _turretV = 27.5; // 27.5 水平，低于前倾，高于后倾 ( 180舵机 )
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
        _turretH = Math.Clamp(newValue, 20, 32.5);
        IsContextChanged = true;
    }

    partial void OnTurretVChanged(double oldValue, double newValue)
    {
        _turretV = Math.Clamp(newValue, 15, 35);
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
            await EnsureConnectedAsync(ct);

            _msgbuilder.Clear();
            _msgbuilder.Append("CSharpST{")
                .Append(LeftTrack.ToString("0.00")).Append(',')
                .Append(RightTrack.ToString("0.00")).Append(',')
                .Append(TurretH.ToString("0.00")).Append(',')
                .Append(TurretV.ToString("0.00")).Append(',')
                .Append((Fire ? 0.1 : 0.01).ToString("0.00"))
                .Append("}CSharpED");
            
            Console.WriteLine(_msgbuilder.ToString());

            var count = Encoding.UTF8.GetBytes(_msgbuilder.ToString(), 0, _msgbuilder.Length, _sendBuffer, 0);
            await _networkStream!.WriteAsync(_sendBuffer.AsMemory(0, count), ct);

            // 可在UI线程通知状态
            Avalonia.Threading.Dispatcher.UIThread.Post(() => IsConnected = true);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Send failed: {ex.Message}");
            Avalonia.Threading.Dispatcher.UIThread.Post(() => IsConnected = false);
        }
    }
    
    private async Task EnsureConnectedAsync(CancellationToken ct)
    {
        if (_tcpclient is { Connected: true } && _networkStream is { CanWrite: true })
            return;

        try
        {
            _tcpclient?.Close(); // 不 Dispose，避免 ESP 残留 CLOSE_WAIT
            _tcpclient = new TcpClient { NoDelay = true };
            await _tcpclient.ConnectAsync(EspIp, EspPort, ct);
            _networkStream = _tcpclient.GetStream();
            Debug.WriteLine("Connected to ESP-01S successfully");
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Reconnect failed: {ex.Message}");
        }
    }
}