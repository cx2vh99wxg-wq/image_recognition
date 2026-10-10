param([string]$VivadoBin = 'C:\Xilinx\Vivado\2018.3\bin')
$ErrorActionPreference='Stop'
$repo = Split-Path -Parent $PSScriptRoot
$out = Join-Path $repo 'tmp\adas_sim'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$rtl = Join-Path $repo 'fpga\fpga_pcie_ov5640\drive_6ch'
Push-Location $out
try {
    $sources = 'hmi_regs.v','safe_fspi.v','night_isp.v','camera_health.v','tb_drive.sv' | ForEach-Object { Join-Path $rtl $_ }
    & (Join-Path $VivadoBin 'xvlog.bat') --sv @sources
    if ($LASTEXITCODE) { throw 'RTL parsing failed' }
    & (Join-Path $VivadoBin 'xelab.bat') tb_drive -s drive_test
    if ($LASTEXITCODE) { throw 'RTL elaboration failed' }
    $result = & (Join-Path $VivadoBin 'xsim.bat') drive_test -runall 2>&1
    $result | Set-Content -LiteralPath 'drive-result.log' -Encoding utf8
    $result
    # XSim can return exit code zero even after a SystemVerilog $fatal.
    if ($LASTEXITCODE -or ($result -match 'Fatal:') -or -not ($result -match 'PASS: HMI')) { throw 'RTL assertions failed' }
} finally { Pop-Location }
