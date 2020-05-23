@echo off
SET b=%1

IF "%b%"=="" (SET "b=Build\Debug")

ECHO Build configuration: %b%
CD %~dp0\..

IF "%b%"=="Build\Debug" (
	XCOPY /Y "code\deps\assimp\assimpd.dll" "%b%\"
) ELSE (
	XCOPY /Y "code\deps\assimp\assimp.dll" "%b%\"
)

ECHO Done!