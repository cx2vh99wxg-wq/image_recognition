param([string]$Port='COM7')
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.IO.Ports
$perfRoot='D:\Study_Object\image_recognition\image_recognition\udp_rk_rk_yolo\S'
$perfFiles=@('fspi/csrc/main.c','fspi/csrc/fspi_module.c','fspi/Makefile','fspi/test_lane_steering.c','fspi/test_auto_brake_switch.c','fspi/test_brake_control.c')
$perfPython=@'
import sys,json,base64,hashlib,pathlib
root=pathlib.Path('/home/linaro/S/perf_build/20261026-steering/incoming').resolve()
root.mkdir(parents=True,exist_ok=True)
handle=None
fid=-1
seq=0
digest=''
print('SERIAL_READY',flush=True)
for raw in sys.stdin.buffer:
 try:
  msg=json.loads(raw)
  op=msg['op']
  if op=='quit':
   if handle: handle.close()
   print('SERIAL_BYE',flush=True)
   break
  if op=='start':
   if handle: handle.close()
   target=(root/msg['name']).resolve()
   if root not in target.parents: raise ValueError('path')
   target.parent.mkdir(parents=True,exist_ok=True)
   handle=open(target,'wb')
   fid=int(msg['id']);seq=0;digest=''
   print('SERIAL_START:%d'%fid,flush=True)
  elif op=='data':
   if int(msg['id'])!=fid: raise ValueError('file')
   data=base64.b64decode(msg['data'],validate=True)
   checksum=hashlib.sha256(data).hexdigest()
   if checksum!=msg['hash']: raise ValueError('checksum')
   index=int(msg['seq'])
   if index==seq:
    handle.write(data);seq+=1
   elif index!=seq-1: raise ValueError('sequence')
   print('SERIAL_ACK:%d:%d:%s'%(fid,index,checksum),flush=True)
  elif op=='end':
   if int(msg['id'])!=fid: raise ValueError('file')
   if handle:
    handle.close();handle=None
    digest=hashlib.sha256(target.read_bytes()).hexdigest()
   if digest!=msg['hash']: raise ValueError('full checksum')
   print('SERIAL_DONE:%d:%s'%(fid,digest),flush=True)
 except Exception:
  print('SERIAL_ERROR',flush=True)
'@
$perfSerial=[IO.Ports.SerialPort]::new($Port,1500000,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
$perfSerial.DtrEnable=$false;$perfSerial.RtsEnable=$false
$perfSerial.Encoding=[Text.UTF8Encoding]::new($false)
$perfSerial.ReadBufferSize=524288;$perfSerial.WriteTimeout=5000
$script:perfRx='';$script:perfRetries=0
function Read-PerfResponse([string]$pattern,[int]$milliseconds) {
 $until=[DateTime]::UtcNow.AddMilliseconds($milliseconds)
 do {
  $script:perfRx+=$perfSerial.ReadExisting()
  if($script:perfRx -match $pattern){return $true}
  if($script:perfRx -match 'SERIAL_ERROR'){return $false}
  Start-Sleep -Milliseconds 3
 } while([DateTime]::UtcNow -lt $until)
 return $false
}
function Send-PerfFrame($frame,[string]$pattern) {
 $line=ConvertTo-Json -InputObject $frame -Compress
 for($attempt=0;$attempt -lt 12;$attempt++) {
  $script:perfRx=''
  $perfSerial.Write($line+"`r")
  if(Read-PerfResponse $pattern 2000){return}
  $script:perfRetries++
  $perfSerial.Write("`r")
  Start-Sleep -Milliseconds 20
  [void]$perfSerial.ReadExisting()
 }
 throw 'Serial packet did not pass acknowledgement/checksum'
}
try {
 $perfSerial.Open();$perfSerial.DiscardInBuffer()
 $bootstrap="stty -echo`nmkdir -p /home/linaro/S/perf_build/20261026-steering`ncat >/home/linaro/S/perf_build/20261026-steering/serial_receive.py <<'PERF_SERIAL_PY'`n"+$perfPython+"`nPERF_SERIAL_PY`npython3 -u /home/linaro/S/perf_build/20261026-steering/serial_receive.py`n"
 foreach($line in ($bootstrap -split "`n")) {
  $perfSerial.Write($line.TrimEnd("`r")+"`r")
  Start-Sleep -Milliseconds 12
  $script:perfRx+=$perfSerial.ReadExisting()
 }
 if(!(Read-PerfResponse 'SERIAL_READY' 15000)){throw 'Serial receiver did not start'}
 $manifest=@();$fileId=0
 foreach($relative in $perfFiles) {
  $bytes=[IO.File]::ReadAllBytes((Join-Path $perfRoot $relative))
  $fullHash=(Get-FileHash -LiteralPath (Join-Path $perfRoot $relative) -Algorithm SHA256).Hash.ToLower()
  Send-PerfFrame @{op='start';id=$fileId;name=$relative} ('SERIAL_START:'+$fileId+'(?:\r|\n)')
  $seq=0
  for($offset=0;$offset -lt $bytes.Length;$offset+=512) {
   $count=[Math]::Min(512,$bytes.Length-$offset)
   $chunk=[byte[]]::new($count)
   [Array]::Copy($bytes,$offset,$chunk,0,$count)
   $sha=[Security.Cryptography.SHA256]::Create()
   try {$hash=([BitConverter]::ToString($sha.ComputeHash($chunk))).Replace('-','').ToLower()} finally {$sha.Dispose()}
   Send-PerfFrame @{op='data';id=$fileId;seq=$seq;hash=$hash;data=[Convert]::ToBase64String($chunk)} ('SERIAL_ACK:'+$fileId+':'+$seq+':'+$hash)
   $seq++
  }
  Send-PerfFrame @{op='end';id=$fileId;hash=$fullHash} ('SERIAL_DONE:'+$fileId+':'+$fullHash)
  $manifest+=$fullHash+'  '+$relative
  Write-Output ('Verified S '+$relative+' '+$fullHash)
  $fileId++
 }
 Send-PerfFrame @{op='quit'} 'SERIAL_BYE'
 $perfSerial.Write("stty echo`r")
 Write-Output ('Direct serial upload complete; retries='+$script:perfRetries)
} finally {
 if($perfSerial.IsOpen){$perfSerial.Write([string][char]3+"`rstty echo`r");$perfSerial.Close()}
 $perfSerial.Dispose()
}


