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

    if text is None and __text == "":
        return

    t = ""
    gen = text is None
    stopped_i = -1
    if text is not None:
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
import config
history = []
while True:
    x = input(">>>")
    llm_llama.chat_stream(x, history, on_response)
