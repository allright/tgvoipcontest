#!/bin/sh
cd ../../
rm -rf submission.zip
rm -rf submission
mkdir submission
cp -a src submission/src
find ./submission -type d -name '.*' -exec rm -rf {} +
find ./submission -type f -name '.*' -exec rm {} +
rm -rf ./submission/src/tgvoipcallpy/venv
zip -r submission.zip submission
rm -rf ./submission

docker build -t tgvoipcontestbuild -f ./src/scripts/DockerfileBuildSubmission .
rm -rf ./docker_build
docker run -v ${PWD}/.docker_build:/out tgvoipcontestbuild cp ./tgvoiprate /out
cd ./.docker_build
mkdir ./submission
mv ./tgvoiprate ./submission/
zip -r ../submission.zip submission/tgvoiprate
cd ..
rm -rf ./.docker_build
rm -rf ./submission



# rm -rf .build
# mkdir .build
# cd ./.build
# cmake ../src/tgvoiprate -DCMAKE_BUILD_TYPE=RELEASE
# make -j8 



#docker build -t tgvoipcontext .
