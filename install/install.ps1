if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator"))  
{  
  $arguments = "& '" +$myinvocation.mycommand.definition + "'"
  Start-Process powershell -Verb runAs -ArgumentList $arguments
  Break
}

New-Item -ItemType Directory -Force -Path "C:\Program Files\smake\bin"
Invoke-WebRequest -URI "https://cdn.viper.tools/smake/win/smake.exe" -OutFile "C:\Program Files\smake\bin\smake.exe"

if ((Get-Command "smake.exe" -ErrorAction SilentlyContinue) -eq $null) 
{ 
    [Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\Program Files\smake\bin", [System.EnvironmentVariableTarget]::User)
}