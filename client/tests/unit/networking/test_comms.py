# client/networking/test_client.py

import logging
import socket
import pytest
from unittest.mock import patch, Mock
from client.networking.comms import (
    connect_to_server,
    recv_from_srv,
    send_to_srv
)


@pytest.fixture
def mock_socket():
    """mock socket used for all socket tests"""

    mock_sock = Mock(spec=socket.socket)

    with patch("socket.socket", return_value=mock_sock):
        yield mock_sock

## connect_to_server ##


@pytest.mark.parametrize("server_ip, server_port,", [
    ("127.0.0.1", 8080),
    ("127.0.0.1", 80),
    ("127.0.0.1", 53),
])
def test_connect_to_server(server_ip, server_port, mock_socket):
    result = connect_to_server(server_ip, server_port)

    assert result == mock_socket
    socket.socket.assert_called_once_with(socket.AF_INET, socket.SOCK_STREAM)
    mock_socket.connect.assert_called_once_with((server_ip, server_port))


@pytest.mark.parametrize("server_ip, server_port", [
    ("255.255.255.255", 8080),
    ("1.1.1.256", 2345),
    ("192.168.1", 3456),
])
def test_connect_to_server_invalid_ip(server_ip, server_port):
    result = connect_to_server(server_ip, server_port)
    assert result is None

## recv_from_srv ##


@pytest.mark.parametrize("verbose, expected_result", [
    (False, "test_data"),
    (True, "test_data")
])
def test_recv_from_srv_different_verbosity(mock_socket, verbose, expected_result):
    with patch("builtins.print") as mock_print:
        with patch.object(mock_socket, "recv", return_value=b"test_data") as mock_recv:
            result = recv_from_srv(mock_socket, verbose)

    assert result == expected_result
    mock_recv.assert_called_once_with(1024)
    if verbose:
        mock_print.assert_called_once_with(f"Received: {expected_result}")
    else:
        mock_print.assert_not_called()

## send_to_srv ##


@pytest.mark.parametrize("data, verbose", [
    (b"test_data", False),
    (b"test_data", True),
])
def test_send_to_srv(mock_socket, data, verbose):
    with patch("builtins.print") as mock_print:
        with patch.object(mock_socket, "sendall") as mock_sendall:
            send_to_srv(mock_socket, data, verbose)

    mock_sendall.assert_called_once_with(data)
    if verbose:
        mock_print.assert_called_once_with(f"Sent: {data}")
    else:
        mock_print.assert_not_called()
