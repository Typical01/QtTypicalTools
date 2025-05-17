@echo off
setlocal enabledelayedexpansion

:: 要移动的文件名（修改为你的文件名）
set "filename=YourApp.exe"

:: 检查文件是否存在
if not exist "%filename%" (
    echo 错误：文件 "%filename%" 不存在！
    pause
    exit /b 1
)

:: 检查 build 目录是否存在，不存在则创建
if not exist "..\build\" (
    mkdir "..\build"
    echo 创建目录：..\build
)

:: 移动文件
move "%filename%" "..\build\%filename%"

if errorlevel 1 (
    echo 移动失败！
    pause
    exit /b 1
) else (
    echo 成功将 "%filename%" 移动到 ..\build\
)

:: 启动移动后的程序
echo 正在启动程序...
start "" "..\build\%filename%"

pause