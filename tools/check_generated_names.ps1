<#
.SYNOPSIS
    Name check for every captured VRChat type, against the decompiled sources.

.DESCRIPTION
    The capture is read for the names the generator will use, and the .cs files
    are read fresh as ground truth. Those are different paths, so agreement
    between them is a real check rather than a restatement.

    For every type:
      1. the managed name resolves to a real file that declares it
      2. the namespace in the capture matches the file's
      3. the image matches the file's // Assembly: marker
      4. the C++ name is a valid identifier and unique in its namespace
      5. the header path the generator will use follows the namespace
      6. every captured field, property and method name is a public member

    Exits 1 if any check fails.
#>
param(
    [string]$Decompile = 'C:\Users\Biscuit\Desktop\VRCDecompile\VRChat',
    [string]$Repo      = (Split-Path -Parent $PSScriptRoot),
    [switch]$ShowAll
)

$ErrorActionPreference = 'Stop'

$capturePath = "$Repo\tools\vrchat_slice.decompile.json"
if (-not (Test-Path $capturePath)) { throw "capture not found: $capturePath (run tools/regen_vrchat_table.ps1)" }

$problems = New-Object System.Collections.Generic.List[string]
function Bad([string]$kind, [string]$name, [string]$detail) {
    $problems.Add(("{0,-10} {1,-58} {2}" -f $kind, $name, $detail))
}

function Convert-Pascal([string]$managed) {
    $dot = $managed.LastIndexOf('.')
    $name = if ($dot -lt 0) { $managed } else { $managed.Substring($dot + 1) }
    if ($name.StartsWith('VRC_')) { $name = $name.Substring(4) }
    $out = ''
    $upper = $true
    foreach ($c in $name.ToCharArray()) {
        if ($c -eq '_') { $out += $c; $upper = $true; continue }
        $out += if ($upper) { [char]::ToUpperInvariant($c) } else { $c }
        $upper = $false
    }
    return $out
}

function Convert-Ns([string]$ns) {
    if ($ns -eq 'VRC' -or -not $ns) { return 'VRC' }
    $rest = if ($ns.StartsWith('VRC.')) { $ns.Substring(4) } else { $ns }
    return 'VRC::' + ($rest -replace '\.', '::')
}

function Convert-Header([string]$ns, [string]$cls) {
    $rest = if ($ns.StartsWith('VRC.')) { $ns.Substring(4) } else { $ns }
    return 'sdk/VRChat/VRC/' + ($rest -replace '\.', '/') + "/$cls.h"
}

# Public member names of a decompiled type, taken three ways because the decompile
# presents them differently: plain fields, properties behind a get_ accessor, and
# methods. Property accessors are excluded because they are the implementation of
# a property, not a member in their own right.
function Get-PublicNames([string]$src) {
    $names = @{}
    foreach ($m in [regex]::Matches($src, 'NativeFieldInfoPtr_([A-Za-z0-9_]+)\s*;')) { $names[$m.Groups[1].Value] = $true }
    foreach ($m in [regex]::Matches($src, 'NativeMethodInfoPtr_get_([A-Za-z0-9_]+?)_Public')) { $names[$m.Groups[1].Value] = $true }
    foreach ($m in [regex]::Matches($src, '(?m)^\s*public\s+[^;{(]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*[;{(]')) {
        $n = $m.Groups[1].Value
        if ($n -notmatch '^(get_|set_|add_|remove_)') { $names[$n] = $true }
    }
    return $names
}

$cap = Get-Content -LiteralPath $capturePath -Raw | ConvertFrom-Json
$types = $cap.types.PSObject.Properties

# Managed name -> file, indexed from each file's own // Type: header. Looking a
# type up by namespace path is exactly the assumption that turned out to be
# wrong: VRCParentConstraint sits in VRC\SDK3\Dynamics\Constraint\ but is
# VRC.SDK3.Dynamics.Constraint.Components.VRCParentConstraint.
$fileIndex = @{}
foreach ($f in Get-ChildItem -Recurse -File -Filter *.cs $Decompile) {
    $marker = Get-Content -LiteralPath $f.FullName -TotalCount 4 |
        Select-String -Pattern '^// Type:\s*(\S+)' | Select-Object -First 1
    if (-not $marker) { continue }
    $fileIndex[$marker.Matches[0].Groups[1].Value] = $f.FullName
}

# Every header path the generator will actually write, taken from the emitted
# factories rather than recomputed, so a mismatch points at real output.
$factoryText = Get-Content -LiteralPath "$Repo\src\sdk\templates\VRChat\vrchat_generated_factories.inl" -Raw
$factories = @{}
foreach ($m in [regex]::Matches($factoryText, 'EmitType\(VrcGenerated::\w+, "([^"]+)"\)')) {
    $factories[$m.Groups[1].Value] = $true
}

