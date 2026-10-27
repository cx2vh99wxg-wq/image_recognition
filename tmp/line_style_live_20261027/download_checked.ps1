param([string]$Port,[string]$RemoteFile,[string]$LocalFile)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.IO.Ports
$taskSource=@'
import sys,pathlib,base64,hashlib
data=pathlib.Path(sys.argv[1]).read_bytes()
print('GET_INFO:%d:%s'%(len(data),hashlib.sha256(data).hexdigest()),flush=True)
for line in sys.stdin:
 try:
  text=line.strip()
  if text=='QUIT': print('GET_BYE',flush=True);break
  offset=int(text);chunk=data[offset:offset+768]
  print('GET_CHUNK:%d:%s:%s'%(offset,base64.b64encode(chunk).decode(),hashlib.sha256(chunk).hexdigest()),flush=True)
 except Exception: print('GET_ERROR',flush=True)
'@
$taskCode=[Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($taskSource))
if($RemoteFile -notmatch '^/[A-Za-z0-9_./-]+$'){throw 'Unexpected remote path'}
$taskSerial=[IO.Ports.SerialPort]::new($Port,1500000,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
$taskSerial.DtrEnable=$false;$taskSerial.RtsEnable=$false;$taskSerial.ReadBufferSize=1048576;$taskSerial.WriteTimeout=5000
$script:taskRx=''
function Read-Checked([string]$pattern,[int]$timeout) {
 $until=[DateTime]::UtcNow.AddMilliseconds($timeout)
 do {
  $script:taskRx+=$taskSerial.ReadExisting()
  if($script:taskRx -match $pattern){return $Matches}
  Start-Sleep -Milliseconds 2
 } while([DateTime]::UtcNow -lt $until)
 return $null
}
function Get-HashBytes([byte[]]$data) {
 $sha=[Security.Cryptography.SHA256]::Create()
 try {return ([BitConverter]::ToString($sha.ComputeHash($data))).Replace('-','').ToLower()} finally {$sha.Dispose()}
}
try {
 $taskSerial.Open();$taskSerial.DiscardInBuffer();$taskSerial.Write("stty -echo`r");Start-Sleep -Milliseconds 50
 $taskSerial.Write('python3 -u -c "import base64;exec(base64.b64decode('''+$taskCode+'''))" '+$RemoteFile+"`r")
 $info=Read-Checked 'GET_INFO:([0-9]+):([a-f0-9]{64})' 10000
 if(!$info){throw 'Download receiver not ready'}
 $length=[int]$info[1];$expected=$info[2];$buffer=[byte[]]::new($length);$retries=0
 for($offset=0;$offset -lt $length;$offset+=768) {
  $accepted=$false
  for($attempt=0;$attempt -lt 8;$attempt++) {
   $script:taskRx='';$taskSerial.Write([string]$offset+"`r")
   $chunk=Read-Checked ('GET_CHUNK:'+$offset+':([A-Za-z0-9+/=]+):([a-f0-9]{64})') 1500
   if($chunk){
    try {$data=[Convert]::FromBase64String($chunk[1])} catch {$data=$null}
    if($null -ne $data -and $data.Length -eq [Math]::Min(768,$length-$offset) -and (Get-HashBytes $data) -eq $chunk[2]) {
     [Array]::Copy($data,0,$buffer,$offset,$data.Length);$accepted=$true;break
    }
   }
   $retries++
  }
  if(!$accepted){throw ('Download chunk failed: '+$offset)}
 }
 if((Get-HashBytes $buffer) -ne $expected){throw 'Full checksum mismatch'}
 $taskSerial.Write("QUIT`r");[void](Read-Checked 'GET_BYE' 2000)
 [IO.File]::WriteAllBytes((Join-Path (Get-Location) $LocalFile),$buffer)
 Write-Output ('Downloaded '+$LocalFile+' bytes='+$length+' retries='+$retries+' sha256='+$expected)
} finally {
 if($taskSerial.IsOpen){$taskSerial.Write([string][char]3+"`rstty echo`r");$taskSerial.Close()}
 $taskSerial.Dispose()
}
