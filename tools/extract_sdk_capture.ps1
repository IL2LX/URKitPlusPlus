<#
.SYNOPSIS
    Builds a vrchat_slice.json capture from the decompiled SDK sources.

.DESCRIPTION
    The authoritative member signatures come from live IL2CPP metadata. Until that
    capture is available, this derives an equivalent capture from the decompiled
    C#, which is a different shape (Il2CppInterop, unsafe modifiers, Il2CppSystem
    aliases). Output is marked as decompile-derived so it is never mistaken for a
    metadata capture.

    Field versus property is decided from the interop plumbing rather than from
    naming: NativeFieldInfoPtr_<name> is a field, NativeMethodInfoPtr_get_<name>
    is a property. Guessing from casing gets VRChat's statics wrong.
#>
param(
    [string]$Decompile = 'C:\Users\Biscuit\Desktop\VRCDecompile\VRChat',
    [string]$Out      = "$PSScriptRoot\vrchat_slice.decompile.json",
    [string[]]$Types
)

$ErrorActionPreference = 'Stop'

# Whole subtrees that are not mod API. FlatBuffers is VRChat's wire format and
# dwarfs everything else; Validation and MIDI are editor tooling; VRC.Core.data
# is decompiler-generated anonymous type noise.
$skipPrefix = @(
    'VRC.Core.Networking.FlatBuffers',
    'VRC.Core.data',
    'VRC.Core.Pool',
    'VRC.SDKBase.Validation',
    'VRC.SDK3.Validation',
    'VRC.SDKBase.Midi',
    'VRC.SDK3.Midi',
    'VRC.SDK3.Dynamics.Contact.ContactPerformanceScanner',
    'VRC.SDK3.Dynamics.PhysBone.PhysBoneMigration',
    'VRC.SDK3.Dynamics.PhysBone.PhysBonePerformanceScanner',
    'VRC.SDK3.Dynamics.Constraint.ConstraintsPerformanceScanner',
    'VRC.SDKBase.Source.Validation.Performance',
    'VRC.SDK3.Internal.EventPortals',
    'VRC.SDK3.Components.VRCCamera',
    'VRC.SDK3.Components.VRCTestMarker',
    'VRC.SDK3.Components.CustomAttribute',
    'VRC.SDK3.Components.NetworkCallable'
)

# Individual types that wrap no runtime surface: editor attributes, tutorials,
# rating breakdowns, and the SDK2 legacy RPC tree.
$skipExact = @(
    'VRC.SDKBase.PerformanceFilterSet','VRC.SDKBase.PerformanceScannerAttribute',
    'VRC.SDKBase.PerformanceScannerPlaceholder','VRC.SDKBase.PerformanceScannerSet',
    'VRC.SDKBase.AvatarPerformance','VRC.SDKBase.AvatarPerformanceCategory',
    'VRC.SDKBase.AvatarPerformanceStats','VRC.SDKBase.AvatarPerformanceStatsLevel',
    'VRC.SDKBase.AvatarPerformanceStatsLevelSet','VRC.SDKBase.PerformanceInfoDisplayLevel',
    'VRC.SDKBase.PerformancePlatform','VRC.SDKBase.PerformanceRating',
    'VRC.SDKBase.CurveAttribute','VRC.SDKBase.HelpBoxAttribute',
    'VRC.SDKBase.VRCSdkWhitelistAttribute','VRC.SDKBase.Tutorial',
    'VRC.SDKBase.VRC_TutorialAction','VRC.SDKBase.VRC_TutorialAreaMarker',
    'VRC.SDKBase.IPreprocessCallbackBehaviour','VRC.SDKBase.IEditorOnly',
    'VRC.SDKBase.MidiRawEventArgs','VRC.SDKBase.MidiRawMessageDelegate',
    'VRC.SDKBase.MidiVoiceEventArgs','VRC.SDKBase.MidiVoiceMessageDelegate',
    'VRC.SDKBase.IVRCMidiInput','VRC.SDKBase.RPCIgnoredType','VRC.SDKBase.RPC',
    'VRC.SDK3.AbstractUdonBehaviour','VRC.SDK3.UdonSignatureHolderMarker',
    'VRC.SDK3.IUdonSignatureHolder','VRC.SDK3.IUdonSignatureVerifier',
    'VRC.SDK3.Stats','VRC.SDK3.VRCDepthkitMetadata',
    'VRC.SDK3.ScreenUpdateData','VRC.SDK3.ScreenUpdateType',
    'VRC.SDK3.PhysBoneColliderMigration'
    # Superseded by VRC.SDK3.Components.VRCStation and reduces to the same C++
    # symbol, so only the current one can be wrapped.
    'VRC.SDKBase.VRCStation',
    # Empty legacy shim; the live one is VRC.Dynamics.MathUtil in VRC.Dynamics.dll.
    'VRC.SDKBase.MathUtil',
    # Both of these exist in two assemblies and reduce to the same C++ symbol, so
    # only the current SDK3 copy can be wrapped.
    'VRC.SDKBase.Network.VRCNetworkBehaviour',
    'VRC.SDK3.Avatars.Components.VRCSpatialAudioSource'
)

