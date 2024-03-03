import cmd2
import logging
import socket

logger = logging.getLogger(__name__)


class ClientCmd(cmd2.Cmd):
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
            # TODO: register functionality (temporarily allow all/receive token)
            print(f"User '{username}' registered successfully.")
        else:
            print("Please provide a username for registration. (ex: login RJ)")

    def do_login(self, arg):
        """Login with the provided username."""
        username = arg.strip()
        if username:
            # TODO: login functionality (server-side db validation)
            print(f"User '{username}' logged in successfully.")
        else:
            print("Please provide a username for login.")

    def do_list(self, arg):
        """List packs."""
        if arg.strip().lower() == 'packs':
            print("Listing packs...")
        else:
            print("Invalid command. Use 'list packs' to list packs.")

    def do_quit(self, arg):
        """Quit the program."""
        print("Exiting the program.")
        return True


if __name__ == '__main__':
    client_cmd = ClientCmd()
    client_cmd.cmdloop()
