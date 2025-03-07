# Script for Python tts/llm subprocess

Data, such as `.wav` file bytes for lipsync and llm output text are transfered throught pipe between parent process `Moeroid.exe` and child process `path/to/python.exe path/to/main_subprocess`.  

TTS uses `vits-onnx` models. You can convert your vits model using `export.bat` and test the onnx model using `infer.bat`.   