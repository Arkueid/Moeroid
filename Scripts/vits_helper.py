# Copyright (c) 2022, Yongqiang Li (yongqiangli@alumni.hust.edu.cn)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from io import BytesIO
from typing import Any

from numpy import signedinteger, ndarray, dtype
from numpy._typing import _16Bit

from vits.text import text_to_sequence
import numpy as np
from scipy.io import wavfile
import torch
import json
import vits.commons as commons
import vits.utils as utils
import sys
import io
import logging

try:
    import onnxruntime as ort
except ImportError:
    logging.error('Please install onnxruntime!')
    sys.exit(1)


def get_symbols_from_json(path):
    import os
    assert os.path.isfile(path)
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data['symbols']


import os

__current_dir = os.path.split(__file__)[0]

__current_sid = 2
__model_name = "gal_cj"
__current_config = os.path.join(__current_dir, f"models/{__model_name}.json")
__current_model = os.path.join(__current_dir, f"models/{__model_name}.onnx")


def set_current_model(model_name: str):
    global __current_config
    global __current_model
    global __model_name

    __model_name = model_name
    __current_config = os.path.join(__current_dir, f"models/{__model_name}.json")
    __current_model = os.path.join(__current_dir, f"models/{__model_name}.onnx")

def set_current_sid(sid: int):
    global __current_sid
    __current_sid = sid


__ort_sess = None

def init_onnx_model():
    global __ort_sess
    __ort_sess = ort.InferenceSession(__current_model)
    logging.info("onnx model initialized")

def tts(text: str, array=False, both=False):
    hps = utils.get_hparams_from_file(__current_config)

    sid = __current_sid

    logging.info(f"model: {__model_name}, speaker: {hps.speakers[sid]}")

    seq = text_to_sequence(text, symbols=hps.symbols, cleaner_names=hps.data.text_cleaners)
    
    if hps.data.add_blank:
        seq = commons.intersperse(seq, 0)

    with torch.no_grad():
        # use numpy to replace torch
        x = np.array([seq], dtype=np.int64)
        x_len = np.array([x.shape[1]], dtype=np.int64)
        sid = np.array([sid], dtype=np.int64)
        scales = np.array([0.667, 0.8, 1], dtype=np.float32)
        scales.resize(1, 3)

        ort_inputs = {
            'input': x,
            'input_lengths': x_len,
            'scales': scales,
            'sid': sid
        }

        import time
        start_time = time.perf_counter()
        audio = np.squeeze(__ort_sess.run(None, ort_inputs))
        audio *= 32767.0 / max(0.01, np.max(np.abs(audio))) * 0.6
        audio = np.clip(audio, -32767.0, 32767.0).astype(np.int16)
        end_time = time.perf_counter()
        logging.info(f"tts time cost: {end_time - start_time} s")

        if array and not both:
            return audio, hps.data.sampling_rate
        
        buffer = io.BytesIO()
        wavfile.write(buffer,
                    hps.data.sampling_rate, audio)
        if not both:
            with buffer:
                return buffer.getvalue()
        
        return (audio, hps.data.sampling_rate), buffer
