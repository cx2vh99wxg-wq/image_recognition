param([Parameter(Mandatory=$true)][string]$ExpectedPlanSha256)
$ErrorActionPreference = 'Stop'
$taskAudit = $PSScriptRoot
$taskRoot = [IO.Path]::GetFullPath((Join-Path $taskAudit '../..')).TrimEnd('\','/')
$taskPlanFile = Join-Path $taskAudit 'cleanup_plan.json'
$taskActualPlanHash = (Get-FileHash -LiteralPath $taskPlanFile -Algorithm SHA256).Hash.ToLowerInvariant()
if ($taskActualPlanHash -ne $ExpectedPlanSha256.ToLowerInvariant()) { throw 'Cleanup plan checksum changed' }
$taskPlan = Get-Content -LiteralPath $taskPlanFile -Raw -Encoding UTF8 | ConvertFrom-Json
if ([IO.Path]::GetFullPath($taskPlan.root).TrimEnd('\','/') -ne $taskRoot) { throw 'Workspace mismatch' }
$taskBranch = (& git -C $taskRoot branch --show-current).Trim()
$taskHead = (& git -C $taskRoot rev-parse HEAD).Trim()
if ($taskBranch -ne $taskPlan.branch -or $taskHead -ne $taskPlan.head) { throw 'Current branch or HEAD changed; no deletion' }
$taskPrefix = $taskRoot + [IO.Path]::DirectorySeparatorChar
$taskVerified = [Collections.Generic.List[object]]::new()
foreach ($taskItem in $taskPlan.files) {
    $taskAbsolute = [IO.Path]::GetFullPath((Join-Path $taskRoot $taskItem.path))
    if (!$taskAbsolute.StartsWith($taskPrefix, [StringComparison]::OrdinalIgnoreCase)) { throw "Outside workspace: $taskAbsolute" }
    if ($taskAbsolute -ne [IO.Path]::GetFullPath($taskItem.absolute)) { throw 'Absolute path changed' }
    if ($taskItem.path -match '^(\.git/|HMI/|udp_rk_rk_yolo/|common/|fspi/|output/)' -or
        $taskItem.path -match '/(source|ipcore)/' -or
        $taskItem.path -match '^fpga/fpga_pcie_ov5640/project/generate_bitstream/') { throw "Protected path: $($taskItem.path)" }
    $taskFile = Get-Item -LiteralPath $taskAbsolute -Force
    if ($taskFile.PSIsContainer -or ($taskFile.Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw 'Not a regular file' }
    if ($taskFile.Length -ne $taskItem.size -or $taskFile.LastWriteTimeUtc.Ticks -ne [long]$taskItem.mtime_ticks) {
        throw "File changed after inventory: $($taskItem.path)"
    }
    $taskAncestor = $taskFile.Directory
    while ($taskAncestor.FullName -ne $taskRoot) {
        if (!$taskAncestor.FullName.StartsWith($taskPrefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'Escaping ancestor' }
        if ($taskAncestor.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw 'Reparse ancestor' }
        $taskAncestor = $taskAncestor.Parent
    }
    if ($null -ne $taskItem.duplicate_of) {
        $taskOriginalHash = (Get-FileHash -LiteralPath $taskItem.duplicate_of -Algorithm SHA256).Hash.ToLowerInvariant()
        $taskDuplicateHash = (Get-FileHash -LiteralPath $taskAbsolute -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($taskOriginalHash -ne $taskItem.sha256 -or $taskDuplicateHash -ne $taskItem.sha256) { throw 'Duplicate proof changed' }
    }
    $taskVerified.Add([pscustomobject]@{ Path=$taskAbsolute; Relative=$taskItem.path; Size=[long]$taskItem.size; Reason=$taskItem.reason })
}
Write-Output "PRECHECK_PASSED branch=$taskBranch files=$($taskVerified.Count) all paths verified inside workspace"
$taskDeleted = [Collections.Generic.List[object]]::new()
try {
    foreach ($taskItem in $taskVerified) {
        Remove-Item -LiteralPath $taskItem.Path -Force -ErrorAction Stop
        $taskDeleted.Add([pscustomobject]@{ path=$taskItem.Relative; bytes=$taskItem.Size; reason=$taskItem.Reason })
    }
} finally {
    $taskDeleted | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $taskAudit 'deleted_files.json') -Encoding utf8
}
$taskDeletedBytes = ($taskDeleted | Measure-Object -Property bytes -Sum).Sum
Write-Output ('CLEANUP_COMPLETE files={0} removed_GiB={1:F3}' -f $taskDeleted.Count, ($taskDeletedBytes / 1GB))
