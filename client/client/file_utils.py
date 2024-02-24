import os
import json
import logging

logger = logging.getLogger(__name__)

# JQR 7.4 - read file, parse data
# Note this is called before log setup (since this is needed for log filename)
# Hence we cannot use logging in here
def load_settings_from_file(filename: str) -> json:
    if filename is None or not os.path.exists(filename):
        print(f"Settings file \"{filename}\" does not exist")
        return {}

    jf = None
    
    try:
        with open(filename, 'r') as f:
            jf = json.load(f)
            print(json.dumps(jf, indent=2))
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON in file {filename}: {e}")
        return {}

    return jf

# JQR 7.4 - Write to file, create new file, close open file 
def write_settings_to_file(filename: str, settings: dict) -> None:
    if filename is None:
        logger.debug('Failed to save settings -- filename is None')
        return
    
    print("SOMETHING WAS CALLED pt 2")
    f = open(filename, 'w')
    f.write(json.dumps(settings))
    f.close()
