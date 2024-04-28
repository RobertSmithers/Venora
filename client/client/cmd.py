"""
cmd module

Runs a fake command prompt for the user to interact with Venora server
using specified commands/actions
"""
import logging
import socket

from typing import Dict

import cmd2
from client.config.logs import GREEN_COLOR_START, RED_COLOR_START, COLOR_END
from client.networking.comms import connect_to_server
from client.networking.schema import ResponseType
from client.logic.netparse import base_response_handler
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
            logger.info(GREEN_COLOR_START + "Connected!" + COLOR_END)

    def do_register(self, arg):
        """Register a user with the provided username."""
        if not self.sock:
            logger.warn("You must connect to a server before registering.")
            return

        username = arg.strip()
        if username:
            # try:
            send_register_request(self.sock, username, True)
            response = receive_register_response(self.sock, True)
            base_response_handler(
                response, [ResponseType.SUCCESS_DATA, ResponseType.FAILURE])
            status = response[0]
            if status == ResponseType.SUCCESS_DATA:
                [_, token] = response
                resp_text_prompt = f"User '{username}' registered successfully. Please save your account's private token, it is only sent once!\nYou may now login"
                logger.info(resp_text_prompt)
                # Using stdout to prevent token artifact in logs
                print(GREEN_COLOR_START + "Token: " + token + COLOR_END)
            elif status == ResponseType.FAILURE:
                [_, msg] = response
                logger.error("SERVER - %s", msg)
            else:
                logger.error("Unexpected response of type %s",
                             ResponseType(status).name)
            # except:
            #     logger.error(
            #         "Server pipe problem during communication. Cancelling operation")
            #     return
        else:
            print("Please provide a username for registration. (ex: register RJ)")

    def do_login(self, args):
        """Login with the provided username."""
        if not self.sock:
            logger.warn("You must connect to a server before logging in.")
            return

        if not args.arg_list or len(args.arg_list) != 2:
            logging.error("Usage: login [username] [token]")
            return

        username, token = args.arg_list
        if username and token:
            try:
                send_login_request(self.sock, username, token, True)
                resp = receive_login_response(self.sock, True)
                base_response_handler(
                    resp, [ResponseType.FAILURE, ResponseType.SUCCESS])
                if (resp[0] == ResponseType.FAILURE):
                    if (len(resp) > 1):
                        print(RED_COLOR_START + "Login failed: " +
                              str(resp[1]) + COLOR_END)
                    else:
                        print(RED_COLOR_START + "Login failed" + COLOR_END)
                elif (resp[0] == ResponseType.SUCCESS):
                    print(GREEN_COLOR_START + "Login successful!" + COLOR_END)
            except:
                logger.error(
                    "Server pipe problem during communication. Cancelling operation")
                return

    def do_list(self, arg):
        """List packs."""
        if not self.sock:
            logger.warn("You must connect to a server before listing packs.")
            return

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
