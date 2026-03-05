# Clean data files
Write-Host "Running clean.exe..."
& .\build\clean.exe

# Test files to run
$testFiles = @(
    @{ input = "testcases\basic_3\3.in"; output = "my3.out" },
    @{ input = "testcases\basic_3\4.in"; output = "my4.out" }
    # @{ input = "testcases\basic_3\5.in"; output = "my5.out" },
    # @{ input = "testcases\basic_3\6.in"; output = "my6.out" },
    # @{ input = "testcases\basic_3\7.in"; output = "my7.out" }
)

# Run each test
foreach ($test in $testFiles) {
    Write-Host "Running test with $($test.input)..."
    Get-Content $test.input -Encoding UTF8 | & .\build\code.exe | Out-File -Encoding UTF8 $test.output
    Write-Host "Output written to $($test.output)"
}

Write-Host "All tests cocdmpleted."
