FROM alpine

RUN mkdir /opt/shakadb
RUN mkdir /var/lib/shakadb

COPY build/bin/shakadb /opt/shakadb

VOLUME /var/lib/shakadb

CMD [ "sh", "-c", "/usr/local/bin/shakadb" ]