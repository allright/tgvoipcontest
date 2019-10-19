FROM debian:10.1
RUN apt-get update -y && \
    apt-get clean && \
    apt-get install -y \
    unzip python2.7 python-pip \
    && apt-get clean

RUN pip install requests


RUN mkdir /work
WORKDIR /work
COPY submission.zip ./
RUN unzip submission.zip
WORKDIR submission/src
RUN ls -al
RUN tgvoipcallpy/tgvoipcallpy.py
