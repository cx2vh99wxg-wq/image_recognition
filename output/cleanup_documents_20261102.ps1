$ErrorActionPreference = 'Stop'
$taskRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).ProviderPath.TrimEnd('\')
$expectedRoot = 'D:\Study_Object\image_recognition\image_recognition'
if (-not [string]::Equals($taskRoot, $expectedRoot, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Unexpected cleanup root: $taskRoot"
}
$branch = (& git -C $taskRoot branch --show-current).Trim()
if ($branch -ne 'prelim_code') { throw "Unexpected branch: $branch" }

function Get-WorkspaceFiles {
    $taskDirs = [Collections.Generic.Stack[string]]::new()
    $taskDirs.Push($taskRoot)
    while ($taskDirs.Count -gt 0) {
        $taskDir = $taskDirs.Pop()
        foreach ($taskItem in Get-ChildItem -LiteralPath $taskDir -Force) {
            if (($taskItem.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { continue }
            if ($taskItem.PSIsContainer) {
                if ($taskItem.Name -notin @('.git','.agents','.codex','.aws')) { $taskDirs.Push($taskItem.FullName) }
            } else { $taskItem }
        }
    }
}

$fmc = Join-Path $taskRoot 'FMC接线引脚清单_S端与M端.md'
$runtimeLabels = Join-Path $taskRoot 'udp_rk_rk_yolo\S\pcie\model\coco_80_labels_list.txt'
$fontLicense = Join-Path $taskRoot 'udp_rk_rk_yolo\S\x11\dashboard_font.LICENSE.txt'
$extraKeep = @($fmc, $runtimeLabels, $fontLicense)
$allFiles = @(Get-WorkspaceFiles)
$documents = @($allFiles | Where-Object { $_.Extension -in @('.txt','.md') })
$keep = @($documents | Where-Object { $_.Name -eq 'README.md' -or $_.FullName -in $extraKeep })
$remove = @($documents | Where-Object { $_.Name -ne 'README.md' -and $_.FullName -notin $extraKeep })
foreach ($required in $extraKeep) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) { throw "Required retained file missing: $required" }
}
$extensionSet = @('.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.inc','.v','.vh','.sv','.vp','.py','.sh','.ps1','.tcl','.pds','.fdc','.pcf','.sdc','.rknn','.hmi','.tft','.sbit','.sfc')
$guardFiles = @($allFiles | Where-Object {
    $_.FullName -notmatch '\\(?:output|tmp|pds_work)\\' -and
    ($_.Extension.ToLowerInvariant() -in $extensionSet -or $_.Name -eq 'Makefile')
}) + $keep
$guardManifest = @($guardFiles | ForEach-Object {
    [pscustomobject]@{ Path=$_.FullName; SHA256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash }
})
$deletions = @($remove | ForEach-Object {
    $resolved = (Resolve-Path -LiteralPath $_.FullName).ProviderPath
    $full = [IO.Path]::GetFullPath($resolved)
    if (-not $full.StartsWith($taskRoot+'\', [StringComparison]::OrdinalIgnoreCase)) { throw "Path escapes workspace: $full" }
    if ([IO.Path]::GetExtension($full) -notin @('.txt','.md')) { throw "Unexpected deletion extension: $full" }
    if ([IO.Path]::GetFileName($full) -eq 'README.md' -or $full -in $extraKeep) { throw "Protected deletion target: $full" }
    [pscustomobject]@{ Path=$full; Bytes=$_.Length; SHA256=(Get-FileHash -LiteralPath $full -Algorithm SHA256).Hash }
})
$manifestPath = Join-Path $PSScriptRoot 'document_cleanup_20261102.json'
$manifest = [ordered]@{
    Root=$taskRoot; Branch=$branch; Status='preflight';
    Policy='Delete .txt/.md documents except README.md, named FMC wiring list, runtime COCO labels and required font license';
    PlannedCount=$deletions.Count; PlannedBytes=($deletions | Measure-Object Bytes -Sum).Sum;
    Retained=@($keep.FullName); Deleted=$deletions; ProtectedFiles=$guardManifest;
    BoardsChanged=$false; OriginDirectoryChanged=$false
}
$manifest | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath $manifestPath -Encoding utf8
# Validate every target and record the complete plan before deleting any file.
# Delete individual verified paths only; never recursively delete a directory.
foreach ($target in $deletions) {
    if ((Get-FileHash -LiteralPath $target.Path -Algorithm SHA256).Hash -ne $target.SHA256) {
        throw "File changed after inventory: $($target.Path)"
    }
}
foreach ($target in $deletions) { Remove-Item -LiteralPath $target.Path -Force }
foreach ($entry in $guardManifest) {
    if (-not (Test-Path -LiteralPath $entry.Path -PathType Leaf)) { throw "Retained file missing: $($entry.Path)" }
    if ((Get-FileHash -LiteralPath $entry.Path -Algorithm SHA256).Hash -ne $entry.SHA256) {
        throw "Retained file changed: $($entry.Path)"
    }
}
$remaining = @(Get-WorkspaceFiles | Where-Object { $_.Extension -in @('.txt','.md') })
$unexpected = @($remaining | Where-Object { $_.Name -ne 'README.md' -and $_.FullName -notin $extraKeep })
if ($unexpected.Count -ne 0) { throw "Unexpected documents remain: $($unexpected.FullName -join ', ')" }
if ($remaining.Count -ne $keep.Count) { throw 'Retained document count changed' }
$manifest.Status = 'complete'
$manifest['RemainingCount'] = $remaining.Count
$manifest['ProtectedFilesVerified'] = $guardManifest.Count
$manifest | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath $manifestPath -Encoding utf8
[pscustomobject]@{
    Deleted=$deletions.Count; DeletedBytes=$manifest.PlannedBytes;
    Retained=$remaining.Count; ProtectedFilesVerified=$guardManifest.Count;
    Branch=$branch; BoardsChanged=$false; Manifest=$manifestPath
} | ConvertTo-Json -Depth 4