$primitiveMap = @{
    'int'='System.Int32'; 'bool'='System.Boolean'; 'float'='System.Single'
    'string'='System.String'; 'void'='System.Void'; 'long'='System.Int64'
    'byte'='System.Byte'; 'short'='System.Int16'; 'uint'='System.UInt32'
    'double'='System.Double'; 'sbyte'='System.SByte'; 'char'='System.Char'
    'object'='System.Object'
}

$unityMap = @{
    'GameObject'='UnityEngine.GameObject'; 'Transform'='UnityEngine.Transform'
    'Component'='UnityEngine.Component'; 'Object'='UnityEngine.Object'
    'Texture2D'='UnityEngine.Texture2D'; 'Sprite'='UnityEngine.Sprite'
    'Material'='UnityEngine.Material'; 'Shader'='UnityEngine.Shader'
    'Camera'='UnityEngine.Camera'; 'Renderer'='UnityEngine.Renderer'
    'AudioSource'='UnityEngine.AudioSource'; 'RenderTexture'='UnityEngine.RenderTexture'
    'Cubemap'='UnityEngine.Cubemap'; 'Skybox'='UnityEngine.Skybox'
    'Color'='UnityEngine.Color'; 'Vector2'='UnityEngine.Vector2'
    'Vector3'='UnityEngine.Vector3'; 'Vector4'='UnityEngine.Vector4'
    'Quaternion'='UnityEngine.Quaternion'; 'Rect'='UnityEngine.Rect'
    'LayerMask'='UnityEngine.LayerMask'; 'Matrix4x4'='UnityEngine.Matrix4x4'
    'Plane'='UnityEngine.Plane'; 'MonoBehaviour'='UnityEngine.MonoBehaviour'
    'Behaviour'='UnityEngine.Behaviour'; 'ScriptableObject'='UnityEngine.ScriptableObject'
    'Light'='UnityEngine.Light'; 'BoxCollider'='UnityEngine.BoxCollider'
    'Collider'='UnityEngine.Collider'; 'Mesh'='UnityEngine.Mesh'; 'Font'='UnityEngine.Font'
    'Animation'='UnityEngine.Animation'; 'Texture'='UnityEngine.Texture'
    'ComputeShader'='UnityEngine.ComputeShader'; 'AudioClip'='UnityEngine.AudioClip'
}

