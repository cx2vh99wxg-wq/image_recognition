param([string]$Port,[string]$Side,[int]$Resume=-1,[int]$Part=-1)
$ErrorActionPreference='Stop'
$payload=python -B "$PSScriptRoot/source_stream.py" $Side $Part | ConvertFrom-Json
if($LASTEXITCODE -ne 0 -or !$payload.count){throw 'Source enumeration failed'}
$receiver=[IO.File]::ReadAllText("$PSScriptRoot/receive.py").Replace('@SIDE@',$Side)
$encoded=[Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($receiver))
$cmd='python3 -u -c ''import base64;exec(base64.b64decode("'+$encoded+'"))'''
$p=[IO.Ports.SerialPort]::new($Port,1500000)
$p.WriteTimeout=5000
function Send([string]$line){
 for($o=0;$o -lt $line.Length;$o+=32){
  $p.Write($line.Substring($o,[Math]::Min(32,$line.Length-$o)))
  $end=[Diagnostics.Stopwatch]::GetTimestamp()+[long]([Diagnostics.Stopwatch]::Frequency/425)
  while([Diagnostics.Stopwatch]::GetTimestamp() -lt $end){[Threading.Thread]::SpinWait(128)}
 }
 $p.Write("`r")
}
function Await([string]$pattern,[int]$ms){
 $end=[DateTime]::UtcNow.AddMilliseconds($ms);$buf=''
 do{
  $buf+=$p.ReadExisting()
  if($buf -match $pattern){return $Matches[0]}
  if($buf -match 'debug>'){Send 'console';Start-Sleep -Milliseconds 250;return ''}
  if($buf -match 'SYNC_ERR [^\r\n]+'){throw $Matches[0]}
  Start-Sleep -Milliseconds 4
 }while([DateTime]::UtcNow -lt $end)
 return ''
}
try{
 $p.Open()
 if($Resume -ge 0){
   $p.Write("`r");Start-Sleep -Milliseconds 350
   if($p.ReadExisting() -match 'debug>'){Send 'console';Start-Sleep -Milliseconds 450;[void]$p.ReadExisting()}
 }else{
   $p.Write(([string][char]3)+"`r");Start-Sleep -Milliseconds 250
   if($p.ReadExisting() -match 'debug>'){Send 'console';Start-Sleep -Milliseconds 300}
   $p.Write(([string][char]3)+"`r");Start-Sleep -Milliseconds 350
   Send 'stty -echo';Start-Sleep -Milliseconds 300;[void]$p.ReadExisting()
   Send $cmd
   if(!(Await 'SYNC_READY' 5000)){throw 'Receiver unavailable'}
 }
 $i=[Math]::Max(0,$Resume)
 for($o=$i*768;$o -lt $payload.data.Length;$o+=768){
  $chunk=$payload.data.Substring($o,[Math]::Min(768,$payload.data.Length-$o))
  $sha=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData([Text.Encoding]::ASCII.GetBytes($chunk))).ToLowerInvariant().Substring(0,16)
  $ack=''
  for($try=0;$try -lt 4 -and !$ack;$try++){Send "$i $chunk";$ack=Await "SYNC_ACK $i $sha" 1100}
  if(!$ack){throw "Chunk $i timed out"}
  $i++
 }
 Send ('END '+$payload.hash)
 $done=Await ("SYNC_DONE $Side "+$payload.count+' \d+ [^\r\n]+') 30000
 if(!$done -or !$done.EndsWith($payload.hash)){throw 'No verified completion receipt'}
 $done
}finally{
 if($p.IsOpen){$p.Write(([string][char]3)+"stty echo`r");$p.Close()}
 $p.Dispose()
}





