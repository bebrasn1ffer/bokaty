@echo off
rem Put this file into %USERPROFILE%\tools and start it by double click.
set TOOLS=%USERPROFILE%\tools
set PATH=%TOOLS%\git\cmd;%TOOLS%\node;%TOOLS%\sqlite;%PATH%
chcp 65001 >nul
if exist %USERPROFILE%\ivanov cd /d %USERPROFILE%\ivanov
cmd /k
