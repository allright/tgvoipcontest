#!/bin/sh
docker run tgvoipcontest ls
docker run tgvoipcontest ./src/tgvoipcallpy/tgvoipcallpy.py
docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoiprate /testdata/in.wav /testdata/out.wav
docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoipcall /testdata/in.wav /testdata/oo1.wav
