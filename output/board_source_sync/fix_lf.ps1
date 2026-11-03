param([string]$Port,[string]$Side)
$ErrorActionPreference='Stop'
$p=[IO.Ports.SerialPort]::new($Port,1500000)
$payload=[Convert]::ToBase64String([IO.File]::ReadAllBytes("$PSScriptRoot/${Side}_lf.sh"))
function Send([string]$line){
 for($i=0;$i -lt $line.Length;$i+=32){$p.Write($line.Substring($i,[Math]::Min(32,$line.Length-$i)));Start-Sleep -Milliseconds 3}
 $p.Write("`r")
}
try{
 $p.Open();Send 'stty -echo';Start-Sleep -Milliseconds 250;[void]$p.ReadExisting()
 Send 'src_sync_lf='
 for($i=0;$i -lt $payload.Length;$i+=512){Send ("src_sync_lf+="+$payload.Substring($i,[Math]::Min(512,$payload.Length-$i)))}
 Send 'printf %s "$src_sync_lf" | base64 -d | bash; unset src_sync_lf'
 $out='';for($i=0;$i -lt 40;$i++){Start-Sleep -Milliseconds 250;$out+=$p.ReadExisting();if($out -match 'LF_VERIFIED \d+'){ $Matches[0];return }}
 throw $out
}finally{if($p.IsOpen){Send 'stty echo'};$p.Dispose()}
