import numpy as np
import sys
import os
from tqdm import tqdm


def get_uint8_from_bytes(bytes, idx):
    '''
    Reads a uint8 values from a byte object

    :param bytes: bytes object

    :param idx: int
        the offset from which to read

    :return:
        tuple of form (idx + 1 = offset after reading the uint8, the uint8 value)
    '''
    return idx + 1, int.from_bytes(bytes[idx:idx+1], byteorder=sys.byteorder, signed=False)


def get_uint32_from_bytes(bytes, idx):
    '''
    Reads a uint32 values from a byte object

    :param bytes: bytes object

    :param idx: int
        the offset from which to read

    :return:
        tuple of form (idx + 4 = offset after reading the uint32, the uint32 value)
    '''
    return idx + 4, int.from_bytes(bytes[idx:idx+4], byteorder=sys.byteorder, signed=False)


def get_uint64_from_bytes(bytes, idx):
    '''
    Reads a uint64 values from a byte object

    :param bytes: bytes object

    :param idx: int
        the offset from which to read

    :return:
        tuple of form (idx + 8 = offset after reading the uint64, the uint64 value)
    '''
    return idx + 8, int.from_bytes(bytes[idx:idx+8], byteorder=sys.byteorder, signed=False)


def get_string_from_bytes(bytes, idx, length):
    '''
    Reads a int8 values from a byte object

    :param bytes: bytes object

    :param idx: int
        the offset from which to read

    :param length: int
        number of bytes to consider for the string

    :return:
        tuple of form (idx + length = offset after reading the @length bytes, the string)
    '''
    return idx + length, bytes[idx:idx + length].decode('utf-8')


def get_np_array_from_bytes(bytes, idx, length, element_size, data_type):
    '''
    Reads a numpy array from a set of bytes

    :param bytes: bytes object

    :param idx:  int
        the offset from which to read

    :param length: int
        number of elements to read

    :param element_size: int
        size of one element

    :param data_type: 0 or 1
        if 0 it will read np.float32 values
        if 1 it will read np.uint8 values

    :return:
        tuple of form (idx + @element_size * @length, the np array read)
    '''
    if data_type == 0:
        array_type = np.float32
    elif data_type == 1:
        array_type = np.uint8
    # WAERNING need to update for more types
    n_bytes = element_size * length
    return idx + n_bytes, np.frombuffer(bytes[idx:idx + n_bytes], dtype=array_type)


def parse_array_file(array_file):
    '''
    Parses a binary file that was saved with the @array_dump function in the Simulator C code
    :param array_file: str
        absolute path to a binary files created with @array_dump

    :return: dict, with keys
        name: name of the data
        data: numpy array 1D representing the data
    '''
    with open(array_file, 'rb') as file:
        bytes = file.read()

    idx, data_name_lenght = get_uint32_from_bytes(bytes, 0)
    idx, data_name = get_string_from_bytes(bytes, idx, data_name_lenght)
    idx, length = get_uint32_from_bytes(bytes, idx)
    idx, element_size = get_uint64_from_bytes(bytes, idx)
    idx, data_type = get_uint8_from_bytes(bytes, idx)
    _, data = get_np_array_from_bytes(bytes, idx, length, element_size, data_type)

    return dict(name=data_name, data=data)


def get_binaries_for_layers(layers_folder, layers, prefix, log=True):
    '''
    Returns a set of binaries for each layer found in a folder

    :param layers_folder: str
        path to the folder containing the binaries for each layer

    :param layers: list of str
        each element is the name of a layer

    :param prefix: str
        used to select a subset of files (ex: only the voltages)

    :param log: bool, default True
        if it should use tqdm to display the progres

    :return: list
        every element is a dictionary with keys
            layer_name: name of the layer
            binaries: list of the the binaries that have the @prefix
    '''
    binaries_for_layer = list()

    if log is True:
        layers_r = tqdm(layers)
    else:
        layers_r = layers

    for layer_name in layers_r:
        layer_folder = os.path.join(layers_folder, layer_name)

        # get the files that have the prefix
        binaries = [f for f in os.listdir(layer_folder) if f.endswith('.bin')]
        binaries_with_prefix = [f for f in binaries if prefix in f]
        binaries_for_layer.append(dict(layer_name=layer_name, binaries=binaries_with_prefix))

    return binaries_for_layer