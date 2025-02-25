@echo off
REM Run this script from a folder at the same level with netbeans 'dist' output.
echo * -------------------------------------------------------- *
echo * -      JarMaker V 2.0.0 - For Netbeans Projects        - *
echo * -------------------------------------------------------- *
echo 1) Creating Temporary Directory
mkdir classes
echo 2) Copying Library Jars
xcopy  /s /y ..\dist\lib\*.jar classes >> nul
cd classes
echo 3) Deflating Library Jars (this may take a while)
for /f %%a IN ('dir /b *.jar') do jar xf %%a >> nul
del *.jar >> nul
cd ..
echo 4) Deflating Main Program Jar
xcopy /y ..\dist\*.jar classes >> nul
cd classes
for /f %%a IN ('dir /b *.jar') do jar xf %%a >> nul
del *.jar >> nul
echo 5) Creating Unique Jar (this may take a while)
jar cvfm ../dist.jar META-INF/manifest.mf ./ >> nul
for /f %%a IN ('dir /b ..\..\dist\*.jar') do ren ..\dist.jar %%a >> nul
cd ..
echo 6) Removing Temporary Files
rmdir /s /q classes >> nul
echo * -------------------------------------------------------- *
echo * -         JarMaker V 2.0.0 - Process Complete          - *
echo * -------------------------------------------------------- *
