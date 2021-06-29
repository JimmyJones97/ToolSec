@set ABI=arm64-v8a
@if DEFINED ANDROID_NDK_ROOT (
    @set ANDROID_NDK=%ANDROID_NDK_ROOT%
) else (
    @set ANDROID_NDK=D:\tools\android-ndk-r19c-windows-x86_64\android-ndk-r19c
)

@echo ANDROID_NDK=%ANDROID_NDK%

@if DEFINED ANDROID_SDK_ROOT (
    @set CMAKE=%ANDROID_SDK_ROOT%\cmake\3.10.2.4988404\bin\cmake.exe
) else (
    @set CMAKE=D:\tools\sdk\cmake\3.10.2.4988404\bin\cmake.exe
)

@echo CMAKE=%CMAKE%

@echo %~dp0
@cd %~dp0

@set BUILD_DIR=_build_%ABI%

rem @rd /s /q %BUILD_DIR%

rem @mkdir %BUILD_DIR%

@echo %BUILD_DIR%
@cd %BUILD_DIR%

@%CMAKE% -G "Unix Makefiles" ^
  -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake ^
  -DANDROID_ABI=%ABI% ^
  -DANDROID_NATIVE_API_LEVEL=16 ^
  -DCMAKE_MAKE_PROGRAM=%ANDROID_NDK%\prebuilt\windows-x86_64\bin\make.exe ^
  -DANDROID_STL=c++_static ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=%~dp0\built\%ABI% ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=%~dp0\built\%ABI% ^
  ..

@%CMAKE% --build . -- log-level=DEBUG

@echo cd %%~dp0 >> REBUILD.cmd
@echo %CMAKE% --build . >> REBUILD.cmd
@echo @ping -n 5 127.0.0.1 ^> nul >> REBUILD.cmd
@cd ..

pause