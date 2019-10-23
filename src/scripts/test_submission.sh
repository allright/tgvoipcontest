#!/bin/sh
docker run tgvoipcontest ls
docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./src/tgvoipcallpy/tgvoipcallpy.py ./tgvoipcall /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/sound_output_B.ogg /testdata/samples/sample17_fd738975ea9a518e48680e3c33ee4c05.ogg /testdata/sound_output_A.ogg
#docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoipcall /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/out.ogg
#docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoiprate /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/out.ogg
