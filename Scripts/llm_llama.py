from typing import Callable
import requests
import json
import logging
import time

logging.basicConfig(level=logging.DEBUG)


url = "http://127.0.0.1:1234/v1/chat/completions"

def chat_stream(text: str, history: list[dict] = [], on_response: Callable[[str], None] = None):
    history.append({
                "role": "user",
                "content": text
            })
    data = {
        "messages": history,
        "stream": True,
    }
    start_time = time.time()
    rsp = requests.post(url, json=data, stream=True)
    
    logging.info(f"llm time cost: {time.time() - start_time}s")
    complete_msg = ""
    for line in rsp.iter_lines():
        if line:
            line = line.decode('utf-8').replace('data: ', '')
            d = json.loads(line)
            logging.debug(d)

            msg = d['choices'][0]['delta'].get('content', None)
            if msg is not None:
                complete_msg += msg
            else:
                break
            if on_response:
                on_response(msg)
    logging.info("complete msg:" + complete_msg)
    on_response(None)
    history.append({
        "role": "assistant",
        "content": complete_msg
    })

if __name__ == "__main__":
    # import config
    history = []
    while True:
        x = input(">>>")
        chat_stream(x, history, lambda msg: print(msg, end='') if msg is not None else print())
        # chat(x, history)