function Convert-Type([string]$cs, [string]$curNs) {
    if (-not $cs) { return $null }
    $t = $cs.Trim()

    # Il2CppInterop aliases
    $t = $t -replace '^Il2CppSystem\.', 'System.'
    $t = $t -replace '^Il2CppInterop\.Runtime\.InteropTypes\.', ''
    $t = $t -replace '^Il2CppInterop\.Runtime\.InteropTypes\.Arrays\.', ''
    $t = $t -replace '^Il2CppInterop\.Runtime\.', ''
    $t = $t -replace '^System\.', 'System.'
    $t = $t -replace '\s+', ''

    # Arrays
    if ($t -match '^(.+)\[\]$') {
        $inner = Convert-Type $Matches[1] $curNs
        if ($null -eq $inner) { return $null }
        return "$inner[]"
    }
    if ($t -match '^(Il2CppReferenceArray|Array|Il2CppSystem\.Array)<(.+)>$') {
        $inner = Convert-Type $Matches[2] $curNs
        if ($null -eq $inner) { return $null }
        return "$inner[]"
    }
    # Generic collections
    if ($t -match '^(?:System\.Collections\.Generic\.)?(List|IList|IReadOnlyList|HashSet)<(.+)>$') {
        $inner = Convert-Type $Matches[2] $curNs
        if ($null -eq $inner) { return $null }
        return "System.Collections.Generic.List<$inner>"
    }
    if ($t -match '^(?:System\.Collections\.Generic\.)?Dictionary<(.+),(.+)>$') {
        $k = Convert-Type $Matches[1] $curNs
        $v = Convert-Type $Matches[2] $curNs
        if ($null -eq $k -or $null -eq $v) { return $null }
        return "System.Collections.Generic.Dictionary<$k,$v>"
    }

    if ($primitiveMap.ContainsKey($t)) { return $primitiveMap[$t] }
    if ($unityMap.ContainsKey($t))    { return $unityMap[$t] }
    if ($t -match '^(System|UnityEngine|Il2CppSystem)\.') { return $t }

    # Already fully qualified
    if ($t -match '^(VRC|UnityEngine|Il2Cpp)\.[A-Za-z0-9_.]+$') { return $t }

    # A bare VRC type: resolve against the current namespace, then the two known roots.
    if ($t -match '^[A-Za-z_][A-Za-z0-9_]*$') {
        $candidates = @("$curNs.$t", "VRC.SDKBase.$t", "VRC.SDK3.$t", "VRC.Core.$t",
                        "VRC.SDKBase.Network.$t", "VRC.SDK3.Components.$t", "VRC.Udon.$t")
        foreach ($c in $candidates) {
            $file = Join-Path $Decompile (($c -replace '\.','\') + '.cs')
            if (Test-Path $file) { return $c }
        }
        return $null
    }
    return $null
}

function Split-Params([string]$paramText, [string]$curNs) {
    $result = @()
    if (-not $paramText) { return $result }
    $depth = 0; $buf = ''; $parts = @()
    foreach ($ch in $paramText.ToCharArray()) {
        if ($ch -eq '<') { $depth++ }
        elseif ($ch -eq '>') { $depth-- }
        if ($ch -eq ',' -and $depth -eq 0) { $parts += $buf; $buf = '' } else { $buf += $ch }
    }
    if ($buf.Trim()) { $parts += $buf }
    foreach ($p in $parts) {
        $p = $p.Trim()
        if (-not $p) { continue }
        # drop the parameter name: last identifier, and any default value
        $p = $p -replace '=.*$', ''
        $words = $p -split '\s+'
        if ($words.Count -lt 2) { continue }
        $type = ($words[0..($words.Count-2)]) -join ' '
        $mapped = Convert-Type $type $curNs
        if ($null -ne $mapped) { $result += $mapped }
    }
    return $result
}

# Whether a type is a MonoBehaviour, decided by walking the parent chain rather
# than matching the immediate parent. VRCPhysBone derives from VRCPhysBoneBase,
# which derives from VRCNetworkBehaviour, so a single-level check calls it a
# plain class and the emitter gives it the wrong shape.
$componentRoots = '^(MonoBehaviour|Component|Behaviour|VRCNetworkBehaviour|UdonBehaviour|NetworkBehaviour)$'
$chainCache = @{}

function Get-ParentOf([string]$managed) {
    if ($chainCache.ContainsKey($managed)) { return $chainCache[$managed] }
    $path = Join-Path $Decompile (($managed -replace '\.','\') + '.cs')
    if (-not (Test-Path $path)) { $chainCache[$managed] = ''; return '' }
    $m = (Get-Content -LiteralPath $path |
          Select-String -Pattern "public\s+(?:abstract\s+|sealed\s+)?(?:class|struct)\s+[A-Za-z0-9_]+\b.*?:\s*([A-Za-z0-9_.]+)" |
          Select-Object -First 1)
    $parent = if ($m) { $m.Matches[0].Groups[1].Value } else { '' }
    $chainCache[$managed] = $parent
    return $parent
}

function Test-IsComponentChain([string]$parent) {
    $seen = @{}
    $cur = $parent
    for ($depth = 0; $depth -lt 12 -and $cur; $depth++) {
        if ($cur -match $componentRoots) { return $true }
        if ($seen.ContainsKey($cur)) { return $false }
        $seen[$cur] = $true
        # The parent is recorded unqualified in these sources, so it is resolved
        # against the roots the type could plausibly live in.
        $qualified = $null
        foreach ($root in @('VRC.Dynamics','VRC.SDK3','VRC.SDKBase','VRC.Core','VRC.Udon')) {
            $candidate = "$root.$cur"
            if (Test-Path (Join-Path $Decompile (($candidate -replace '\.','\') + '.cs'))) { $qualified = $candidate; break }
        }
        $cur = if ($qualified) { Get-ParentOf $qualified } else { '' }
    }
    return $false
}

function Read-Type([string]$path, [string]$managed, [string]$imageFallback) {
    $lines = Get-Content -LiteralPath $path

    # The IL2CPP image name has to match the assembly a type actually lives in,
    # because that is what the runtime resolves the type by. Guessing it from the
    # folder is wrong: VRC.Core is split across VRCCore-Standalone, VRC.Logging,
    # VRC.Utility and others.
    $image = $imageFallback
    $asm = ($lines | Select-String -Pattern '^// Assembly:\s*([A-Za-z0-9_.-]+)' | Select-Object -First 1)
    if ($asm) { $image = $asm.Matches[0].Groups[1].Value + '.dll' }
    # An editor assembly is not loaded in the client, so a type resolved through
    # it can never be found at runtime.
    $editorOnly = $image -match 'Editor'

    $nsMatch = ($lines | Select-String -Pattern '^namespace\s+([A-Za-z0-9_.]+)\s*;' | Select-Object -First 1)
    $ns = if ($nsMatch) { $nsMatch.Matches[0].Groups[1].Value } else { '' }
    $cls = $managed.Substring($managed.LastIndexOf('.') + 1)

    $parent = ''
    $isAbstract = $false
    $isStatic = $false
    $isIface = $false

    $ifaceLine = ($lines | Select-String -Pattern "public\s+.*interface\s+$([regex]::Escape($cls))\b" | Select-Object -First 1)
    if ($ifaceLine) {
        $isIface = $true
        $parent = 'System.Object'
    } else {
        $classLine = ($lines | Select-String -Pattern "public\s+(?:abstract\s+|sealed\s+)?class\s+$([regex]::Escape($cls))\b" | Select-Object -First 1)
        if ($classLine) {
            $line = $classLine.Line
            $isAbstract = $line -match '\babstract\b'
            $isStatic   = $line -match '\bstatic\b'
            if ($line -match ':\s*([A-Za-z0-9_.]+)') { $parent = $Matches[1] }
        }
    }

    # Classify from the interop plumbing, which is unambiguous.
    $fieldNames = @{}
    $propNames  = @{}
    foreach ($l in $lines) {
        if ($l -match 'NativeFieldInfoPtr_([A-Za-z0-9_]+)\s*;')                  { $fieldNames[$Matches[1]] = $true }
        if ($l -match 'NativeMethodInfoPtr_get_([A-Za-z0-9_]+?)_Public')          { $propNames[$Matches[1]]  = $true }
    }

    $fields = @(); $props = @(); $methods = @()
    $seenMethod = @{}

    foreach ($raw in $lines) {
        $line = $raw.Trim()
        if ($line -notmatch '^public\s' ) { continue }
        if ($line -match 'Native(Field|Method)InfoPtr') { continue }
        if ($line -match '^public\s+(class|struct|enum|interface|delegate|const|event)') { continue }

        $isUnsafe = $line -match '\bunsafe\b'
        $body = $line -replace '^public\s+', ''
        $body = $body -replace '\bunsafe\b', ''
        $body = $body -replace '\b(abstract|virtual|override|sealed|new|readonly|const)\b', ''
        $body = $body.Trim()
        $isStatic = $body -match '^static\s+'
        $body = $body -replace '^static\s+', ''

        if ($body -match '^(?<ret>.+?)\s+(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*\((?<args>.*)\)\s*(\{.*)?$') {
            $name = $Matches['name']
            $ret  = Convert-Type $Matches['ret'] $ns
            if ($null -eq $ret) { continue }
            if ($name -match '^(get_|set_|add_|remove_)') { continue }
            $key = "$name/$($Matches['args'])"
            if ($seenMethod[$key]) { continue }
            $seenMethod[$key] = $true
            $methods += [ordered]@{ n = $name; pt = @(Split-Params $Matches['args'] $ns); rt = $ret; s = $isStatic }
            continue
        }

        if ($body -match '^(?<type>.+?)\s+(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*$') {
            $name = $Matches['name']
            $type = Convert-Type $Matches['type'] $ns
            if ($null -eq $type) { continue }
            if ($name -match '^_') { continue }
            $entry = [ordered]@{ n = $name; t = $type; s = $isStatic }
            if ($propNames.ContainsKey($name)) { $props += $entry } else { $fields += $entry }
        }
    }

    $isComponent = Test-IsComponentChain $parent

    return [ordered]@{
        image         = $image
        editor_only   = $editorOnly
        ns            = $ns
        cls           = $cls
        parent        = $parent
        abstract      = $isAbstract
        static        = $isStatic
        component     = $isComponent
        unity_object  = $true
        iface         = $isIface
        field         = @($fields)
        property      = @($props)
        methods       = @($methods)
    }
}

# --- collect the target list -------------------------------------------------

# Every VRC subfolder that holds runtime types. Dynamics carries the whole
# PhysBone family behind the thin VRCPhysBone/VRCPhysBoneCollider subclasses, so
# leaving it out silently drops those. Obfuscated is decompiler-generated noise
# and SDK/SDKInterface/Networking hold no types of their own.
$roots = @('SDKBase','SDK3','Core','Dynamics','InventoryEffects','Utility',
           'Economy','Localization','CameraSystems')

$all = @()
foreach ($root in $roots) {
    $dir = Join-Path $Decompile "VRC\$root"
    if (-not (Test-Path $dir)) { continue }
    Get-ChildItem -Recurse -File -Filter *.cs $dir | ForEach-Object {
        $rel = $_.FullName.Substring($Decompile.Length + 1) -replace '\\','.' -replace '\.cs$',''
        $all += [pscustomobject]@{ Managed = $rel; Path = $_.FullName }
    }
}

if ($Types -and $Types.Count) {
    $wanted = $Types
    $all = $all | Where-Object { $wanted -contains $_.Managed }
}

$skipSet = @{}
$skipExact | ForEach-Object { $skipSet[$_] = $true }

$outTypes = [ordered]@{}
$built = 0; $skipped = 0
$reasons = @{}

function Skip([string]$name) {
    $script:skipped++
    $r = $script:reasons[$script:why]; if (-not $r) { $script:reasons[$script:why] = 0 }
    $script:reasons[$script:why]++
}

foreach ($entry in ($all | Sort-Object Managed)) {
    # A closed generic has no single runtime type to look up, so it cannot be
    # wrapped. The decompiler renders these with a backtick arity suffix.
    if ($entry.Managed -match '`\d+$') { $why = 'generic'; Skip $entry.Managed; continue }
    if ($skipSet.ContainsKey($entry.Managed)) { $why = 'exact'; Skip $entry.Managed; continue }
    $hit = $skipPrefix | Where-Object { $entry.Managed.StartsWith($_) }
    if ($hit) { $why = "subtree $(@($hit)[0])"; Skip $entry.Managed; continue }

    $image = if ($entry.Managed -like 'VRC.SDK3.*') { 'VRCSDK3.dll' } else { 'VRCSDKBase.dll' }
    $data = Read-Type $entry.Path $entry.Managed $image
    if ($data.editor_only) { $why = 'editor assembly'; Skip $entry.Managed; continue }
    if (-not $data.ns) { $why = 'no namespace'; Skip $entry.Managed; continue }
    $count = $data.field.Count + $data.property.Count + $data.methods.Count
    # A component with no declared members is usually a thin subclass whose
    # surface lives on the base, as VRCPhysBone is on VRCPhysBoneBase. The type
    # still has to resolve, because that is the name a mod puts in a scene, so it
    # is kept and the emitter emits it with empty member tables.
    if ($count -eq 0 -and -not $data.component) { $why = 'no members'; Skip $entry.Managed; continue }

    $outTypes[$entry.Managed] = $data
    $built++
}

$doc = [ordered]@{
    source   = 'VRCDecompile (decompile-derived, NOT live metadata)'
    captured = (Get-Date).ToString('yyyy-MM-ddTHH:mm:ss')
    note     = 'Derived from decompiled C#. Re-capture from live IL2CPP metadata before release.'
    types    = $outTypes
}
$doc | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $Out -NoNewline

Write-Output "wrote $Out"
Write-Output "  types built : $built"
Write-Output "  skipped     : $skipped"
foreach ($k in ($reasons.Keys | Sort-Object { -$reasons[$_] })) {
  Write-Output ("    {0,-46} {1}" -f $k, $reasons[$k])
}
$mem = ($outTypes.Values | ForEach-Object { $_.field.Count + $_.property.Count + $_.methods.Count } | Measure-Object -Sum).Sum
Write-Output "  members     : $mem"
