import socket
import struct as s

SERVER_MAGIC = 0x4B414D41

RESPONSE_OK = 0
RESPONSE_ERROR = 1

WRITE_REQUEST = 1
READ_REQUEST = 3
READ_RESPONSE = 4
SIMPLE_RESPONSE = 2
TRUNCATE_REQUEST = 5
READ_LATEST_REQUEST = 6


class ShakaDbError(Exception):
    def __init__(self, message):
        self.message = message


def ensure_reading_closed(func):
    def do_ensure_reading_closed(self, *args, **kwargs):
        if self.is_reading_open():
            raise ShakaDbError("Multiple active reads are not supported")

        return func(self, *args, **kwargs)

    return do_ensure_reading_closed


class Session:
    def __init__(self, hostname='localhost', port=8487):
        self._reading_open = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.socket.connect((socket.gethostbyname(hostname), port))

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def close(self):
        self.socket.shutdown(socket.SHUT_RDWR)
        self.socket.close()

    @ensure_reading_closed
    def write(self, series_id, points):
        payload = s.pack('II', series_id, len(points))

        for dp in points:
            payload = payload + s.pack('Qf', dp[0], dp[1])

        self._send(payload, WRITE_REQUEST)
        self._assert_simple_response()

    @ensure_reading_closed
    def latest(self, series_id):
        self._send(s.pack('I', series_id), READ_LATEST_REQUEST)
        points = self._read_points()
        return None if len(points) == 0 else points[0]

    @ensure_reading_closed
    def read(self, series_id, begin, end, points_per_packet=65536):
        # turns out pack was returning IQQI -> size 28, IQ is packed incorrectly but QI is fine
        payload = s.pack('I', series_id) + s.pack('QQ', begin, end) + s.pack('I', points_per_packet)
        self._send(payload, READ_REQUEST)
        self._reading_open = True

        while True:
            points = self._read_points()

            if len(points) == 0:
                self._reading_open = False
                break

            for p in points:
                yield p

    @ensure_reading_closed
    def truncate(self, series_id):
        self._send(s.pack('I', series_id), TRUNCATE_REQUEST)
        self._assert_simple_response()

    def is_reading_open(self):
        return self._reading_open

    def _read_points(self):
        payload = self._receive(READ_RESPONSE)
        details = s.unpack('I', payload[:4])
        points = []
        points_count = details[0]

        for i in range(0, points_count):
            p = s.unpack_from('Qf', payload, 4 + 12 * i)
            points.append((p[0], p[1]))

        return points

    def _assert_simple_response(self):
        details = s.unpack('B', self._receive(SIMPLE_RESPONSE))

        if details[0] != RESPONSE_OK:
            raise ShakaDbError("Response does not indicate success")

    def _send(self, payload, packet_type):
        hdr = s.pack('IIB', SERVER_MAGIC, 8 + 1 + len(payload), packet_type)
        self.socket.send(hdr)
        self.socket.send(payload)

    def _receive(self, packet_type):
        hdr = s.unpack('IIB', self._recv_all(9))

        if hdr[0] != SERVER_MAGIC:
            raise ShakaDbError("The server didn't respond with the correct packet")

        if hdr[2] != packet_type:
            raise ShakaDbError("Invalid operation type")

        return self._recv_all(hdr[1] - 9)

    def _recv_all(self, size):
        buffer = b""
        while len(buffer) < size:
            data = self.socket.recv(size - len(buffer))
            if not data:
                break
            buffer += data

        if len(buffer) != size:
            raise ShakaDbError("Failed to read data")

        return buffer


if __name__ == "__main__":
    session = Session()
