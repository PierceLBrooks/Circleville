#!/bin/sh
git submodule update --init --recursive
python3 -m pip install -r ./Buildster/requirements.txt
python3 ./Buildster/buildster.py ./Project.xml

