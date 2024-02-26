import pytest
import struct

from client.networking.schema import RequestType
from client.config import USERNAME_MAX_CHARS
from client.networking.packer import (
    ResponseType,
    pack_type_register,
    pack_type_authenticate,
    unpack_response_type,
)


@pytest.mark.parametrize("data, expected_result", [
    ({"username": "alice"},
     b'\x00\x01\x00\x05alice'),
    ({"username": "bob", "token": 1234},
     b'\x00\x01\x00\x03bob'),
    ({"username": "a"*USERNAME_MAX_CHARS},
     b'\x00\x01' + struct.pack(f'!H{USERNAME_MAX_CHARS}s',
                               USERNAME_MAX_CHARS,
                               ("a"*USERNAME_MAX_CHARS).encode())
     ),
])
def test_pack_type_register(data, expected_result):
    """
    Tests the packed data for register
    Should only pack the request type (0x1), the username length, and the username
    """
    result = pack_type_register(data)
    assert result == expected_result


@pytest.mark.parametrize("data", [
    ({"user": "alice"}),
    ({"token": "bob"}),
])
def test_pack_type_register_missing_field(data):
    """
    Tests the packed data for register w/ invalid data
    Should return None
    """
    result = pack_type_register(data)
    assert result is None


@pytest.mark.parametrize("data", [
    ({"username": "a"*(USERNAME_MAX_CHARS+1)}),
    ({"username": "abc"*USERNAME_MAX_CHARS}),
])
def test_pack_type_register_username_too_large(data):
    """
    Tests the packed data for register w/ invalid data
    Should return None
    """
    result = pack_type_register(data)
    assert result is None


@pytest.mark.parametrize("data, expected_result", [
    ({"username": "alice", "token": "abc123"},
     b'\x00\x02\x00\x05alice\x00\06abc123'),
    ({"username": "bob", "token": "xyz7890"},
     b'\x00\x02\x00\x03bob\x00\x07xyz7890'),
])
def test_pack_type_authenticate(data, expected_result):
    result = pack_type_authenticate(data)
    assert result == expected_result


@pytest.mark.parametrize("data", [
    ({"username": "a"*(USERNAME_MAX_CHARS+1), "token": "abc123"}),
    ({"username": "abc"*USERNAME_MAX_CHARS, "token": "abc123"}),
])
def test_pack_type_authenticate_username_too_large(data):
    """
    Tests the packed data for register w/ invalid data
    Should return None
    """
    result = pack_type_authenticate(data)
    assert result is None


@pytest.mark.parametrize("data", [
    ({"username": "alice"}),
    ({"token": "xyz7890"}),
])
def test_pack_type_authenticate_missing_field(data):
    """
    Tests the packed data for register w/ invalid data
    Should return None
    """
    result = pack_type_authenticate(data)
    assert result is None

# @pytest.mark.parametrize("response, expected_result", [
#     (b'\x01\x00\x00\x00{"status": "success"}', ResponseType.SUCCESS),
#     (b'\x02\x00\x00\x00{"status": "failure"}', ResponseType.FAILURE),
#     (b'\x03\x00\x00\x00{"status": "invalid_req"}', ResponseType.INVALID_REQ),
#     (b'\x04\x00\x00\x00{"status": "server_err"}', ResponseType.SERVER_ERR),
# ])
# def test_unpack_response_type(response, expected_result):
#     result = unpack_response_type(response)
#     assert result == expected_result
