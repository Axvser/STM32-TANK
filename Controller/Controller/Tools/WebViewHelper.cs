using CefNet.Avalonia;

namespace Controller.Tools;

public static class WebViewHelper
{
    /// <summary>
    /// 点击元素
    /// </summary>
    /// <param name="webview">网页视图</param>
    /// <param name="elementId">元素ID</param>
    public static void ClickElementById(this WebView webview, string elementId)
    {
        if (!(webview.GetMainFrame()?.IsValid ?? false)) return;
        var javascriptCode = $"document.getElementById('{elementId}').click();";
        webview.GetMainFrame().ExecuteJavaScript(javascriptCode, null, 0);
    }
    
    /// <summary>
    /// 隐藏滚动条
    /// </summary>
    /// <param name="webview">网页视图</param>
    public static void HideScrollbar(this WebView webview)
    {
        if (!(webview.GetMainFrame()?.IsValid ?? false)) return;
        const string javascriptCode = """

                                                  (function() {
                                                      var style = document.createElement('style');
                                                      style.type = 'text/css';
                                                      style.innerHTML = `
                                                          /* 针对Chrome/Safari等WebKit内核浏览器 */
                                                          body::-webkit-scrollbar {
                                                              display: none !important;
                                                          }
                                                          /* 确保内容仍然可以滚动 */
                                                          body {
                                                              -ms-overflow-style: none;  /* IE and Edge */
                                                              scrollbar-width: none;     /* Firefox */
                                                              overflow: auto !important;
                                                          }
                                                      `;
                                                      document.head.appendChild(style);
                                                  })();
                                              
                                      """;
        webview.GetMainFrame().ExecuteJavaScript(javascriptCode, null, 0);
    }
}