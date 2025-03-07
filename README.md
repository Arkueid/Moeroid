# Moeroid

低配 AI 桌宠，全 cpu 推理。

测试设备：Intel core i3 + 8GB

* 大模型：ollama qwen2.5:1.5b（对于常驻桌面，回答日常对话应该是绰绰有余了）
    * 使用 `requests` 库对本地 ollama 发起请求
* 文本转语音：vits onnx
    * 将 CjangCjengh 大佬发布的 [vits 模型](https://sjtueducn-my.sharepoint.com/:u:/g/personal/cjang_cjengh_sjtu_edu_cn/EQ0IKHchgzZAt0E6GryW17EBsIlIkmby6BcO9FtoODjwNQ?e=5uzWtj) 转为 onnx 模型
* Live2D: 
    * CubismNativeSDK + Qt，对于所有纹理总像素在 1024x1024 左右的模型，运行内存占用在 50M 左右
    * [降低 Live2D 模型运行内存占用的方法](https://github.com/Arkueid/live2d-py/issues/47)


主要思路：C++主进程 + Python子进程 + 管道通信，llm 流式输出 + tts "片段式" 生成

## 详细

桌宠主程序，即界面程序 `Moeroid.exe` 启动时，会执行 `moe.config.json` 中的 `command`，创建 Python 子进程。

子进程用于与 ollama 的大模型进行对话（使用 `requests` 库发起http请求），流式生成的文本片段会在子进程中使用 vits-onnx 模型生成语音，在子进程直接调用音频设备播放，同时将音频数据通过父子进程间的管道传输至 `Moeroid.exe` 进程进行 Live2D 模型的口型同步，也将大模型的生成文本通过管道返回至界面进行显示。

语音播放是阻塞式的，只有上一条语音播放完毕才会继续播放，语音播放和 `tts/llm输出` 是并行的。

tts 的文本是按照 `逗号、句号` 等将句子分割后相对完整的片段，实测一般情况下每个片段可以在 1~2s 内生成完毕，由于上一个文本片段并行播放，可以实现用户说一句话，桌宠几乎无延时地连续讲几段话。

以上是低配置的情况，在高配置下完全可以到达更高的速度，也能使用参数量更大的模型以达到更好的对话效果。