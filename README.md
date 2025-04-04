<div align="center" style="padding-top: 10px">
    <h1>
        <img src="./Resources/16x16.ico" alt="koe" width="30px" height="30px">
        Moeroid
    </h1>
</div>

![](https://img.shields.io/badge/Qt-41CD52?logo=qt&logoColor=white)
![](https://img.shields.io/badge/OpenGL-5586A4?logo=opengl&logoColor=white&style=flat)
![](https://img.shields.io/badge/C++-00599C?logo=c%2B%2B)
![](https://img.shields.io/badge/CMake-064F8C?logo=cmake)

低配 AI 桌宠，全 cpu 推理。

测试设备：Intel core i3 + 8GB

* 大模型：使用 llama.cpp + qwen2.5-3b-instruct-q4_k_m.gguf
    * 使用 `requests` 库对本地 llama-server 发起请求
* 文本转语音：vits onnx
    * 将 CjangCjengh 大佬发布的 [vits 模型](https://sjtueducn-my.sharepoint.com/:u:/g/personal/cjang_cjengh_sjtu_edu_cn/EQ0IKHchgzZAt0E6GryW17EBsIlIkmby6BcO9FtoODjwNQ?e=5uzWtj) 转为 onnx 模型
* Live2D: 
    * CubismNativeSDK + Qt，对于所有纹理总像素在 1024x1024 左右的模型，运行内存占用在 50M 左右
    * [降低 Live2D 模型运行内存占用的方法](https://github.com/Arkueid/live2d-py/issues/47)


主要思路：C++主进程 + Python子进程 + 管道通信，llm 流式输出 + tts 拆分生成