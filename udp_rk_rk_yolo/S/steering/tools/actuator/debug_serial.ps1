param(
    [Parameter(Mandatory=$true)][string]$Port,
    [int]$Baud = 1500000,
    [Parameter(Mandatory=$true)][string]$CommandFile,
    [int]$TimeoutSeconds = 60,
    [string]$LogFile
)
# Run an explicit shell script on the already-connected DEBUG console.
# Source transfer uses the separate checked uploader; this never sends a pulse itself.
$ErrorActionPreference = 'Stop'
$commandText = [IO.File]::ReadAllText((Resolve-Path -LiteralPath $CommandFile)).Replace("`r`n","`n")
$commandBytes = [Text.Encoding]::UTF8.GetBytes($commandText)
$payload = [Convert]::ToBase64String($commandBytes)
$commandSha = [Security.Cryptography.SHA256]::Create()
try { $digest = ([BitConverter]::ToString($commandSha.ComputeHash($commandBytes))).Replace('-','').ToLowerInvariant() }
finally { $commandSha.Dispose() }
$tag = 'STEER_CMD_' + [Guid]::NewGuid().ToString('N')
$remote = 'python3 -u -c ''import base64,hashlib,subprocess; s=base64.b64decode("' + $payload + '"); assert hashlib.sha256(s).hexdigest()=="' + $digest + '"; p=subprocess.run(s,shell=True,executable="/bin/bash"); print("' + $tag + ':%d"%p.returncode,flush=True)'''
if ($remote.Length -gt 3800) { throw 'Script too large for the console line; upload it as a checked file first.' }
$link = [IO.Ports.SerialPort]::new($Port,$Baud,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
$link.Encoding = [Text.UTF8Encoding]::new($false)
$link.DtrEnable = $false; $link.RtsEnable = $false
$link.ReadBufferSize = 1048576; $link.WriteTimeout = 5000
$received = ''; $finished = $false
function Send-ConsoleLine([string]$line) {
    for ($offset=0; $offset -lt $line.Length; $offset+=32) {
        $link.Write($line.Substring($offset,[Math]::Min(32,$line.Length-$offset)))
        Start-Sleep -Milliseconds 2
    }
    $link.Write("`r")
}
try {
    $link.Open()
    $link.Write("`r"); Start-Sleep -Milliseconds 200
    $initial = $link.ReadExisting()
    if ($initial -match 'debug>') { Send-ConsoleLine 'console'; Start-Sleep -Milliseconds 200 }
    Send-ConsoleLine 'stty -echo'
    Start-Sleep -Milliseconds 150
    [void]$link.ReadExisting()
    Send-ConsoleLine $remote
    $deadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
    do {
        $received += $link.ReadExisting()
        if ($received -match ($tag + ':(\d+)')) {
            $code = [int]$Matches[1]; $finished = $true; break
        }
        Start-Sleep -Milliseconds 25
    } while ([DateTime]::UtcNow -lt $deadline)
    if ($LogFile) {
        $destination = [IO.Path]::GetFullPath($LogFile)
        [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($destination)) | Out-Null
        [IO.File]::WriteAllText($destination,$received,[Text.UTF8Encoding]::new($false))
    }
    Write-Output $received
    if (!$finished) { throw 'DEBUG command timed out; its remote state is unknown. Inspect before retrying.' }
    if ($code -ne 0) { throw "Board command returned $code" }
} finally {
    if ($link.IsOpen) {
        if ($finished) { $link.Write("stty echo`r") }
        $link.Close()
    }
    $link.Dispose()
}
