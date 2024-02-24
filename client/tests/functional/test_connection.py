import pytest
import subprocess
from time import sleep
from client import Client

class ClientTester(Client):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

# Client used for integration tests
@pytest.fixture(scope='session')
def client():
    app = ClientTester()
    app.fixture_setup()
    yield app
    app.fixture_teardown()

class TestServer():
    # Server used for integration tests
    @pytest.fixture(scope='session')
    def server(self):
        s = subprocess.Popen(['server', '4444'])
        sleep(1)
        yield s
        s.terminate()

    @pytest.mark.parametrize("arg, expected_result", [(0, 0), (1, 2), (4, 8)])
    def test_example(self, client, server, arg, expected_result):
        out = client.something(arg)
        assert out == expected_result
