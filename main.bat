@echo off

cd %~dp0
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

	if not exist "demos\%proj%" (
		set "proj=basic"
		echo %proj% > build\.proj
	)
) else (
	echo %proj% > build\.proj
)

set proj=%proj: =%

:begin
	color
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
	echo  E. Run debug
	echo  F. Create new project
	echo  G. Open in explorer
	echo  H. Switch to base template
	echo  I. Open itch.io page
	echo  J. Edit docs
	echo  K. Search on StackOverflow (lol)
	echo =======================
	choice /C 123456789ABCDEFGHIJK /N /M "Your choice:"
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
	if %errorlevel%==14 call :debug
	if %errorlevel%==15 call :new_project
	if %errorlevel%==16 call :open_explorer
	if %errorlevel%==17 call :toggle_base
	if %errorlevel%==18 start "" "https://zaklaus.itch.io/neon-86"
	if %errorlevel%==19 call :open_docs
	if %errorlevel%==20 call :search_so

goto :begin

:build
	%msbuild_cmd% code\neon86.sln /p:Configuration=Debug /p:Platform=x64 /m
	pause
exit /B 0

:build_release
	%msbuild_cmd% code\neon86.sln /p:Configuration=Release /p:Platform=x64 /m
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
	build\debug\player.exe demos\%proj%
exit /B 0

:run_release
	if not exist build\release\player.exe call :build_release
	if %errorlevel%==0 exit /B 0
	build\release\player.exe demos\%proj%
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
	xcopy /Y /E /exclude:.gitignore "demos\%proj%\" "build\deploy\data\"
	xcopy /Y /E /I /exclude:.gitignore libs build\deploy\libs
	xcopy /Y LICENSE.md build\deploy\
	xcopy /Y README.md build\deploy\
	xcopy /Y /E /I /exclude:.gitignore docs build\deploy\docs
	del   /S /Q /F build\deploy\*.blend
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
				rem Delete save files
				del /Q /F data\save.neon
			popd
		)
	cls
goto :package_prompt

:change_project
	cls
	echo NEON86 PROJECTS
	echo =======================
	echo  1. Back to menu
	echo  2. Custom...
	echo  3. Minimal
	echo  4. Mafia
	echo  5. Terrain view
	echo  6. Space
	echo  7. Music demo
	echo  8. RTT demo
	echo  9. Animated sprites
	echo  A. Bullet physics
	echo  B. Shadows
	echo =======================
	choice /C 123456789AB /N /M "Your choice:"
	echo.

	if %errorlevel%==1 exit /B 0
	if %errorlevel%==3 set "proj=minimal"
	if %errorlevel%==4 set "proj=mafia"
	if %errorlevel%==5 set "proj=terrainview"
	if %errorlevel%==6 set "proj=space"
	if %errorlevel%==7 set "proj=music"
	if %errorlevel%==8 set "proj=rtt"
	if %errorlevel%==9 set "proj=fire"
	if %errorlevel%==10 set "proj=bullet"
	if %errorlevel%==11 set "proj=shadowmap"

	if %errorlevel%==2 (
		set /p a="Enter name: "
		if "%a%"=="" goto :change_project
		set "proj=%a%"
		rem Avoid weird bug
		echo %proj% > build\.proj
		exit /B 0
	)

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
	start code\neon86.sln
exit /B 0


:open_in_vscode
	start cmd /C "code.exe ."
exit /B 0


:open_in_lite
	start lite demos\%proj%
exit /B 0

:open_docs
	start docs\neon86_cheatsheet.docx
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

:new_project
	set /P AREYOUSURE="Are you sure (Y/[N])? "
	IF /I "%AREYOUSURE%" NEQ "Y" exit /B 0
	set /p a="Enter name: "
	if "%a%"=="" exit /B 0
	set proj=%a%
	xcopy /Y /E demos\base\ demos\%proj%\
exit /B 0

:toggle_base
	if "%proj%"=="base" (
		set proj=%oldproj%
	) else (
		set oldproj=%proj%
		set proj=base
	)
exit /B 0

:open_explorer
	start explorer.exe demos\%proj%\
exit /B 0

:search_so
	set /p a="so> "
	if "%a%"=="" exit /B 0
	call cmd /C "so %a%"
exit /B 0
