"""
Stores configuration information for client and handles operations to access or mutate them.
"""
import argparse

from client.file_utils import load_settings_from_file, write_settings_to_file

SETTINGS_FILENAME = "venora_settings.json"
LOG_FILENAME = "client.log"
CONNECTION_RETRY_TIME = 3  # in seconds
USERNAME_MAX_CHARS = 100

CHUNK_SIZE = 1024

default_settings = {
    'log_file': LOG_FILENAME,
    'verbose': False,
    'server_ip': '127.0.0.1',
    'server_port': 9393,
}


def get_settings() -> dict:
    """
    Get Venora settings from command line and from settings file for the remainder

    Returns:
        dict: Key-values representing the app settings
    """
    parser = argparse.ArgumentParser(description='Venora client')

    # Get command line args, otherwise use default settings
    parser.add_argument('--log-file', type=str, default=None,
                        help='Filename for output logs')
    parser.add_argument('--server_ip', type=str,
                        default=None, help='Server IP address')
    parser.add_argument('--server_port', type=int,
                        default=None, help='Server port number')
    parser.add_argument('-v', '--verbose', action='store_true',
                        default=None, help='Enable verbose mode')

    # Overwrite settings with specified values from SETTINGS_FILENAME
    settings = default_settings.copy()
    f_settings = load_settings_from_file(SETTINGS_FILENAME)
    settings.update(f_settings)

    # Overwrite settings with specified values from command line
    args = parser.parse_args()
    arg_dict = vars(args)
    arg_dict = {k: v for k, v in arg_dict.items() if v is not None}
    settings.update(arg_dict)

    return settings


def save_settings(settings: dict) -> None:
    """
    Save Venora settings to a file
    """

    write_settings_to_file(SETTINGS_FILENAME, settings)
