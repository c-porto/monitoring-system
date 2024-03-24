from enum import Enum
from req import get_measurements_data
import numpy as np

class Metrics(Enum):
    AIR = 0
    UV = 1
    TEMP = 2
    HM = 3
    TIME = 4

def get_data_statistics(endpoint: str):
    temp_arr = np.zeros(100,dtype=np.float64)
    hm_arr = np.zeros(100,dtype=np.float64)
    air_arr = np.zeros(100,dtype=np.float64)
    light_arr = np.zeros(100,dtype=np.float64)

    samples: list[dict] | None = get_measurements_data(endpoint)

    if samples == None:
        print('Failed to get measurement data from server')
        return

    for idx,sample in enumerate(samples):
        temp_arr[idx] = sample['temp']
        hm_arr[idx] = sample['humidity']
        light_arr[idx] = sample['light']
        air_arr[idx] = sample['air']

    print('\033[38;5;168mPrinting last 100 samples statistics...\033[0m\n')

    print(f'\033[38;5;212mTemperature:\033[0m\n\t\033[38;5;219mMean:\033[0m {temp_arr.mean()}\n\t\033[38;5;219mDeviation:\033[0m {temp_arr.std()}\n\t\033[38;5;219mMedian:\033[0m {np.median(temp_arr)}\n')
    print(f'\033[38;5;212mHumidity:\033[0m\n\t\033[38;5;219mMean:\033[0m {hm_arr.mean()}\n\t\033[38;5;219mDeviation:\033[0m {hm_arr.std()}\n\t\033[38;5;219mMedian:\033[0m {np.median(hm_arr)}\n')
    print(f'\033[38;5;212mUltraviolet Irradiance:\033[0m\n\t\033[38;5;219mMean:\033[0m {light_arr.mean()}\n\t\033[38;5;219mDeviation:\033[0m {light_arr.std()}\n\t\033[38;5;219mMedian:\033[0m {np.median(light_arr)}\n')
    print(f'\033[38;5;212mCO2:\033[0m\n\t\033[38;5;219mMean:\033[0m {air_arr.mean()}\n\t\033[38;5;219mDeviation:\033[0m {air_arr.std()}\n\t\033[38;5;219mMedian:\033[0m {np.median(air_arr)}\n')


def prepare_samples_for_plotting(endpoint: str) -> dict[Metrics,list] | None:
    ret_dict: dict[Metrics,list] = {}
    time_arr: list[str] = []
    temp_arr: list[np.float64] = []
    hm_arr: list[np.float64] = []
    air_arr: list[np.float64] = []
    light_arr: list[np.float64] = []

    samples: list[dict] | None = get_measurements_data(endpoint)

    if samples == None:
        print('Failed to get measurement data from server')
        return None

    for sample in samples:
        temp_arr.append(sample['temp'])
        hm_arr.append(sample['humidity'])
        light_arr.append(sample['light'])
        air_arr.append(sample['air'])
        time_arr.append(sample['time'])

    ret_dict[Metrics.TEMP] = temp_arr
    ret_dict[Metrics.HM] = hm_arr
    ret_dict[Metrics.UV] = light_arr
    ret_dict[Metrics.AIR] = air_arr
    ret_dict[Metrics.TIME] = time_arr

    return ret_dict
