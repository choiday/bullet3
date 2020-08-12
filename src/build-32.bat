

@echo off
setlocal enableDelayedExpansion

echo Setting up the environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

set /p userver=<otbullet\version.last
echo The last released version: %userver%
set oldver=%userver%

for %%a in (%oldver:.= %) do (
  if not [!last!]==[] (
    if not [!first!]==[] set first=!first!.
    set first=!first!!last!
  )
  set last=%%a
)
rem echo %first%
rem echo %last%
set /A "last+=1"
set newver=%first%.%last%

set userver=
set /p userver=Enter the new version string [%newver%]:
if "%userver%"=="" set userver=%newver%

echo Version: %userver%
echo %userver%>otbullet\version.last


rem rev number

call git rev-parse --short HEAD >otbullet\revision.last
set /p revision=<otbullet\revision.last

echo Revision: %revision%

set VERSTR=%userver%.%revision%
set VERLST=%userver%.0x%revision%
set VERLST=%VERLST:.=,%

echo VERSTR: %VERSTR%
echo VERLST: %VERLST%

msbuild otbullet\otbullet.sln /m /target:Build /p:Configuration=Debug /p:Platform=Win32

set BUILD_STATUS=%ERRORLEVEL%
echo build status (x86 Debug) %BUILD_STATUS%
if "%BUILD_STATUS%" neq "0" goto :eof


msbuild otbullet\otbullet.sln /m /target:Build /p:Configuration=ReleaseLTCG /p:Platform=Win32

set BUILD_STATUS=%ERRORLEVEL%
echo build status (x86 ReleaseLTCG) %BUILD_STATUS%
if "%BUILD_STATUS%" neq "0" goto :eof


del /S /Q ..\..\..\include\bullet\*.*

xcopy BulletCollision ..\..\..\include\bullet\BulletCollision\ /sy /exclude:copy-headers.exc
xcopy BulletDynamics ..\..\..\include\bullet\BulletDynamics\ /sy /exclude:copy-headers.exc
xcopy LinearMath ..\..\..\include\bullet\LinearMath\ /sy /exclude:copy-headers.exc
xcopy *.h ..\..\..\include\bullet\ /y
xcopy otbullet\physics.h ..\..\..\include\bullet\otbullet\ /y
xcopy otbullet\physics_cfg.h ..\..\..\include\bullet\otbullet\ /y
xcopy otbullet\shape_info_cfg.h ..\..\..\include\bullet\otbullet\ /y
xcopy otbullet\docs\*.html ..\..\..\include\bullet\otbullet\docs\ /sy

xcopy ..\bin\Win32\ReleaseLTCG\otbullet.dll ..\..\..\..\bin\ /y
xcopy ..\bin\Win32\ReleaseLTCG\otbullet.pdb ..\..\..\..\bin\ /y
xcopy ..\bin\Win32\Debug\otbulletd.dll ..\..\..\..\bin\ /y
xcopy ..\bin\Win32\Debug\otbulletd.pdb ..\..\..\..\bin\ /y
