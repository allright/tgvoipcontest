#!/bin/sh
docker run tgvoipcontest ls
docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./src/tgvoipcallpy/tgvoipcallpy.py ./tgvoipcall /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/sound_output_B.ogg /testdata/samples/sample17_fd738975ea9a518e48680e3c33ee4c05.ogg /testdata/sound_output_A.ogg ./tgvoiprate
#docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./src/tgvoipcallpy/tgvoipcallpy.py ./tgvoipcall /testdata/samples/sample06_6691afc81fd72df69da5b1a7a508b30e.ogg /testdata/sound_output_B.ogg /testdata/samples/sample05_b4e08e2fae45c5991b82b80755d042a5.ogg /testdata/sound_output_A.ogg ./tgvoiprate
#docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./src/tgvoipcallpy/tgvoipcallpy.py ./tgvoipcall /testdata/samples/sample60_bf6bb62a74e210c44b6a256417a8193a.ogg /testdata/sound_output_B.ogg /testdata/samples/sample60_c70f8ed50f21ebc03588c87900db520d.ogg /testdata/sound_output_A.ogg ./tgvoiprate


#docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoipcall /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/out.ogg
#docker run -v ${PWD}/../../testdata:/testdata tgvoipcontest ./tgvoiprate /testdata/samples/sample17_0d1f6a407f028a451f3a6a90098a9300.ogg /testdata/out.ogg