# Types that are legitimately not generated: the hand-maintained TypeSpecs, and
# any header path another template already owns.
$handSpecs = @{}
$tableText = Get-Content -LiteralPath "$Repo\src\sdk\templates\VRChat\vrchat_generated_table.inl" -Raw
foreach ($m in [regex]::Matches($tableText, '(?s)inline const TypeSpec k\w+ = \{.*?", "[^"]+\.dll", "([^"]+)", "([^"]+)",')) {
    $handSpecs["$($m.Groups[1].Value)|$($m.Groups[2].Value)"] = $true
}
$takenPaths = @{}
$commonText = Get-Content -LiteralPath "$Repo\src\sdk\mod_project_generator_common.cpp" -Raw
$commonText = [regex]::Replace($commonText, '(?s)// >>> VRCHAT_GENERATED_WRITES >>>.*?// <<< VRCHAT_GENERATED_WRITES <<<', '')
foreach ($m in [regex]::Matches($commonText, '\{("(?:[^"\\]|\\.)*\.h")\s*,\s*OutputFilePolicy')) {
    $takenPaths[$m.Groups[1].Value.Trim('"')] = $true
}

$seenNs = @{}
$stats = @{ types = 0; fields = 0; properties = 0; methods = 0; members = 0 }

foreach ($tp in $types) {
    $managed = $tp.Name
    $t = $tp.Value
    $ns = $t.ns
    $cls = $t.cls
    $stats.types++

    # 4. C++ name validity and uniqueness
    $cppName = 'Vrc' + (Convert-Pascal $managed)
    $cppNs = Convert-Ns $ns
    if ($cppName -notmatch '^[A-Za-z_][A-Za-z0-9_]*$') { Bad 'cpp-name' $managed "invalid identifier: $cppName" }
    $key = "$cppNs|$cppName"
    if ($seenNs.ContainsKey($key)) { Bad 'cpp-name' $managed "collides with $($seenNs[$key])" }
    else { $seenNs[$key] = $managed }

    # 1. the type has to exist, resolved by its real managed name
    $path = $fileIndex[$managed]
    if (-not $path) { Bad 'missing' $managed 'no decompiled file declares this type'; continue }
    $src = Get-Content -LiteralPath $path -Raw

    # 2. namespace
    if ($src -notmatch ('namespace\s+' + [regex]::Escape($ns) + '\s*;')) {
        $actual = [regex]::Match($src, 'namespace\s+([A-Za-z0-9_.]+)\s*;')
        Bad 'namespace' $managed "file says '$($actual.Groups[1].Value)', capture says '$ns'"
    }

    # 3. image
    $asm = [regex]::Match($src, '(?m)^// Assembly:\s*([A-Za-z0-9_.-]+)')
    if ($asm.Success) {
        $expected = $asm.Groups[1].Value + '.dll'
        if ($expected -ne $t.image) { Bad 'image' $managed "capture says $($t.image), assembly is $expected" }
    }

    # 5. header path. A type that is deliberately not generated has no factory and
    # that is correct, so only an unexpected one is a problem: the hand-written
    # TypeSpecs and the types whose header path another template already owns.
    $expectedPath = Convert-Header $ns $cls
    if (-not $factories.Contains($expectedPath)) {
        $handWritten = $handSpecs.ContainsKey("$ns|$cls")
        $pathTaken = $takenPaths.Contains($expectedPath)
        if (-not ($handWritten -or $pathTaken)) {
            Bad 'path' $managed "no factory writes $expectedPath and nothing else claims it"
        }
    }

    # 6. every captured member name has to be a real public member
    $known = Get-PublicNames $src
    $kindKey = @{ 'field' = 'fields'; 'property' = 'properties'; 'methods' = 'methods' }
    foreach ($kind in @('field', 'property', 'methods')) {
        $list = @($t.$kind)
        $stats[$kindKey[$kind]] += $list.Count
        $stats.members += $list.Count
        foreach ($e in $list) {
            if (-not $e.n) { continue }
            if ($kind -eq 'methods') {
                # A method may be listed more than once per overload set; each
                # name still has to exist.
                if ($e.n -match '^(get_|set_|add_|remove_)') { continue }
            }
            if (-not $known.ContainsKey($e.n)) {
                Bad $kind $managed "'$($e.n)' is not a public member of $cls"
            }
        }
    }
}

Write-Output "name check over $($stats.types) captured types"
Write-Output "  fields $($stats.fields)  properties $($stats.properties)  methods $($stats.methods)  total $($stats.members)"
Write-Output ''
if ($problems.Count -eq 0) {
    Write-Output 'RESULT: clean'
    exit 0
}
Write-Output "RESULT: $($problems.Count) problems"
Write-Output ''
$problems | Group-Object { ($_ -split '\s+')[0] } | Sort-Object Count -Descending |
    ForEach-Object { Write-Output ("  {0,5}  {1}" -f $_.Count, $_.Name) }
Write-Output ''
$show = if ($ShowAll) { $problems } else { $problems | Select-Object -First 40 }
$show | ForEach-Object { Write-Output "  $_" }
if (-not $ShowAll -and $problems.Count -gt $show.Count) {
    Write-Output "  ... and $($problems.Count - $show.Count) more (use -ShowAll)"
}
exit 1
