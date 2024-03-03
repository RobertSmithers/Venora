# pylint: disable=missing-module-docstring
from setuptools import setup, find_packages

setup(
    name='client',
    version='0.1',
    packages=find_packages(),
    install_requires=[
        'cmd2',
        'colorlog',
        'pytest'
    ],
    entry_points={
        'console_scripts': [
            'venora-client=client.client:main',
        ],
    },
    setup_requires=['pytest-runner'],
    # test_suite='tests',  # Specify the test suite location
    tests_require=[
        'pytest',
    ],
)
