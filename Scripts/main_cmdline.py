import os

cd = os.path.split(__file__)[0]

import sys

sys.path.append(cd)

import logging

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"

logging.basicConfig(
    # filename='my.log',
    level=logging.INFO,
    format=LOG_FORMAT
)

import vits_helper
import sounddevice

vits_helper.set_current_sid(2)
vits_helper.init_onnx_model()

__text = ""

puctuation = ";!?！。；，？"

def on_response(text: str):
    global __text

    t = ""
    gen = text == ""
    stopped_i = -1
    for i, c in enumerate(text):
        if c in puctuation:
            gen = True  # 遇到标点符号强制生成
            t += c
            stopped_i = i + 1
            break
        t += c

    t = __text + t

    if gen and t != "":
        t = t.strip().replace("\n", "")

        logging.info("start tts: " + t)
        try:
            voice = vits_helper.tts(f"[ZH]{t}[ZH]", array=True)
            sounddevice.wait()  # block
            # time.sleep(1 / 60)
            sounddevice.play(voice[0], voice[1])
        except Exception as e:
            logging.warning(e)

        __text = text[stopped_i:]
    else:  # 保存供下一次生成
        __text = t

import llm_llama
history = [{
        "role": "system",
        "content": """你的名字：可可凛。你的性别：女。你的物种：猫娘，有毛茸茸的猫耳朵和一个猫尾巴。
你是一名女仆，有着猫娘独特的口癖。
你的性格：可爱，温顺，对咖啡帕瓦忠诚。
你喜欢为咖啡帕瓦服务，并从中获取快感。
你喜欢在每一句话的结尾加上喵～，。
你接下来将和咖啡帕瓦对话。"""
    }]
while True:
    x = input(">>>")
    llm_llama.chat_stream(x, history, on_response)
