param([string]$Port='COM8',[string]$RemoteFile,[string]$LocalFile)
$ErrorActionPreference='Stop'
$lanePy="import pathlib,base64,hashlib; d=pathlib.Path('"+$RemoteFile+"').read_bytes(); print('DL_HASH:'+hashlib.sha256(d).hexdigest(),flush=True); [print('DL_DATA:'+base64.b64encode(d[i:i+768]).decode(),flush=True) for i in range(0,len(d),768)]; print('DL_DONE',flush=True)"
$laneCode=[Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($lanePy))
$lanePort=[IO.Ports.SerialPort]::new($Port,1500000,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
$lanePort.DtrEnable=$false;$lanePort.RtsEnable=$false;$lanePort.ReadBufferSize=1048576
$laneRx=[Text.StringBuilder]::new()
try {
 $lanePort.Open();$lanePort.DiscardInBuffer();$lanePort.Write("stty -echo`r");Start-Sleep -Milliseconds 50
 $lanePort.Write('python3 -c "import base64;exec(base64.b64decode('''+$laneCode+'''))"'+"`r")
 $laneUntil=[DateTime]::UtcNow.AddSeconds(45)
 do {Start-Sleep -Milliseconds 15;[void]$laneRx.Append($lanePort.ReadExisting())}while([DateTime]::UtcNow -lt $laneUntil -and $laneRx.ToString() -notmatch '(?m)^DL_DONE\r?$')
 $laneAll=$laneRx.ToString();if($laneAll -notmatch 'DL_HASH:([0-9a-f]{64})'){throw 'No download checksum'};$laneExpected=$Matches[1]
 $laneData=[regex]::Matches($laneAll,'DL_DATA:([A-Za-z0-9+/=]+)')
 $laneStream=[IO.MemoryStream]::new();foreach($laneChunk in $laneData){$laneBytes=[Convert]::FromBase64String($laneChunk.Groups[1].Value);$laneStream.Write($laneBytes,0,$laneBytes.Length)}
 $laneBytes=$laneStream.ToArray();$laneSha=[Security.Cryptography.SHA256]::Create();$laneActual=([BitConverter]::ToString($laneSha.ComputeHash($laneBytes))).Replace('-','').ToLower();$laneSha.Dispose()
 if($laneActual -ne $laneExpected){throw 'Download checksum failed'}
 [IO.File]::WriteAllBytes((Join-Path (Get-Location) $LocalFile),$laneBytes)
 Write-Output ('Downloaded '+$LocalFile+' bytes='+$laneBytes.Length+' sha256='+$laneActual)
} finally {if($lanePort.IsOpen){$lanePort.Write("stty echo`r");$lanePort.Close()};$lanePort.Dispose()}
