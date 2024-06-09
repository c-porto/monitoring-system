from enum import Enum
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

from stats import Metrics, prepare_samples_for_plotting

VISUALS_REFRESH_PERIOD_SEC = 500

def plot_sample_dist(samples: dict[Metrics, list]):
    temp = np.array(samples[Metrics.TEMP])
    hm = np.array(samples[Metrics.HM])
    uv = np.array(samples[Metrics.UV])

    fig, axs = plt.subplots(1, 3, figsize=(12, 8))

    sb_plot_1 = axs[0]
    sb_plot_2 = axs[1]
    sb_plot_3 = axs[2]

    sb_plot_1.hist(temp, bins='auto', color='crimson', edgecolor='k')
    sb_plot_1.set_xlabel('Temperature (°C)')
    sb_plot_1.set_ylabel('Frequency')
    sb_plot_1.set_title('Temperature Distribution')
    sb_plot_1.tick_params(axis='x', rotation=45)
    sb_plot_1.grid(True, color='gray', zorder=0)

    sb_plot_2.hist(hm, bins='auto', color='azure', edgecolor='k')
    sb_plot_2.set_xlabel('Humidity (%)')
    sb_plot_2.set_ylabel('Frequency')
    sb_plot_2.set_title('Humidity Distribution')
    sb_plot_2.tick_params(axis='x', rotation=45)  
    sb_plot_2.grid(True, color='gray', zorder=0)

    sb_plot_3.hist(uv, bins='auto', color='indigo', edgecolor='k')
    sb_plot_3.set_xlabel('Ultraviolet Irradiance (mW/m²)')
    sb_plot_3.set_ylabel('Frequency')
    sb_plot_3.set_title('Ultraviolet Irradiance Distribution')
    sb_plot_3.tick_params(axis='x', rotation=45)  
    sb_plot_3.grid(True, color='gray', zorder=0)

    plt.tight_layout()

    return fig

def plot_sample_graph(samples: dict[Metrics, list]):
    time = samples[Metrics.TIME]
    # timestamps = np.array([datetime.strptime(ts,"[%Y/%m/%d - %H:%M:%S]") for ts in time])
    n_samples = np.array([x for x in range(0,len(time))])

    temp = np.array(samples[Metrics.TEMP])
    hm = np.array(samples[Metrics.HM])
    uv = np.array(samples[Metrics.UV])

    fig, axs = plt.subplots(1, 3, figsize=(12, 8))

    sb_plot_1 = axs[0]
    sb_plot_2 = axs[1]
    sb_plot_3 = axs[2]

    sb_plot_1.plot(n_samples, temp, color='crimson')
    sb_plot_1.set_xlabel('Samples')
    sb_plot_1.set_ylabel('Temperature (°C)')
    sb_plot_1.set_title('Temperature over time')
    sb_plot_1.tick_params(axis='x', rotation=45)
    sb_plot_1.grid(True, color='gray', zorder=0)

    sb_plot_2.plot(n_samples, hm, color='azure')
    sb_plot_2.set_xlabel('Samples')
    sb_plot_2.set_ylabel('Humidity (%)')
    sb_plot_2.set_title('Humidity over time')
    sb_plot_2.tick_params(axis='x', rotation=45)  
    sb_plot_2.grid(True, color='gray', zorder=0)

    sb_plot_3.plot(n_samples, uv, color='indigo')
    sb_plot_3.set_xlabel('Samples')
    sb_plot_3.set_ylabel('Ultraviolet Irradiance (mW/m²)')
    sb_plot_3.set_title('Ultraviolet Irradiance over time')
    sb_plot_3.tick_params(axis='x', rotation=45)  
    sb_plot_3.grid(True, color='gray', zorder=0)

    plt.tight_layout()

    return fig

def plot_visuals(display_flags: list[bool], ep: str | None = None):
    data = {}

    if ep == None:
        return 
     
    data = prepare_samples_for_plotting(ep)

    plt.style.use('dark_background')

    if display_flags[0] and data:
        plot_sample_graph(data)

    if display_flags[1] and data: 
        plot_sample_dist(data)

    if any(display_flags) and data:
        plt.show(block=False)
        plt.pause(VISUALS_REFRESH_PERIOD_SEC)
        plt.close("all")

