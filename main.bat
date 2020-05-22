@echo off

REM detect paths
set msbuild_cmd=msbuild.exe
where /q msbuild.exe
if not %errorlevel%==0 set msbuild_cmd="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"


:begin
	cls
	
	echo NEON86
	echo =======================
	echo    1. Exit
	echo    2. Build
	echo    3. Debug
	echo    4. Deploy (itch.io)
	echo    5. Open in VS
	echo    6. Open in VS Code
	echo    7. Open in lite
	echo    8. Pull upstream
	echo    9. itch.io build log
	echo =======================
	choice /C 123456789 /N /M "Your choice:"
	echo.
	
	if %errorlevel%==1 goto :EOF
	if %errorlevel%==2 call :build
	if %errorlevel%==3 call :debug
	if %errorlevel%==4 call :deploy
	if %errorlevel%==5 call :open_in_vs
	if %errorlevel%==6 call :open_in_vscode
	if %errorlevel%==7 call :open_in_lite
	if %errorlevel%==8 call :git_pull
	if %errorlevel%==9 call :butler_builds
goto :begin

:build
	%msbuild_cmd% neon86.sln /p:Configuration=Debug
	pause
exit /B 0

:build_release
	%msbuild_cmd% neon86.sln /p:Configuration=Release
	echo.
	echo =======================
	echo  1. Continue with deployment
	echo  2. Cancel deployment
	echo =======================
	choice /C 12 /N /M "Your choice:"
	echo.
	
	if %errorlevel%==1 exit /B 1
exit /B 0


:debug
	if not exist build\debug\player.exe call :build
	build\debug\player.exe data
exit /B 0

:package
	echo Deploying to itch.io ...
	rem Package release files
	if not exist build\deploy mkdir build\deploy
	xcopy /Y build\release\*.dll build\deploy\
	xcopy /Y build\release\player.exe build\deploy\
	xcopy /Y /E toys\basic\ build\deploy\data
	echo.
	
	:package_prompt
		echo =======================
		echo  1. Upload to itch.io
		echo  2. Cancel deployment
		echo  3. Test it first
		echo =======================
		choice /C 123 /N /M "Your choice:"
		echo.
		if %errorlevel%==1 exit /B 1
		if %errorlevel%==2 exit /B 0
		if %errorlevel%==3 (
			pushd build\deploy\
				player.exe data
			popd
		)
	cls
goto :package_prompt

:deploy
	call :build_release
	if %errorlevel%==0 exit /B 0
	
	call :package
	if %errorlevel%==0 exit /B 0
	
	rem Upload process
	pushd build\
		butler push deploy zaklaus/neon-86:win32-minimal-demo
	popd
	pause
exit /B 0

:butler_builds
	butler status zaklaus/neon-86:win32-minimal-demo
	pause
exit /B 0

:open_in_vs
	start neon86.sln
exit /B 0


:open_in_vscode
	start  code.exe .
	cls
exit /B 0


:open_in_lite
	start lite data
exit /B 0

:git_pull
	git pull
	pause
exit /B 0
