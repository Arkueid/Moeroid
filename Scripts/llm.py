from typing import Callable
import requests
import json
import logging
import time

logging.basicConfig(level=logging.DEBUG)

def chat(model_name: str, text: str, history: list[dict] = []) -> str:
    history.append({
                "role": "user",
                "content": text
            })
    data = {
        "model": model_name,
        "messages": history
        ,
        "stream": False
    }
    url = "http://127.0.0.1:11434/api/chat"
    start_time = time.time()
    rsp = requests.post(url, json=data).json()
    logging.info(f"llm time cost: {time.time() - start_time}s")
    logging.debug(rsp)
    msg = rsp['message']
    msg['content'] = msg['content']
    history.append(msg)
    return msg['content']

def chat_stream(model_name: str, text: str, history: list[dict] = [], on_response: Callable[[str], None] = None):
    history.append({
                "role": "user",
                "content": text
            })
    data = {
        "model": model_name,
        "messages": history,
        "stream": True
    }
    url = "http://127.0.0.1:11434/api/chat"
    start_time = time.time()
    rsp = requests.post(url, json=data)
    
    logging.info(f"llm time cost: {time.time() - start_time}s")
    complete_msg = ""
    for line in rsp.iter_lines():
        if not line:
            break
        d = json.loads(line)
        msg = d['message']['content']
        complete_msg += msg
        if on_response:
            on_response(msg)
    logging.debug("complete msg:" + complete_msg)
    history.append({
        "role": "assistant",
        "content": complete_msg
    })

if __name__ == "__main__":
    model = "neko:qwen2.5-0.5b"
    history = []
    while True:
        x = input(">>>")
        chat_stream(model, x, history, lambda msg: print(msg, end=''))