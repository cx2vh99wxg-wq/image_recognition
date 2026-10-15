param([string]$VivadoBin = 'D:\vivado\Vivado\2018.3\bin')
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$rtl = Join-Path $repo 'fpga\fpga_pcie_ov5640\drive_6ch'
$outDir = Join-Path $repo 'tmp\isp_pipeline_sim'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
Push-Location $outDir
try {
    & (Join-Path $VivadoBin 'xvlog.bat') --sv (Join-Path $rtl 'night_isp.v') (Join-Path $rtl 'test\night_isp_reference.v') (Join-Path $rtl 'test\tb_night_isp_pipeline.sv')
    if ($LASTEXITCODE) { throw 'ISP RTL parsing failed' }
    & (Join-Path $VivadoBin 'xelab.bat') tb_night_isp_pipeline -s isp_pipeline_test
    if ($LASTEXITCODE) { throw 'ISP elaboration failed' }
    $result = & (Join-Path $VivadoBin 'xsim.bat') isp_pipeline_test -runall 2>&1
    $simExit = $LASTEXITCODE
    $result | Set-Content -LiteralPath 'result.log' -Encoding utf8
    $result
    if ($simExit -or ($result -match 'Fatal:') -or -not ($result -match 'PASS: ISP equivalence')) { throw 'ISP equivalence failed' }
} finally { Pop-Location }
