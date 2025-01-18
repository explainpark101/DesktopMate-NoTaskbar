# DesktopMate 작업표시줄에서 숨기기
ps1 파일을 실행시키면 Steam에서 설치한 DesktopMate가 자동으로 실행되고, 작업표시줄에서 사라집니다.

캐릭터를 우클릭하고 Quit을 누르면 DesktopMate를 종료할 수 있으므로 더러운 작업표시줄을 정리하는데 도움이 됩니다.

## exe파일로 빌드
exe 파일은 powershell로 실행할 ps1 파일을 쉽게 실행시킬 수 있도록 변형한 파일입니다. 아래의 코드를 통해서 직접 빌드하실 수 있습니다.

폴더에서 Shift + 우클릭하고 `터미널에서 열기` 혹은 `여기에 powershell 창 실행`을 눌러 powershell을 실행해주시고 아래의 명령어를 입력해주세요.
```powershell
Invoke-PS2EXE .\DesktopMate_noTaskBar.ps1 DesktopMate-NoTaskbar.exe
```

### 컴퓨터 켜지면 자동으로 켜지도록 하기
아래의 코드를 통해 빌드하시면, 컴퓨터가 켜지면 자동으로 작업표시줄에서 지워진 상태로 DesktopMate가 켜지도록할 수 있습니다.
```powershell
Invoke-PS2EXE .\DesktopMate_noTaskBar.ps1 "$HOME\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\DesktopMate-NoTaskbar.exe"
```
