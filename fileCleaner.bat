del /f /q /s /a rhsa *.bak *.ncb *.obj *.pdb *.idb *.ilk *.pch *.suo *.aps *.opt *.sbr *.bsc *.user *.VC.db
for /f "tokens=*" %%I in ('dir /b /s /ad .vs*') do (rmdir /s /q "%%I")
for /f "tokens=*" %%I in ('dir /b /s /ad Debug*') do (rmdir /s /q "%%I")
for /f "tokens=*" %%I in ('dir /b /s /ad release*') do (rmdir /s /q "%%I")
for /f "tokens=*" %%I in ('dir /b /s /ad ipch*') do (rmdir /s /q "%%I")
pause