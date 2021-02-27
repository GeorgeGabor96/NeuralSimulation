import numpy as np
import sys


def get_int32_from_bytes(bytes, idx):
    return idx + 4, int.from_bytes(bytes[idx:idx+4], byteorder=sys.byteorder)

def get_int64_from_bytes(bytes, idx):
    return idx + 8, int.from_bytes(bytes[idx:idx+8], byteorder=sys.byteorder)

def get_int8_from_bytes(bytes, idx):
    return idx + 1, bytes[idx]

def get_string_from_bytes(bytes, idx, length):
    return idx + length, bytes[idx:idx + length].decode('utf-8')

def get_np_array_from_bytes(bytes, idx, length, element_size, data_type):
    if data_type == 0:
        array_type = np.float32
    elif data_type == 1:
        array_type = np.uint8
    # WAERNING need to update for more types
    n_bytes = element_size * length
    return idx + n_bytes, np.frombuffer(bytes[idx:idx + n_bytes], dtype=array_type)


def parse_array_file(array_file):

    with open(array_file, 'rb') as file:
        bytes = file.read()

    idx, data_name_lenght = get_int32_from_bytes(bytes, 0)
    idx, data_name = get_string_from_bytes(bytes, idx, data_name_lenght)
    idx, length = get_int32_from_bytes(bytes, idx)
    idx, element_size = get_int64_from_bytes(bytes, idx)
    idx, data_type = get_int8_from_bytes(bytes, idx)
    _, data = get_np_array_from_bytes(bytes, idx, length, element_size, data_type)

    return dict(name=data_name, data=data)
