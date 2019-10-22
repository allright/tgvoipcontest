#!/bin/sh
docker run tgvoipcontest ls
#docker run tgvoipcontest ./src/tgvoipcallpy/tgvoipcallpy.py
docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoipcall /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/out.ogg
docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoiprate /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/out.ogg
