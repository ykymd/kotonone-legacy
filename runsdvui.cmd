cd /d "C:\dev\KotononeDrive" &msbuild "KotononeDrive.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform=Win32
exit %errorlevel% 