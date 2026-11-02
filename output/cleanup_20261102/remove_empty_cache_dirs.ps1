$ErrorActionPreference = 'Stop'
$taskRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..')).TrimEnd('\','/')
$taskPrefix = $taskRoot + [IO.Path]::DirectorySeparatorChar
$taskDeleted = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'deleted_files.json') -Raw -Encoding utf8 | ConvertFrom-Json
$taskDirectories = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach ($taskFile in $taskDeleted) {
    $taskParent = [IO.Path]::GetDirectoryName([IO.Path]::GetFullPath((Join-Path $taskRoot $taskFile.path)))
    while ($taskParent -and $taskParent -ne $taskRoot) {
        if (!$taskParent.StartsWith($taskPrefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'Outside workspace' }
        [void]$taskDirectories.Add($taskParent)
        $taskParent = [IO.Path]::GetDirectoryName($taskParent)
    }
}
$taskRemoved = [Collections.Generic.List[string]]::new()
foreach ($taskDir in ($taskDirectories | Sort-Object Length -Descending)) {
    if (!(Test-Path -LiteralPath $taskDir -PathType Container)) { continue }
    $taskInfo = Get-Item -LiteralPath $taskDir -Force
    if ($taskInfo.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw 'Reparse directory' }
    if ((Get-ChildItem -LiteralPath $taskDir -Force | Select-Object -First 1) -ne $null) { continue }
    # Nonrecursive: a concurrent new file causes an error, never deletion of it.
    [IO.Directory]::Delete($taskDir, $false)
    $taskRemoved.Add($taskDir.Substring($taskPrefix.Length))
}
$taskRemoved | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $PSScriptRoot 'removed_empty_directories.json') -Encoding utf8
Write-Output "EMPTY_CACHE_DIRECTORIES_REMOVED $($taskRemoved.Count)"
