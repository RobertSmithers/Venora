"""
cmd module

Runs a fake command prompt for the user to interact with Venora server
using specified commands/actions
"""
import logging
import socket

import cmd2

logger = logging.getLogger(__name__)


class ClientCmd(cmd2.Cmd):
    """cmd2 instance to handle interfacing with user
    Will eventually replace this with a web frontend"""

    def __init__(self, sock: socket.socket):
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

    def do_register(self, arg):
        """Register a user with the provided username."""
        username = arg.strip()
        if username:
            # TODO: register functionality (temporarily allow all/receive
            # token)
            print(f"User '{username}' registered successfully.")
        else:
            print("Please provide a username for registration. (ex: register RJ)")

    def do_login(self, arg):
        """Login with the provided username."""
        token = arg.strip()
        if token:
            # TODO: login functionality (server-side db validation)
            print("Login successful!")
        else:
            print("Please provide a token for login. (ex: login longtokenstringgoeshere)")

    def do_list(self, arg):
        """List packs."""
        if arg.strip().lower() == 'packs':
            print("Listing packs...")
        else:
            print("Invalid command. Use 'list packs' to list packs.")

    # pylint: disable=unused-argument
    def do_quit(self, args):
        """Quit the program."""
        print("Exiting the program.")
        return True
