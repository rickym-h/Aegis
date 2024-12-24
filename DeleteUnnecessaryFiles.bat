@echo off
:: List of folder names to delete (separated by spaces)
set "FOLDERS_TO_DELETE=Binaries Intermediate Platforms Saved Build DerivedDataCache"

:: Loop through each folder name
for %%F in (%FOLDERS_TO_DELETE%) do (
    echo Checking for folder "%%F"...
    if exist "%%F" (
        echo Deleting folder "%%F"...
        rmdir /s /q "%%F"
        echo Folder "%%F" deleted successfully.
    ) else (
        echo Folder "%%F" does not exist.
    )
)

:: Pause to show the output
pause
