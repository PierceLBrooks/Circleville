git submodule update --init --recursive
python -m pip install -r ./Buildster/requirements.txt
python ./Buildster/buildster.py ./Project.xml

