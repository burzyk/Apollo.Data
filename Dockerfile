FROM alpine

ARG version
ARG build_number

ENV SDB_VERSION=$version
ENV SDB_BUILD=$build_number
ENV SDB_CONFIGURATION release

RUN mkdir /opt
RUN mkdir /opt/shakadb
RUN mkdir /var/lib/shakadb

COPY build/source-latest.bz2 /root

RUN apk add --no-cache cmake gcc make musl-dev libuv-dev

RUN cd /root && \
    tar -jxf source-latest.bz2 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    cp shakadb /opt/shakadb

RUN apk del --no-cache cmake gcc make musl-dev libuv-dev
RUN apk add --no-cache libuv
RUN rm -rf /root/*

VOLUME /var/lib/shakadb
EXPOSE 8487

CMD [ "/bin/ash", "-c", "/opt/shakadb/shakadb" ]