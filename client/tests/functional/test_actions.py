
import unittest
from unittest.mock import patch, MagicMock
import socket

from client.networking.schema import RequestType
from client.logic.actions import send_get_strike_packs_request, receive_get_strike_packs_response


class TestClientActions(unittest.TestCase):

    @patch('client.logic.actions.pack_req')
    @patch('client.logic.actions.send_to_srv')
    def test_send_get_strike_packs_request(self, mock_send_to_srv, mock_pack_req):
        # Mock the pack_req function to return a non-empty request
        mock_pack_req.return_value = b'packed_request'

        # Create a mock socket
        mock_sock = MagicMock(spec=socket.socket)

        # Call the function to be tested
        send_get_strike_packs_request(mock_sock, verbose=True)

        # Assert pack_req was called with the correct arguments
        mock_pack_req.assert_called_once_with(RequestType.GET_STRIKE_PACKS, {})

        # Assert send_to_srv was called with the correct arguments
        mock_send_to_srv.assert_called_once_with(
            mock_sock, b'packed_request', verbose=True)

    @patch('client.logic.actions.recv_from_srv')
    @patch('client.logic.actions.unpack_response_type')
    @patch('client.logic.actions.unpack_type')
    def test_receive_get_strike_packs_response(self, mock_unpack_type, mock_unpack_response_type, mock_recv_from_srv):

        # Mock return data of recv and some functions
        mock_recv_from_srv.return_value = b'response_data'
        mock_unpack_response_type.return_value = 'response_type'
        mock_unpack_type.return_value = [
            'tmp_strike_pack_1', 'tmp_strike_pack_2']

        mock_sock = MagicMock(spec=socket.socket)

        # Actual result
        result = receive_get_strike_packs_response(mock_sock, verbose=True)

        # Assert funcs called with correct args
        mock_recv_from_srv.assert_called_once_with(mock_sock, 2, verbose=True)
        mock_unpack_response_type.assert_called_once_with(b'response_data')
        mock_unpack_type.assert_called_once_with(
            mock_sock, 'response_type', RequestType.GET_STRIKE_PACKS)

        # Assert the result is as expected
        self.assertEqual(result, ['tmp_strike_pack_1', 'tmp_strike_pack_2'])


if __name__ == '__main__':
    unittest.main()
