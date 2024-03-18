import argparse
import time 
import threading as th

from stats import Metrics, get_data_statistics, prepare_samples_for_plotting
from visuals import plot_visuals

ALL_SAMPLES_ENDPOINT: str = 'http://0.0.0.0:42068/api/get_samples'
LAST_100_SAMPLES_ENDPOINT: str = 'http://0.0.0.0:42068/api/ui_update'
STATS_DISPLAY_PERIOD_SEC = 60

def stats_display():
    while True:
        get_data_statistics(LAST_100_SAMPLES_ENDPOINT)
        time.sleep(STATS_DISPLAY_PERIOD_SEC)

def visuals_display(samples: dict[Metrics, list] | None, display_flags: list[bool]):
    plot_visuals(display_flags, samples, None)

    while True:
        # The plot_visuals function handles refreshing by itself 
        plot_visuals(display_flags, None, ALL_SAMPLES_ENDPOINT)

def setup_jobs(display_flags: list[bool]):
    samples_dict: dict[Metrics, list] | None = None

    stats_thread = th.Thread(target=stats_display)

    stats_thread.start()

    if any(display_flags):
        samples_dict = prepare_samples_for_plotting(ALL_SAMPLES_ENDPOINT)

        visuals_display(samples_dict, display_flags)

def monitoring_system_cli():
    cli_parser = argparse.ArgumentParser(description="Monitoring System CLI data visualization")
    cli_parser.add_argument('-d', '--dist', action='store_true', help='enables dist displaying')
    cli_parser.add_argument('-g','--graph', action='store_true', help='enables graph displaying')

    args = cli_parser.parse_args()

    display_flags: list[bool] = [False,False]

    if args.graph:
        display_flags[0] = True

    if args.dist:
        display_flags[1] = True

    setup_jobs(display_flags)

if __name__ == "__main__":
    monitoring_system_cli()
