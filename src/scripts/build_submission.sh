#!/bin/sh
cd ../../
rm -rf submission.zip
rm -rf submission
mkdir submission
cp -a src submission/src
find ./submission -type d -name '.*' -exec rm -rf {} +
find ./submission -type f -name '.*' -exec rm {} +
rm -rf ./submission/src/tgvoiprate/cmake-build-debug
rm -rf ./submission/src/tgvoipcall/cmake-build-debug
rm -rf ./submission/src/cmake-build-debug
rm -rf ./submission/src/tgvoipcallpy/venv
zip -r submission.zip submission
rm -rf ./submission

touch ./src/scripts/DockerfileTestSubmission
docker build -t tgvoipcontestbuild -f ./src/scripts/DockerfileBuildSubmission .
rm -rf ./docker_build
docker run -v ${PWD}/.docker_build:/out tgvoipcontestbuild cp ./tgvoiprate/tgvoiprate /out
docker run -v ${PWD}/.docker_build:/out tgvoipcontestbuild cp ./tgvoipcall/tgvoipcall /out
cd ./.docker_build
mkdir ./submission
mv ./tgvoiprate ./submission/
mv ./tgvoipcall ./submission/
zip -r ../submission.zip submission/tgvoiprate
zip -r ../submission.zip submission/tgvoipcall
cd ..
rm -rf ./.docker_build
rm -rf ./submission

docker build -t tgvoipcontest -f ./src/scripts/DockerfileTestSubmission .


# rm -rf .build
# mkdir .build
# cd ./.build
# cmake ../src/tgvoiprate -DCMAKE_BUILD_TYPE=RELEASE
# make -j8 



#docker build -t tgvoipcontext .
