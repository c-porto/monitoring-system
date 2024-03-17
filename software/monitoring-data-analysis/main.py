import argparse
import time 
import threading as th

from stats import Metrics, get_data_statistics, prepare_samples_for_plotting

ALL_SAMPLES_ENDPOINT: str = 'http://0.0.0.0:42068/api/get_samples'
LAST_100_SAMPLES_ENDPOINT: str = 'http://0.0.0.0:42068/api/ui_update'

def stats_display():
    while True:
        get_data_statistics(LAST_100_SAMPLES_ENDPOINT)
        time.sleep(300)

def graph_display(samples: dict[Metrics, list]):
    # TODO
    while True:
        print("Teste2")
        time.sleep(2)

def dist_display(samples: dict[Metrics, list]):
    # TODO
    while True:
        print("Teste1")
        time.sleep(5)

def setup_jobs(display_flags: list[bool]):
    samples_dict: dict[Metrics, list] | None = None

    stats_thread = th.Thread(target=stats_display)

    stats_thread.start()

    if any(display_flags):
        samples_dict = prepare_samples_for_plotting(ALL_SAMPLES_ENDPOINT)

    if display_flags[0]:
        graph_thread = th.Thread(target=graph_display, args=(samples_dict,))
        graph_thread.start()

    if display_flags[1]:
        dist_thread = th.Thread(target=dist_display, args=(samples_dict,))
        dist_thread.start()

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
