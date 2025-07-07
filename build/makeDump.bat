@echo off
setlocal enabledelayedexpansion

echo [DUMP] Dumping all source and header files into build\dump.txt...

> dump.txt (
    echo ===== DUMP: All .c source files =====
    for /R ../src %%f in (*.c) do (
        echo.
        echo ---------- %%~nxf ----------
        type "%%f"
    )

    echo.
    echo ===== DUMP: All .h header files =====
    for /R ../src %%f in (*.h) do (
        echo.
        echo ---------- %%~nxf ----------
        type "%%f"
    )
)

echo [DONE] dump.txt generated.
endlocal
