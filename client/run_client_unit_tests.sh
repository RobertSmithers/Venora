#!/bin/bash

pip install client/
python3 -m pytest --full-trace client/tests/unit/
# python3 -m pytest client/tests/unit/networking/