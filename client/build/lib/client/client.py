#!/usr/bin/env python3
import logging
import json 
import argparse

from client.config import get_settings, save_settings, LOG_FILENAME

# fp = urllib.request.urlopen("http://localhost:1234/")

def run_client():
    print("I am a client. I am a client. I am A CLIENT. I AM A CLIENT!")

    # Get the settings from 3 sources and save it
    settings = get_settings()
    save_settings(settings)

    # Setup logging
    logging.basicConfig(filename=settings.get('log_file', LOG_FILENAME), level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
    logging.info('Settings: %s', settings)

    logging.debug("Reach out to server here and start logic")
    
def main():
    run_client()