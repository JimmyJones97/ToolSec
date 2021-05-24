@if DEFINED ANDROID_SDK_ROOT (
    @set ADB=%ANDROID_SDK_ROOT%\platform-tools\adb.exe
) else (
    @set ADB=D:\tools\platform-tools_r30.0.2-windows\platform-tools\adb.exe
)
@set ABI=arm64-v8a
@%ADB% push built\%ABI%\process_vm_dump /data/local/tmp/process_vm_dump
@%ADB% shell "chmod a+x" /data/local/tmp/process_vm_dump

@set ABI=armeabi-v7a
@%ADB% push built\%ABI%\readname32 /data/local/tmp/readname32
@%ADB% shell "chmod a+x" /data/local/tmp/readname32

@echo "push handy finish"

pause