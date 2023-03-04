$InstallLocation = "C:\Program Files\Syntad"
$SmakeAppData = "$env:APPDATA\Syntad\Smake" # DO NOT MODIFY

if (Test-Path $InstallLocation) {
    Remove-Item -Path $InstallLocation -Recurse
}

if (Test-Path $SmakeAppData) {
    Remove-Item -Path $SmakeAppData -Recurse
}

Write-Output "Uninstalled Smake"