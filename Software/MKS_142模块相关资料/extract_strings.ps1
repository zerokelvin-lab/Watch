$asm = [Reflection.Assembly]::LoadFile("$env:TEMP\app.exe")
$types = $asm.GetTypes()
$strings = @{}
foreach ($t in $types) {
    $methods = $t.GetMethods([Reflection.BindingFlags]::NonPublic -bor [Reflection.BindingFlags]::Public -bor [Reflection.BindingFlags]::Instance -bor [Reflection.BindingFlags]::Static -bor [Reflection.BindingFlags]::DeclaredOnly)
    foreach ($m in $methods) {
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
                            $strings[$key] += $str
                        }
                    } catch {}
                }
            }
        }
    }
}
foreach ($key in $strings.Keys) {
    Write-Host "=== $key ==="
    foreach ($s in $strings[$key]) {
        Write-Host "  `"$s`""
    }
}
