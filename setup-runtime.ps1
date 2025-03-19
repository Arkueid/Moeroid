# install-and-pack.ps1
$pythonDir = ".\Runtime"
$pythonZip = "python-3.12.8-embed-amd64.zip"

# 下载并解压
Invoke-WebRequest -Uri "https://www.python.org/ftp/python/3.12.8/python-3.12.8-embed-amd64.zip" -OutFile $pythonZip
Expand-Archive -Path $pythonZip -DestinationPath $pythonDir

# 启用 pip
((Get-Content "$pythonDir\python312._pth") -replace "#import site","import site") | Set-Content "$pythonDir\python312._pth"

# 安装 pip
Invoke-WebRequest -Uri "https://bootstrap.pypa.io/get-pip.py" -OutFile "$pythonDir\get-pip.py"

.\Runtime\python.exe .\Runtime\get-pip.py

# 安装库
.\Runtime\python.exe -m pip install -r .\Scripts\requirements.txt

# 移除被依赖但未使用的包
.\Runtime\python.exe -m pip uninstall sympy -y

# 下载 onnx 模型
Invoke-WebRequest -Uri "https://github.com/Arkueid/Moeroid/releases/download/onnx-model/gal_cj.onnx" -OutFile .\Scripts\models\gal_cj.onnx
# 打包
# Compress-Archive -Path $pythonDir -DestinationPath "python-portable.zip"