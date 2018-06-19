REM --------------------
REM Get the last commit from master
REM --------------------

Echo Pull git master
REM git pull

REM --------------------
REM Set build dir, check if previous build
REM --------------------

for /f "tokens=1-4 delims=/ " %%G in ('date /t') do set mmddyyyy=%%G-%%H-%%I & echo mmddyyyy
Set builddir="%~dp0\Package\S-%mmddyyyy%"
Echo build path set to %builddir%

Echo create the package directory if there are none
if exist ".\Package" echo "Package folder already exists"
if not exist ".\Package" mkdir Package

Echo create a directory for this day if there are none else warning and exit
@if exist %builddir% echo ------------------------- & echo. Build already exists with today's date %mmddyyyy% (remove it manually) & echo. -------------------------
REM if exist %builddir% pause & exit
if not exist %builddir% mkdir %builddir%

REM --------------------
REM Build game
REM --------------------

Set uproject=%~dp0\Drift.uproject
Set UATdir="C:\Program Files\Epic Games\UE_4.13\Engine\Build\BatchFiles

%UATdir%\RunUAT.bat" BuildCookRun -project="%uproject%" -cook -stage -archive -archivedirectory=%builddir% -package -clientconfig=Shipping -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output

REM --------------------
REM Move Files to builddir instead of WindowsNoEditor
REM --------------------

pause