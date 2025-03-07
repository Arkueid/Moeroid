import logging
import struct

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"
logging.basicConfig(filename="log.txt", level=logging.INFO, format=LOG_FORMAT, encoding="utf-8")

import vits_helper
import llm
import sys
import sounddevice
import time

vits_helper.init_onnx_model()

logging.info("task start")

__text = ""

puctuation = ";!?！。；，？"


def on_response(text: str):
    global __text
    
    if text == "" and __text == "":
        sys.stdout.buffer.write(struct.pack("Q", 0))
        sys.stdout.buffer.flush()
        return

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
            voice = vits_helper.tts(f"[ZH]{t}[ZH]", array=True, both=True)
            time.sleep(0.9)
            sounddevice.wait()  # block
            text_bytes = t.encode("utf-8")
            length_bytes = struct.pack("Q", len(text_bytes))
            sys.stdout.buffer.write(length_bytes)
            sys.stdout.buffer.write(text_bytes)

            audio_bytes: bytes = voice[1].getvalue()
            length_bytes = struct.pack("Q", len(audio_bytes))
            sys.stdout.buffer.write(length_bytes)
            sys.stdout.buffer.write(audio_bytes)
            sys.stdout.buffer.flush()
            sounddevice.play(voice[0][0], voice[0][1])
            voice[1].close()

            if text == "":
                sys.stdout.buffer.write(struct.pack("Q", 0))
                sys.stdout.buffer.flush()
        except Exception as e:
            logging.error(e)

        __text = text[stopped_i:]
    else:  # 保存供下一次生成
        __text = t


# model = "neko:qwen2.5-1.5b"
model = "neko:qwen2.5-1.5b"
history = []
while True:
    try:
        length_bytes = sys.stdin.buffer.read(8)
        size = struct.unpack("Q", length_bytes)[0]
        text = sys.stdin.buffer.read(size).decode("utf-8")
        logging.info("received: %s" % text)
    except Exception as e:
        logging.error(e)
        continue

    try:
        llm.chat_stream(model, text, history, on_response)
    except Exception as e:
        on_response("抱歉，大模型宕机了...")
        on_response("")
