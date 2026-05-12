$asm = [Reflection.Assembly]::LoadFile("$env:TEMP\app.exe")
$types = $asm.GetTypes()

$keyMethods = @('MatchProtocol', 'AnalysisProtocol', 'SaveTJPack', 'serialPort1_DataReceived',
                'versionFiltration', 'isHave255', 'isAllZero', 'initPackageIndex',
                'ShowReport', 'InitRecData', 'initVersionFiltration',
                'TimerShowWave', 'sysTimer_Tick',
                'ShowRRNewImage', 'changeColor', 'RefurbishArray', 'ShowBloodstate',
                'createCSVTitle', 'savedata', 'getHistoryData', 'postdata', 'refreshData',
                'getalldata', 'getHeartRate', 'getSpo2', 'makeDataV2', 'makeData',
                'Button5_history_Click', 'button1_Click', 'button2_Click', 'button3_Click',
                'button4_Click', 'button6_Click', 'packsavepostThread',
                'TimerShowWave', 'timer1_Tick', 'Form1_Load', 'ClearMemory')

$results = @{}
foreach ($t in $types) {
    $methods = $t.GetMethods([Reflection.BindingFlags]::NonPublic -bor [Reflection.BindingFlags]::Public -bor [Reflection.BindingFlags]::Instance -bor [Reflection.BindingFlags]::Static -bor [Reflection.BindingFlags]::DeclaredOnly)
    foreach ($m in $methods) {
        if ($m.Name -in $keyMethods) {
            $body = $m.GetMethodBody()
            if ($body) {
                $il = $body.GetILAsByteArray()
                $hex = ($il | ForEach-Object { $_.ToString('X2') }) -join ' '
                $typeName = $t.FullName
                $params = ($m.GetParameters() | ForEach-Object { "$($_.ParameterType.Name) $($_.Name)" }) -join ', '
                $locals = @()
                foreach ($loc in $body.LocalVariables) {
                    $locals += "$($loc.LocalType.Name) loc$($loc.LocalIndex)"
                }
                $localsStr = $locals -join ', '
                $key = "$typeName::$($m.Name)"
                Write-Host "METHOD:$key"
                Write-Host "RETURN:$($m.ReturnType.Name)"
                Write-Host "PARAMS:$params"
                Write-Host "LOCALS:$localsStr"
                Write-Host "IL:$hex"
                Write-Host "---"
            }
        }
    }
}
