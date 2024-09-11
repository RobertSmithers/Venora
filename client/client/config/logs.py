'''Module to maintain Venora's logging features across entire package'''
import logging
import colorlog

GREEN_COLOR_START = '\x1b[1;32m'
RED_COLOR_START = '\033[91m'
COLOR_END = '\x1b[0m'


def setup_logging(log_fname: str) -> None:  # logging.Logger:
    """creates and returns a logger instance with a file handler and stdout handler"""

    logger = logging.getLogger()

    # Check if the logger already has handlers
    if not logger.hasHandlers():
        logger.setLevel(logging.INFO)

        fmt_str = '%(levelname)s - %(message)s'
        cmd_formatter = colorlog.ColoredFormatter(
            '%(log_color)s' + fmt_str,
            log_colors={
                'DEBUG': 'cyan',
                'INFO': 'white',
                'WARNING': 'yellow',
                'ERROR': 'red',
                'CRITICAL': 'bold_red',
            })
        file_formatter = logging.Formatter('%(asctime)s ' + fmt_str)

        # Stdout/cmd handler
        stdout_handler = logging.StreamHandler()
        stdout_handler.setLevel(logging.DEBUG)
        stdout_handler.setFormatter(cmd_formatter)

        # File handler
        file_handler = logging.FileHandler(log_fname)
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(file_formatter)

        logging.getLogger().addHandler(stdout_handler)
        logging.getLogger().addHandler(file_handler)

    return logger
