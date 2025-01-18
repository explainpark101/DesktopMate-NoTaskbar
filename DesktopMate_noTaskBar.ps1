Add-Type @"
using System;
using System.Runtime.InteropServices;

public class Win32 {
    [DllImport("user32.dll")]
    public static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

    [DllImport("user32.dll")]
    public static extern int GetWindowLong(IntPtr hWnd, int nIndex);

    [DllImport("user32.dll")]
    public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

    [DllImport("user32.dll")]
    public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
}
"@ -Language CSharp

$GWL_EXSTYLE = -20
$WS_EX_TOOLWINDOW = 0x00000080
$WS_EX_APPWINDOW = 0x00040000
$SW_HIDE = 0
$SW_SHOW = 5
$desktopMatePath = "C:\Program Files (x86)\Steam\steamapps\common\Desktop Mate\DesktopMate.exe"  # DesktopMate 경로
if (Get-Process -Name "DesktopMate" -ErrorAction SilentlyContinue) {
    Stop-Process -Name "DesktopMate"
}
Start-Process -FilePath $desktopMatePath

$styleChangedCount = 0

while ($styleChangedCount -lt 1) {
    # 프로세스의 창 핸들 가져오기
    $desktopMate = Get-Process -Name "DesktopMate" -ErrorAction SilentlyContinue
    if ($desktopMate) {
        $hWnd = $desktopMate.MainWindowHandle
        if ($hWnd -ne [IntPtr]::Zero) {
            # 창의 스타일을 변경하여 작업표시줄에서 숨기기
            $currentStyle = [Win32]::GetWindowLong($hWnd, $GWL_EXSTYLE)
            if ($currentStyle -eq 256) {
                if ($desktopMate.MainWindowHandle -eq (Get-Process -Name "DesktopMate" -ErrorAction SilentlyContinue).MainWindowHandle) {
                    $newStyle = $currentStyle -bor $WS_EX_TOOLWINDOW -band (-bnot $WS_EX_APPWINDOW)
                    [Win32]::SetWindowLong($hWnd, $GWL_EXSTYLE, $newStyle)
                    [Win32]::ShowWindow($hWnd, $SW_HIDE)  # 창 숨기기

                    $styleChangedCount = $styleChangedCount + 1
                }
            }
        }
    }
    Start-Sleep -Seconds 1
}