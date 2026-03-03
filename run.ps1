# 检查参数
if ($args.Count -eq 0) {
    Write-Host "Usage: script.ps1 <testcase_group_name>"
    Write-Host "Available testcase groups from config.json:"
    $groups = (Get-Content -Path "testcases\config.json" | ConvertFrom-Json).Groups
    $groups | ForEach-Object { Write-Host "  $_.GroupName" }
    exit 1
}

# 检查 config.json 文件是否存在
if (-not (Test-Path -Path "testcases\config.json")) {
    Write-Host "./testcases/config.json does not exist, please extract testcases to that directory."
    exit 1
}

# 检查 jq 是否安装 (通过检查 PowerShell 是否可以正确解析 JSON)
try {
    $config = Get-Content -Path "testcases\config.json" | ConvertFrom-Json
} catch {
    Write-Host "Failed to parse config.json. Ensure it's a valid JSON file."
    exit 1
}

# 检查是否有对应的 testcase group
$groupName = $args[0]
$group = $config.Groups | Where-Object { $_.GroupName -eq $groupName }

if (-not $group) {
    Write-Host "Testcase group '$groupName' not found in config.json"
    Write-Host "Available testcase groups:"
    $config.Groups | ForEach-Object { Write-Host "  $_.GroupName" }
    exit 1
}

# 获取测试点列表
$list = $group.TestPoints

# 检查编译好的可执行文件
$exeName = "code.exe"
if (-not (Test-Path -Path $exeName)) {
    Write-Host "Please compile the ticket system and place the executable at '$exeName'"
    exit 1
}

# 创建临时文件
function Get-TempFile {
    return [System.IO.Path]::GetTempFileName()
}

# 创建临时目录
function Get-TempDir {
    $tempDir = [System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), "ticket" + [System.Guid]::NewGuid().ToString())
    New-Item -Path $tempDir -ItemType Directory
    return $tempDir
}

# 设置临时目录
$testDir = Get-TempDir
Copy-Item -Path $exeName -Destination $testDir
$exePath = Join-Path -Path $testDir -ChildPath $exeName
$cwd = Get-Location
$baseDir = Join-Path -Path $cwd -ChildPath "testcases"

# 切换到临时目录
Set-Location -Path $testDir

# 运行每个测试点
foreach ($i in $list) {
    $inputFile = Join-Path -Path $baseDir -ChildPath "$i.in"
    $outputFile = Join-Path -Path $testDir -ChildPath (Get-TempFile)
    $expectedOutputFile = Join-Path -Path $baseDir -ChildPath "$i.out"
    
    Write-Host "About to run input #$i..."

    # 读取输入文件内容
    $inputContent = Get-Content -Path $inputFile

    # 执行程序并获取输出
    $outputContent = & $exePath $inputContent

    # 将程序输出写入临时输出文件
    Set-Content -Path $outputFile -Value $outputContent

    # 比较输出
    $diffFile = Get-TempFile
    $diffResult = Compare-Object -ReferenceObject (Get-Content -Path $outputFile) -DifferenceObject (Get-Content -Path $expectedOutputFile)

    if ($diffResult) {
        $diffResult | Select-Object -First 5 | ForEach-Object { Write-Host $_ }
        $backup = "test-$groupName-$i-$(Get-Date -Format 'yyyyMMddHHmmss').log"
        Copy-Item -Path $outputFile -Destination (Join-Path -Path $cwd -ChildPath $backup)
        Write-Host "Test failed on input #$i."
        Write-Host "Output saved to $backup"
        exit 1
    }

    Remove-Item -Path $outputFile
    Remove-Item -Path $diffFile
}


# 清理临时目录
Remove-Item -Path $testDir -Recurse -Force
Write-Host "Testcase complete, answer correct."
