import logging
import threading
import time

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"

logging.basicConfig(
    # filename='my.log',
    level=logging.INFO,
    format=LOG_FORMAT
)

import vits_helper
import llm
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
        t = t.replace("\n", "")
        logging.info("start tts: " + t)
        try:
            voice = vits_helper.tts(f"[ZH]{t}[ZH]", array=True)
            sounddevice.wait()  # block
            # time.sleep(1 / 60)
            sounddevice.play(voice[0], voice[1])
        except Exception as e:
            logging.warn(e)

        __text = text[stopped_i:]
    else:  # 保存供下一次生成
        __text = t


model = "neko:qwen2.5-1.5b"
history = []
while True:
    x = input(">>>")
    llm.chat_stream(model, x, history, on_response)
