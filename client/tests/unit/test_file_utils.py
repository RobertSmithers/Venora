import json
import pytest
from unittest.mock import patch, mock_open

from client.file_utils import load_settings_from_file, write_settings_to_file
from client.config import settings as default_settings

@pytest.mark.parametrize("file_content, expected_settings", [
    ('{"nonexistent_key": "value"}', {"nonexistent_key": "value"}),
    ('{"server_ip": "255.255.255.999"}', {"server_ip": "255.255.255.999"}),
    ('{}', {}),
])
def test_load_settings_from_file(file_content, expected_settings):
    with patch('builtins.open', mock_open(read_data=file_content)) as mock_file, \
         patch('os.path.exists', return_value=True):
        settings = load_settings_from_file('settings.json')
    assert settings == expected_settings

def test_load_settings_from_file_not_found_error():
    with patch('builtins.open', side_effect=FileNotFoundError):
        settings = load_settings_from_file('nonexistent_file.json')
    assert settings == {}

@pytest.mark.parametrize("file_content", [
    ('Test123'),
    ('{{"key": "value"}'),
    ('\n{"verbose": true}')
])
def test_load_settings_from_file_invalid_json(file_content):
    with patch('builtins.open', mock_open(read_data=file_content)):
        settings = load_settings_from_file('test_settings.json')
    assert settings == {}

@pytest.mark.parametrize("settings", [
    ({"key": "value"}),
    ({"verbose": "true", "server_ip": "127.0.0.1", "server_port": 9999})
])
def test_write_settings_to_file(settings):
    with patch('builtins.open', mock_open()) as mock_file:
        write_settings_to_file('test_settings.json', settings)
        mock_file.assert_called_once_with('test_settings.json', 'w')
        mock_file().write.assert_called_once_with(json.dumps(settings))
        mock_file().close.assert_called_once()

if __name__ == '__main__':
    pytest.main()

