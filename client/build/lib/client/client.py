"""
client module

This module serves as the main entry point for the client application and includes utility functions
for executing client functionality.

Functions:
- run_client() -> None:
  Runs the client and handles communication w/ the server

- main() -> None:
  Main entry point of the application.
  Initializes all components and calls run_client() when ready

Note:
- The `file_utils` module provides utility functions for working with JSON files and settings.
"""

#!/usr/bin/env python3

import logging

from client.config import get_settings, save_settings, LOG_FILENAME

def run_client() -> None:
    """
    Run the client.

    Connects to the Venora Server, handles communication, and deals with client logic.
    """
    print("I am a client. I am a client. I am A CLIENT. I AM A CLIENT!")

    # Get the settings from 3 sources and save it
    settings = get_settings()
    save_settings(settings)

    # Setup logging
    logging.basicConfig(filename=settings.get('log_file', LOG_FILENAME),
                        level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
    logging.info('Settings: %s', settings)

    logging.debug("Reach out to server here and start logic")

def main():
    """
    Entry point of client
    """
    run_client()
