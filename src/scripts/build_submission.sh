#!/bin/sh
rm -rf ../../submission
mkdir ../../submission
cp -a ../../src ../../submission/src
cd ../../
find ./submission -type d -name '.*' -exec rm -rf {} +
rm -rf ./submission/src/tgvoipcallpy/venv
zip -r submission.zip submission
rm -rf ./submission
#docker build -t tgvoipcontext .
