"""
test class for client.networking.comms.py
"""
# client/networking/test_client.py

import socket
from unittest.mock import patch, Mock
import pytest
import logging
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
    """tests connect to server returns a single socket with given ip:port"""
    result = connect_to_server(server_ip, server_port)

    assert result == mock_socket
    socket.socket.assert_called_once_with(socket.AF_INET, socket.SOCK_STREAM)
    mock_socket.connect.assert_called_once_with((server_ip, server_port))


@pytest.mark.parametrize("server_ip, server_port", [
    ("255.255.255.255", 8080),
    ("1.1.1.256", 2345),
    ("192.16a.1.1", 3456),
])
def test_connect_to_server_invalid_ip(server_ip, server_port):
    """tests connect to server returns None when given invalid ip"""
    result = connect_to_server(server_ip, server_port)
    assert result is None

## recv_from_srv ##


@pytest.mark.parametrize("verbose, expected_result", [
    (False, b"test_data"),
    (True, b"test_data")
])
def test_recv_from_srv_different_verbosity(
        mock_socket, caplog, verbose, expected_result):
    """tests connect to server prints the received data when verbose is set"""
    with caplog.at_level(logging.DEBUG):
        with patch.object(mock_socket, "recv", return_value=b"test_data") as mock_recv:
            result = recv_from_srv(mock_socket, 1024, verbose)

    assert result == expected_result
    mock_recv.assert_called_once_with(1024)
    if verbose:
        assert f"Received: {expected_result}" in caplog.text
    else:
        assert "Received:" not in caplog.text

## send_to_srv ##


@pytest.mark.parametrize("data, verbose", [
    (b"test_data", False),
    (b"test_data", True),
])
def test_send_to_srv(mock_socket, caplog, data, verbose):
    """tests send to srv uses sendall and logs it when verbose"""
    with caplog.at_level(logging.INFO):
        with patch.object(mock_socket, "send", return_value=len("test_data")) as mock_send:
            send_to_srv(mock_socket, data, verbose)

    mock_send.assert_called_once_with(data)
    if verbose:
        assert "Sent: " in caplog.text
        assert str(data) in caplog.text
    else:
        assert "Sent:" not in caplog.text
        assert str(data) not in caplog.text
