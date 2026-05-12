$exeDir = "$env:TEMP"
$asm = [Reflection.Assembly]::LoadFile("$exeDir\app.exe")

# Try to load Newtonsoft.Json from same directory
$jsonPath = Join-Path (Split-Path "$exeDir\app.exe") "Newtonsoft.Json.dll"
if (Test-Path $jsonPath) {
    [Reflection.Assembly]::LoadFile($jsonPath) | Out-Null
}

$types = $asm.GetTypes()
$strings = @{}
foreach ($t in $types) {
    $methods = $t.GetMethods([Reflection.BindingFlags]::NonPublic -bor [Reflection.BindingFlags]::Public -bor [Reflection.BindingFlags]::Instance -bor [Reflection.BindingFlags]::Static -bor [Reflection.BindingFlags]::DeclaredOnly)
    foreach ($m in $methods) {
        try {
            $body = $m.GetMethodBody()
            if ($body) {
                $il = $body.GetILAsByteArray()
                for ($i = 0; $i -lt $il.Length - 4; $i++) {
                    if ($il[$i] -eq 0x72) {
                        $token = [BitConverter]::ToInt32($il, $i+1)
                        try {
                            $str = $asm.GetString($token)
                            if ($str -and $str.Length -gt 0) {
                                $key = "$($t.FullName)::$($m.Name)"
                                if (-not $strings.ContainsKey($key)) { $strings[$key] = @() }
                                if ($strings[$key] -notcontains $str) {
                                    $strings[$key] += $str
                                }
                            }
                        } catch {}
                    }
                }
            }
        } catch {}
    }
}
foreach ($key in $strings.Keys) {
    Write-Host "=== $key ==="
    foreach ($s in $strings[$key]) {
        Write-Host "  `"$s`""
    }
}
