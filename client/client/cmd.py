"""
cmd module

Runs a fake command prompt for the user to interact with Venora server
using specified commands/actions
"""
import logging
import socket

from typing import Dict

import cmd2
from client.config.logs import GREEN_COLOR_START, GREEN_COLOR_END
from client.networking.comms import connect_to_server
from client.networking.schema import ResponseType
from client.logic.actions import (
    send_register_request, receive_register_response,
    send_login_request,
    receive_login_response,
    send_get_strike_packs_request
)

logger = logging.getLogger(__name__)


class ClientCmd(cmd2.Cmd):
    """cmd2 instance to handle interfacing with user
    Will eventually replace this with a web frontend"""

    def __init__(self, sock: socket.socket, settings: Dict = None):
        super().__init__()
        self.hidden_commands = {
            "alias", "macro", "run_script",
            "shell", "edit", "run_pyscript",
            "set", "shortcuts", "eof",
            "_relative_run_script", "help"
        }

        self.default_to_shell = {
            "shell": self.hidden_commands,
            "builtin": [],
            "script": [],
            "alias": [],
        }

        self.sock = sock
        self.settings = settings

    def do_connect(self, args):
        """Connect to Verona Server
        If IP and Port are not provided, will use the values given in settings"""
        if self.sock:
            logger.warn("Already connected to server. Aborting")
            return

        params = args.arg_list
        # If didn't specify ip/port, attempt to use settings
        if not params:
            if not self.settings:
                logger.error("No ip/port specified to connect to")
                return
            ip = self.settings.get('server_ip')
            port = self.settings.get('server_port')
        # Attempt to use args if present
        elif len(params) != 2:
            logger.error(
                "To connect to VenoraServer from settings, type 'connect'\nOtherwise, type 'connect [ip] [port]'")
            return
        else:
            ip, port = params
            try:
                port = int(port)
                if 65535 < port <= 0:
                    raise ValueError()
            except ValueError:
                logger.error("Port number %s is invalid", port)
                return
        logger.info("Connecting to Venora Server (%s:%d)", ip, port)
        self.sock = connect_to_server(ip, port)
        if not self.sock:
            logger.warn("Failed to connect to server.")
        else:
            logger.info(GREEN_COLOR_START + "Connected!" + GREEN_COLOR_END)

    def do_register(self, arg):
        """Register a user with the provided username."""
        username = arg.strip()
        if username:
            # TODO: register functionality (temporarily allow all/receive
            # token)
            # try:
            send_register_request(self.sock, username, True)
            response = receive_register_response(self.sock, True)
            # except:
            #     logger.error(
            #         "Server pipe problem during communication. Cancelling operation")
            #     return
            print(response)
            status = response[0]
            if status == ResponseType.SUCCESS_DATA:
                [_, token] = response
                resp_text_prompt = f"User '{username}' registered successfully. Please save your account's private token, it is only sent once!"
                logger.info(resp_text_prompt)
                # Using stdout to prevent token artifact in logs
                print(GREEN_COLOR_START + "Token: " + token + GREEN_COLOR_END)
            elif status == ResponseType.FAILURE:
                [_, msg] = response
                logger.error("SERVER - %s", msg)
            elif status == ResponseType.INVALID_REQUEST:
                logger.error("SERVER - Invalid request")
            elif status == ResponseType.SERVER_ERROR:
                logger.error("Server Error")
            else:
                logger.error("Unexpected response of type %s",
                             ResponseType(status).name)
        else:
            print("Please provide a username for registration. (ex: register RJ)")

    def do_login(self, args):
        """Login with the provided username."""
        if not args.arg_list or len(args.arg_list) != 2:
            logging.error("Usage: login [username] [token]")
            return

        username, token = args.arg_list
        if username and token:
            # try:
            send_login_request(self.sock, username, token, True)
            [token] = receive_login_response(self.sock, True)
            print("Login successful!")
            # except:
            #     logger.error(
            #         "Server pipe problem during communication. Cancelling operation")
            #     return

    def do_list(self, arg):
        """List packs."""
        if arg.strip().lower() == 'packs':
            print("Listing packs...")
            send_get_strike_packs_request(self.sock)

        else:
            print("Invalid command. Use 'list packs' to list packs.")

    # pylint: disable=unused-argument
    def do_quit(self, args):
        """Quit the program."""
        print("Exiting the program.")
        return True
