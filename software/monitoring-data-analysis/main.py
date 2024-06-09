import argparse
import time 
import threading as th

from stats import get_data_statistics 
from visuals import plot_visuals

STATS_DISPLAY_PERIOD_SEC = 60

ALL_SAMPLES_ENDPOINT: str = 'http://0.0.0.0:42068/api/get_samples'
LAST_100_SAMPLES_ENDPOINT: str = 'http://0.0.0.0:42068/api/ui_update'

def stats_display(endpoint: str):
    while True:
        get_data_statistics(endpoint)
        time.sleep(STATS_DISPLAY_PERIOD_SEC)

def visuals_display(endpoint: str, display_flags: list[bool]):
    while True:
        # The plot_visuals function handles refreshing by itself 
        plot_visuals(display_flags, endpoint)

def setup_jobs(display_flags: list[bool], endpoints: list[str]):
    stats_thread = th.Thread(target=stats_display, args=(endpoints[1],))

    stats_thread.start()

    if any(display_flags):
        visuals_display(endpoints[1], display_flags)

def monitoring_system_cli():
    cli_parser = argparse.ArgumentParser(description="Monitoring System CLI data visualization")
    cli_parser.add_argument('-d', '--dist', action='store_true', help='enables dist displaying')
    cli_parser.add_argument('-g','--graph', action='store_true', help='enables graph displaying')
    cli_parser.add_argument('-D', '--docker', action='store_true', help='changes the endpoints to the docker compose network')

    args = cli_parser.parse_args()

    display_flags: list[bool] = [False,False]
    endpoints: list[str] = [ALL_SAMPLES_ENDPOINT, LAST_100_SAMPLES_ENDPOINT]

    if args.graph:
        display_flags[0] = True

    if args.dist:
        display_flags[1] = True

    if args.docker:
        endpoints[0] = 'http://backend:42068/api/get_samples'
        endpoints[1] = 'http://backend:42068/api/ui_update'

    setup_jobs(display_flags, endpoints)

if __name__ == "__main__":
    monitoring_system_cli()
