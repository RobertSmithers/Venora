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
import time

from client.config import get_settings, save_settings, LOG_FILENAME, CONNECTION_RETRY_TIME
import client.networking.comms as net


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

    ip, port = settings.get('server_ip'), settings.get('server_port')
    if not ip or not port:
        logging.error("Server IP or port is not set")
        return
    if not isinstance(port, int):
        logging.error("Could not parse server port number")
        return

    c_sock = net.connect_to_server(ip, port)
    # If fail to connect, retry one time after a timeout
    if not c_sock:
        logging.info(
            "Failed to connect to server. Retrying in %d seconds.", CONNECTION_RETRY_TIME)
        time.sleep(CONNECTION_RETRY_TIME)
        c_sock = net.connect_to_server(ip, port)

    # If failed a second time, abort
    if not c_sock:
        logging.error("Failed to connect to server")
        return

    # Connected to server
    logging.info(f"Connected to Venora Server {ip}:{port}")

    # Here I should have a shell to input commands/talk to program --> talk to server if needed


def main():
    """
    Entry point of client
    """
    run_client()
