import argparse
import requests as rq
import json 
import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser(description='Simple method for visualizing data from monitoring-system api')

parser.add_argument('port',help='Port number of the api',type=str)
parser.add_argument('-n','--samples',help='Defines the size request query',required=False,type=int)
parser.add_argument('-s','--stats',help='Enables request for statistics',action='store_true',required=False)

args = parser.parse_args()

# TODO default case
if args.samples != None:
    samples = args.samples
else:
    samples = 0

if args.stats:
    stats = "True"
else: 
    stats = "False"

# TODO Request stuff
# data = rq.get(f'http://127.0.0.1:{args.port}/data?')

# TODO matplotlib stuff
