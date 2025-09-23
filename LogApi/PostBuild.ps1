# HLDS Path
$hldsPath = "D:\Files\Desktop\hlds";

# Test HLDS Path
if (!(Test-Path($hldsPath)))
{
    Write-Host("$($hldsPath) was not found.") -ForegroundColor yellow;
    Exit 0
}

# Executable path
$hldsExecutable = "D:\Files\Desktop\hlds\hlds.exe";

# Plugin Name
$pluginName = ((Get-Item .).Name).ToLower();

# Release DLL
$releaseDLL = "$PSScriptRoot\Release\$($pluginName)_mm.dll"

# Test if was build
if (!(Test-Path($releaseDLL)))
{
    Write-Host("Failed do run HLDS: $($releaseDLL) was not found or not build.") -ForegroundColor yellow;
    Exit 0
}

# Get Ethernet address as IPV4
$hldsIP = (Get-NetIPAddress -AddressFamily IPV4 -InterfaceAlias Ethernet).IPAddress;

# Port
$hldsPort = 27020

# Executable parameters
$hldsParam = "-console -game cstrike -port $hldsPort -pingboost 3 -steam -master -secure -bots -timeout 3 +ip $hldsIP +map de_inferno +maxplayers 32 +sys_ticrate 1000";

# DLL Path
$dllPath = "$hldsPath\cstrike\addons\$pluginName\dlls";

# Resolve path of released dll
$releaseDLL = (Resolve-Path($releaseDLL)).Path;

# Find HLDS Process
$hlds = Get-Process -Name hlds -ErrorAction SilentlyContinue;

# If found
if ($hlds)
{
    # If is started 
    if (!$hlds.started)
    {
        # Get HLDS path
        $hldsPath = (Get-Item $hlds.Path).DirectoryName;

        # Set HLDS executable path
        $hldsExecutable = $hlds.Path;
        
        # Set DLL path
        $dllPath = "$hldsPath\cstrike\addons\$pluginName\dlls";

        # Force kill
        $hlds | Stop-Process -Force -ErrorAction SilentlyContinue
    }
}

# Wait 1 second
Start-Sleep(1);

# Check if path exists
if(Test-Path($releaseDLL))
{
    # Copy DLL tao path
    Copy-Item  -Path $releaseDLL -Destination $dllPath -Recurse -Force

    # Start HLDS
    Start-Process -FilePath $hldsExecutable -ArgumentList $hldsParam -WorkingDirectory $hldsPath -WindowStyle Minimized

    # Success
    Write-Host("Running HLDS: IP is $($hldsIP)") -ForegroundColor green;
}
else
{
    # Failed to run
    Write-Host("Failed to run HLDS: Path not found $($dllPath)") -ForegroundColor red;
}