@echo off
cls
MSBuild.exe %1 /p:VisualStudioVersion=14.0 /t:Build /nologo /p:Configuration=Debug /p:Platform=x64 /verbosity:m

rem /verbosity
rem You can specify the following verbosity levels:
rem q[uiet], m[inimal], n[ormal] (default), d[etailed], and diag[nostic].