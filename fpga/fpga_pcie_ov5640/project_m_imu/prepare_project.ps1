$ErrorActionPreference = 'Stop'
$taskProject = $PSScriptRoot
$taskOriginal = Join-Path (Split-Path $taskProject -Parent) 'project'
if (!(Test-Path -LiteralPath (Join-Path $taskProject 'ipcore'))) {
    Copy-Item -LiteralPath (Join-Path $taskOriginal 'ipcore') -Destination (Join-Path $taskProject 'ipcore') -Recurse
}
$taskMap = Join-Path $taskProject 'device_map'
New-Item -ItemType Directory -Path $taskMap -Force | Out-Null
$taskPcf = Join-Path $taskMap 'image_pcie_capture_m_imu.pcf'
if (!(Test-Path -LiteralPath $taskPcf)) {
    Copy-Item -LiteralPath (Join-Path $taskOriginal 'device_map/image_pcie_capture.pcf') -Destination $taskPcf
}
Write-Output 'M project resources ready. Open project_m_imu.pds; original project is unchanged.'
