param(
    [Parameter(Mandatory=$true)][string]$Port, [int]$Baud=1500000,
    [Parameter(Mandatory=$true)][string]$File,
    [Parameter(Mandatory=$true)][string]$RemoteFile
)
# Checked, stop-and-wait transfer of ONE staging file. Does not install or run it.
$ErrorActionPreference='Stop'
if ($RemoteFile -notmatch '^/home/linaro/[A-Za-z0-9_./-]+$' -or $RemoteFile.Contains('..')) {
    throw 'Remote destination must be a staging file under /home/linaro.'
}
$remote64=[Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($RemoteFile))
$receiver=@'
import sys,json,base64,hashlib,pathlib
target=pathlib.Path(base64.b64decode("REMOTE64").decode())
target.parent.mkdir(parents=True,exist_ok=True)
handle=open(str(target)+'.partial','wb')
seq=0
last_hash=''
print('STEER_UPLOAD_READY',flush=True)
for raw in sys.stdin.buffer:
 try:
  m=json.loads(raw)
  if m['op']=='data':
   data=bytes.fromhex(m['data'])
   h=hashlib.sha256(data).hexdigest()
   if h!=m['hash']: raise ValueError('chunk hash')
   index=int(m['seq'])
   if index==seq:
    handle.write(data);seq+=1;last_hash=h
   elif index!=seq-1 or h!=last_hash: raise ValueError('sequence')
   print('STEER_ACK:%d:%s'%(index,h),flush=True)
  elif m['op']=='end':
   if not handle.closed: handle.close()
   tmp=pathlib.Path(str(target)+'.partial')
   if tmp.stat().st_size!=m['size'] or hashlib.sha256(tmp.read_bytes()).hexdigest()!=m['hash']: raise ValueError('full hash')
   tmp.replace(target)
   print('STEER_UPLOAD_DONE:'+m['hash'],flush=True)
   break
 except Exception as e:
  print('STEER_UPLOAD_ERROR:'+str(e),flush=True)
'@
$receiver=$receiver.Replace('REMOTE64',$remote64)
$bootstrap64=[Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($receiver))
$bootstrap='python3 -u -c ''import base64;exec(base64.b64decode("'+$bootstrap64+'"))'''
$link=[IO.Ports.SerialPort]::new($Port,$Baud,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
$link.Encoding=[Text.UTF8Encoding]::new($false)
$link.DtrEnable=$false;$link.RtsEnable=$false
$link.ReadBufferSize=1048576;$link.WriteTimeout=5000
$script:received='';$script:retries=0;$finished=$false
function Send-ConsoleLine([string]$line) {
    for($offset=0;$offset -lt $line.Length;$offset+=32) {
        $link.Write($line.Substring($offset,[Math]::Min(32,$line.Length-$offset)))
        # Windows sleep can round a 2ms gap up to a whole timer tick.
        # Pace these small writes with a monotonic 2ms interval instead.
        $sendUntil=[Diagnostics.Stopwatch]::GetTimestamp()+[long]([Diagnostics.Stopwatch]::Frequency/500)
        while([Diagnostics.Stopwatch]::GetTimestamp() -lt $sendUntil) {[Threading.Thread]::SpinWait(128)}
    }
    $link.Write("`r")
}
function Restore-UploadConsole {
    # A UART BREAK may leave the kernel IRQ debugger consuming our JSON.
    # Clear its partial command without executing it, then return to Python.
    $link.Write(([string][char]8)*128)
    Start-Sleep -Milliseconds 100
    Send-ConsoleLine 'console'
    Start-Sleep -Milliseconds 150
    $link.Write("`r")
    Start-Sleep -Milliseconds 100
    [void]$link.ReadExisting()
}
function Await-Reply([string]$pattern,[int]$milliseconds) {
    $until=[DateTime]::UtcNow.AddMilliseconds($milliseconds)
    do {
        $script:received+=$link.ReadExisting()
        if($script:received -match $pattern) {return $true}
        if($script:received -match 'debug>') {Restore-UploadConsole;return $false}
        if($script:received -match 'STEER_UPLOAD_ERROR:') {return $false}
        Start-Sleep -Milliseconds 3
    } while([DateTime]::UtcNow -lt $until)
    return $false
}
function Send-Frame($frame,[string]$pattern) {
    $line=ConvertTo-Json -InputObject $frame -Compress
    for($attempt=0;$attempt -lt 20;$attempt++) {
        $script:received='';Send-ConsoleLine $line
        if(Await-Reply $pattern 2500) {return}
        $script:retries++
        Start-Sleep -Milliseconds 30
        [void]$link.ReadExisting()
    }
    throw ('Upload acknowledgement timed out; staging file has not been installed. Expected '+$pattern+' Received '+$script:received)
}
try {
    $bytes=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $File))
    $fullHash=(Get-FileHash -LiteralPath $File -Algorithm SHA256).Hash.ToLowerInvariant()
    $link.Open();$link.Write("`r");Start-Sleep -Milliseconds 200
    $initial=$link.ReadExisting()
    if($initial -match 'debug>') {Send-ConsoleLine 'console';Start-Sleep -Milliseconds 200}
    Send-ConsoleLine 'stty -echo'
    Start-Sleep -Milliseconds 150;[void]$link.ReadExisting()
    Send-ConsoleLine $bootstrap
    if(!(Await-Reply 'STEER_UPLOAD_READY' 10000)) {throw 'Upload receiver did not start'}
    $seq=0
    for($offset=0;$offset -lt $bytes.Length;$offset+=128) {
        $count=[Math]::Min(128,$bytes.Length-$offset);$chunk=[byte[]]::new($count)
        [Array]::Copy($bytes,$offset,$chunk,0,$count)
        $sha=[Security.Cryptography.SHA256]::Create()
        try {$hash=([BitConverter]::ToString($sha.ComputeHash($chunk))).Replace('-','').ToLowerInvariant()} finally {$sha.Dispose()}
        $hex=([BitConverter]::ToString($chunk)).Replace('-','').ToLowerInvariant()
        Send-Frame @{op='data';seq=$seq;hash=$hash;data=$hex} ('STEER_ACK:'+$seq+':'+$hash)
        $seq++
        if($seq%128 -eq 0) {Write-Output "Transferred $offset / $($bytes.Length) bytes"}
    }
    Send-Frame @{op='end';size=$bytes.Length;hash=$fullHash} ('STEER_UPLOAD_DONE:'+$fullHash)
    $finished=$true
    Write-Output "VERIFIED $RemoteFile size=$($bytes.Length) sha256=$fullHash retries=$script:retries"
} finally {
    if($link.IsOpen) {
        if($finished) {$link.Write("stty echo`r")} else {$link.Write("`r"+[string][char]4);Start-Sleep -Milliseconds 150;$link.Write("stty echo`r")}
        $link.Close()
    }
    $link.Dispose()
}
