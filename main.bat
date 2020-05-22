@echo off

REM detect paths
set msbuild_cmd=msbuild.exe
where /q msbuild.exe
if not %errorlevel%==0 set msbuild_cmd="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"

set devenv_cmd=devenv.exe
where /q devenv.exe
if not %errorlevel%==0 set devenv_cmd="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe"

set "proj=basic"

if not exist build mkdir build
if exist build\.proj (
	set /p proj=<build\.proj
	
	if not exist "toys\%proj%" (
		set "proj=basic"
		echo %proj% > build\.proj
	)
) else (
	echo %proj% > build\.proj
)

set proj=%proj: =%

:begin
	cls
	
	echo NEON86
	echo Active Project: %proj%
	echo =======================
	echo  1. Exit
	echo  2. Build
	echo  3. Debug
	echo  4. Deploy (itch.io)
	echo  5. Open in VS
	echo  6. Open in VS Code
	echo  7. Open in lite
	echo  8. Pull upstream
	echo  9. itch.io build log
	echo  A. Open shell
	echo  B. Change project
	echo  C. Open In VS (minimal)
	echo  D. Run production
	echo  E. Run unit tests
	echo =======================
	choice /C 123456789ABCDE /N /M "Your choice:"
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
	if %errorlevel%==10 call :shell
	if %errorlevel%==11 call :change_project
	if %errorlevel%==12 call :open_in_vs_minimal
	if %errorlevel%==13 call :run_release
	if %errorlevel%==14 call :tests
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
	build\debug\player.exe toys\%proj%
exit /B 0

:run_release
	if not exist build\release\player.exe call :build_release
	if %errorlevel%==0 exit /B 0
	build\release\player.exe toys\%proj%
exit /B 0

:tests
	echo Never!
	pause
exit /B 0

:open_in_vs_minimal
	if not exist build\debug\player.exe call :build
	call %devenv_cmd% build\debug\player.exe
exit /B 0

:package
	echo Packaging files ...
	rem Package release files
	if exist build\deploy rmdir /S /Q build\deploy
	mkdir build\deploy
	xcopy /Y build\release\*.dll build\deploy\
	xcopy /Y build\release\player.exe build\deploy\
	xcopy /Y /E toys\%proj%\ build\deploy\data\
	xcopy /Y LICENSE build\deploy\
	xcopy /Y README.md build\deploy\
	echo.
	
	:package_prompt
		echo NEON86 DEPLOY
		echo Active Project: %proj%
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

:change_project
	cls
	echo NEON86 PROJECTS
	echo =======================
	echo  1. Back to menu
	echo  2. Minimal
	echo  3. Basic
	echo  4. Terrain view
	echo  5. Space
	echo  6. Scene graph
	echo =======================
	choice /C 123456 /N /M "Your choice:"
	echo.
	
	if %errorlevel%==1 exit /B 0
	if %errorlevel%==2 set "proj=minimal"
	if %errorlevel%==3 set "proj=basic"
	if %errorlevel%==4 set "proj=terrainview"
	if %errorlevel%==5 set "proj=space"
	if %errorlevel%==6 set "proj=hierarchy"
	
	echo %proj% > build\.proj
exit /B 0

:deploy
	call :build_release
	if %errorlevel%==0 exit /B 0
	
	call :package
	if %errorlevel%==0 exit /B 0
	
	rem Upload process
	pushd build\
		echo Deploying to itch.io ...
		butler push deploy zaklaus/neon-86:win32-%proj%-demo
	popd
	pause
exit /B 0

:butler_builds
	butler status zaklaus/neon-86
	pause
exit /B 0

:open_in_vs
	start neon86.sln
exit /B 0


:open_in_vscode
	start cmd /C "code.exe toys\%proj%"
exit /B 0


:open_in_lite
	start lite toys\%proj%
exit /B 0

:git_pull
	git pull
	pause
exit /B 0

:shell
	cls
	echo NEON86 SHELL
	echo Enter the EXIT command to get back to main menu.
	echo.
	call cmd
exit /B 0