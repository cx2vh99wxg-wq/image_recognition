param([string]$VivadoBin = 'D:\vivado\Vivado\2018.3\bin')
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$rtl = Join-Path $repo 'fpga\fpga_pcie_ov5640'
$outDir = Join-Path $repo 'tmp\axi_cdc_sim'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
Push-Location $outDir
try {
    & (Join-Path $VivadoBin 'xvlog.bat') --sv (Join-Path $rtl 'stereo_j8\stereo_axi_ctrl.v') (Join-Path $rtl 'drive_6ch\drive_camera_top.v') (Join-Path $rtl 'stereo_j8\test_stereo_axi.sv') (Join-Path $rtl 'drive_6ch\test\tb_axi_reset_cdc.sv')
    if ($LASTEXITCODE) { throw 'AXI RTL parsing failed' }
    foreach ($top in @('test_stereo_axi', 'tb_axi_reset_cdc')) {
        & (Join-Path $VivadoBin 'xelab.bat') $top -s $top
        if ($LASTEXITCODE) { throw "$top elaboration failed" }
        $result = & (Join-Path $VivadoBin 'xsim.bat') $top -runall 2>&1
        $simExit = $LASTEXITCODE
        $result | Set-Content -LiteralPath "$top.log" -Encoding utf8
        $result
        if ($simExit -or ($result -match 'Fatal:') -or -not ($result -match 'PASS')) { throw "$top assertions failed" }
    }
} finally { Pop-Location }
