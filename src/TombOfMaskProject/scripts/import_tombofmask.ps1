<#
import_tombofmask.ps1

用途: 将本地独立创建的 Visual Studio 项目拷贝到已克隆的本地仓库，并可选择自动 git 提交/推送。

使用方法:
  1. 打开 PowerShell（建议以用户身份运行）。
  2. 在运行时可传入参数，或使用脚本内默认路径。
  3. 示例（交互模式）:
     cd "C:\Users\U\OneDrive\文档\GitHub\404-Not-Found"
     .\scripts\import_tombofmask.ps1 -RepoPath 'C:\Users\U\OneDrive\文档\GitHub\404-Not-Found' -ProjectPath 'C:\Users\U\OneDrive\Desktop\TombOfMaskProject' -TargetSubdir 'src\TombOfMaskProject'

   示例（自动、无交互）:
     powershell -NoExit -ExecutionPolicy Bypass -File "C:\Users\U\OneDrive\文档\GitHub\404-Not-Found\scripts\import_tombofmask.ps1" -RepoPath 'C:\Users\U\OneDrive\文档\GitHub\404-Not-Found' -ProjectPath 'C:\Users\U\OneDrive\Desktop\TombOfMaskProject' -TargetSubdir 'src\TombOfMaskProject' -Auto

注意: 脚本不会覆盖目标仓库已有文件（会合并）。脚本会排除常见的构建/用户目录：bin, obj, .vs, *.user, *.suo
#>

param(
    [string]$RepoPath = 'C:\Users\U\OneDrive\文档\GitHub\404-Not-Found',
    [string]$ProjectPath = 'C:\Users\U\OneDrive\Desktop\TombOfMaskProject',
    [string]$TargetSubdir = 'src\\TombOfMaskProject',
    [switch]$Auto,
    [string]$DefaultCommitMessage = 'Import TombOfMaskProject into repo'
)

function Confirm-Or-Exit($msg) {
    if ($Auto) { return }
    $r = Read-Host "$msg (y/n)"
    if ($r -ne 'y' -and $r -ne 'Y') {
        Write-Host '已取消。' -ForegroundColor Yellow
        exit 0
    }
}

if (-not (Test-Path $RepoPath)) {
    Write-Host "目标仓库路径不存在: $RepoPath" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $ProjectPath)) {
    Write-Host "项目路径不存在: $ProjectPath" -ForegroundColor Red
    exit 1
}

$FullTarget = Join-Path -Path $RepoPath -ChildPath $TargetSubdir

Write-Host "仓库路径: $RepoPath"
Write-Host "项目路径: $ProjectPath"
Write-Host "目标子目录 (仓库内): $TargetSubdir"

Confirm-Or-Exit '确认将项目拷贝到上述目标位置并继续？'

# 创建目标目录
if (-not (Test-Path $FullTarget)) {
    New-Item -ItemType Directory -Path $FullTarget | Out-Null
}

# 使用 robocopy 进行复制并排除常见目录/文件
$excludes = @('bin','obj','.vs','packages','TestResults')

Write-Host '开始复制项目文件（会排除 bin/ obj/ .vs/ 等）...'

# robocopy 命令: /E 递归拷贝子目录
$robocopyCmd = "robocopy `"$ProjectPath`" `"$FullTarget`" /E /COPY:DAT /R:2 /W:2 /NFL /NDL"

# 添加排除目录参数
foreach ($d in $excludes) { $robocopyCmd += " /XD `"$ProjectPath\\$d`"" }

Write-Host $robocopyCmd
Invoke-Expression $robocopyCmd

Write-Host '复制完成。'

# 如果仓库里没有 .gitignore，则写入一个简单的 Visual Studio 模板
$gitignorePath = Join-Path $RepoPath '.gitignore'
if (-not (Test-Path $gitignorePath)) {
    Write-Host '仓库没有 .gitignore，正在创建 Visual Studio 常用模板...'
    $vsIgnore = @'
# Build Folders
bin/
obj/

# Visual Studio
.vs/
*.suo
*.user
*.userosscache
*.sln.docstates

# Rider
.idea/

# NuGet
packages/
*.nupkg

# Build results
TestResults/

'@
    $vsIgnore | Out-File -FilePath $gitignorePath -Encoding utf8
    Write-Host '.gitignore 已创建。' -ForegroundColor Green
} else {
    Write-Host '.gitignore 已存在，跳过创建。' -ForegroundColor Yellow
}

# 显示要提交的变更并选择是否提交
Write-Host '下面会显示 git status 的摘要（仅列前 200 行）...'
try {
    git -C $RepoPath status --porcelain | Select-Object -First 200 | ForEach-Object { Write-Host $_ }
} catch {
    Write-Host '无法运行 git status（请确保已安装并配置 Git）。' -ForegroundColor Yellow
}

Confirm-Or-Exit '确认要执行 git add/commit 并推送到远程吗？'

if ($Auto) {
    $commitMsg = $DefaultCommitMessage
} else {
    $commitMsg = Read-Host '输入本次提交信息（默认: "Import TombOfMaskProject into repo"）'
    if ([string]::IsNullOrWhiteSpace($commitMsg)) { $commitMsg = $DefaultCommitMessage }
}

Write-Host '执行 git add...'
try { git -C $RepoPath add . } catch { Write-Host 'git add 失败。' -ForegroundColor Red }

Write-Host '执行 git commit...'
# allow-empty ensures script doesn't fail if nothing changed
try { git -C $RepoPath commit -m "$commitMsg" --allow-empty } catch { Write-Host 'git commit 可能没有可提交的更改或失败。' -ForegroundColor Yellow }

Write-Host '准备推送到远程（使用默认远程/分支）...'
Confirm-Or-Exit '确认推送？'

try {
    git -C $RepoPath push
    Write-Host '推送完成。' -ForegroundColor Green
} catch {
    Write-Host "推送失败: $_" -ForegroundColor Red
    Write-Host '请确认远程仓库配置与凭据。' -ForegroundColor Yellow
}

Write-Host '全部完成。' -ForegroundColor Green
