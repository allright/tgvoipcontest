FROM debian:10.1
RUN apt-get update -y && \
    apt-get clean && \
    apt-get install -y \
    build-essential \
    autoconf \
    git \
    automake \
    make \
    cmake \
    libtool \
    libopus-dev \
    libssl-dev \
    libpulse-dev \
    libasound-dev && \
    apt-get clean

ADD . /src
WORKDIR /src
RUN ls
RUN cmake ./
RUN make -j8
RUN ./tgvoiprate file.wav file1.wav