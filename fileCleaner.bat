del /f /q /s /a rhsa *.bak *.ncb *.obj *.idb *.ilk *.aps *.opt *.sbr *.bsc *.VC.db
for /f "tokens=*" %%I in ('dir /b /s /ad Debug*') do (rmdir /s /q "%%I")
for /f "tokens=*" %%I in ('dir /b /s /ad release*') do (rmdir /s /q "%%I")
for /f "tokens=*" %%I in ('dir /b /s /ad x64*') do (rmdir /s /q "%%I")
pause