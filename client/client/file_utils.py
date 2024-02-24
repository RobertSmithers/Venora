"""
file_utils module

This module provides utility functions for working with JSON files and settings.

Functions:
- load_settings_from_file(filename: str) -> dict:
  Load settings from a JSON file.

- write_settings_to_file(filename: str, settings: dict) -> None:
  Write settings to a JSON file.

Note:
- If the file specified in load_settings_from_file is not found or there is an issue
  with the file content, an empty dictionary is returned.

- The write_settings_to_file function writes the provided settings to the specified JSON file.
  If the file does not exist, it will be created. If the file already contains data,
  the new settings will overwrite the existing content.
"""

import os
import json
import logging

logger = logging.getLogger(__name__)

# JQR 7.4 - read file, parse data
def load_settings_from_file(filename: str) -> json:
    """
    Load settings from a JSON file.

    Args:
        filename (str): The name of the JSON file to load settings from.

    Returns:
        dict: A dictionary containing the loaded settings.
              If the file is not found or there is an issue with the file content,
              an empty dictionary is returned.
    """
    # Note this function is called before log setup (since this is needed for log filename)
    # Hence we cannot use logging in here
    if filename is None or not os.path.exists(filename):
        print(f"Settings file \"{filename}\" does not exist")
        return {}

    jf = None

    try:
        with open(filename, 'r', encoding='utf8') as f:
            jf = json.load(f)
            print(json.dumps(jf, indent=2))
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON in file {filename}: {e}")
        return {}

    return jf

# JQR 7.4 - Write to file, create new file, close open file
def write_settings_to_file(filename: str, settings: dict) -> None:
    """
    Write settings to a JSON file.

    Args:
        filename (str): The name of the JSON file to write settings to.
        settings (dict): A dictionary containing the settings to be written.

    Returns:
        None
    """
    if filename is None:
        logger.debug('Failed to save settings -- filename is None')
        return

    # pylint: disable=consider-using-with
    f = open(filename, 'w', encoding='utf8')
    f.write(json.dumps(settings))
    f.close()
