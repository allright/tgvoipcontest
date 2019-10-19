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
    libasound-dev \
    python2.7 python-pip \
    && \
    apt-get clean

RUN pip install requests
RUN mkdir /work
ADD /tgvoipcallpy /work/tgvoipcallpy

WORKDIR /work
RUN tgvoipcallpy/tgvoipcallpy.py


#WORKDIR src/libs/libtgvoip
#RUN ./build_and_install_linux.sh

#WORKDIR /src
#RUN ls
#RUN cmake ./
#RUN make -j8
#RUN ./tgvoipcall