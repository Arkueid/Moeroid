import logging
import struct
import config
import os

cd = os.path.split(__file__)[0]

LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"
logging.basicConfig(filename=os.path.join(cd, "log.txt"), level=logging.INFO, format=LOG_FORMAT, encoding="utf-8")

import vits_helper
import llm
import sys
import sounddevice
import time
import configparser

cfg = configparser.ConfigParser()
cfg.read(os.path.join(cd, "cfg.ini"))

lan = config.CN
lan_val = cfg.getint("common", "language")
if lan_val == config.CN[0]:
    lan = config.CN
elif lan_val == config.JP[0]:
    lan = config.JP
elif lan_val == config.EN[0]:
    lan = config.EN
else:
    logging.warning(f"invalid lan_val: {lan}")
logging.info(f"current lan: {lan}")

# model = "neko:qwen2.5-0.5b"
# model = "neko:gemma2-2b"
model = "neko:qwen2.5-1.5b"

history = []

__text = ""

puctuation = "！。；，？" + ",.;!?"


vits_helper.init_onnx_model()
logging.info("task start")

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
            # -- tts start--
            voice = vits_helper.tts(f"{lan[1]}{t}{lan[1]}", array=True, both=True)
            # -- tts end--
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


def processChat():
    length_bytes = sys.stdin.buffer.read(8)
    size = struct.unpack("Q", length_bytes)[0]
    text = sys.stdin.buffer.read(size).decode("utf-8")
    logging.info("received: %s" % text)

    try:
        text = f"请用{lan[2]}回复：{text}"
        llm.chat_stream(model, text, history, on_response)
    except Exception as e:
        on_response("抱歉，大模型宕机了...")
        on_response("")


def processSetLan():
    global lan

    lan_bytes = sys.stdin.buffer.read(1)
    lan_val = struct.unpack("B", lan_bytes)[0]
    if lan_val == config.CN[0]:
        lan = config.CN
    elif lan_val == config.JP[0]:
        lan = config.JP
    elif lan_val == config.EN[0]:
        lan = config.EN
    else:
        logging.warning(f"invalid lan_val: {lan_val}")

    logging.info(f"current lan: {lan}")
    cfg.set("common", "language", str(lan[0]))
    with open(os.path.join(cd, "cfg.ini"), "w") as f:
        cfg.write(f)

while True:
    try:
        op_bytes = sys.stdin.buffer.read(1)
        op = struct.unpack("B", op_bytes)[0]
        logging.info(f"op: {op_bytes}")
        if op == 0b00000000:
            processChat()
        elif op == 0b10000000:
            processSetLan()
    except Exception as e:
        logging.error(e)
        continue

    
