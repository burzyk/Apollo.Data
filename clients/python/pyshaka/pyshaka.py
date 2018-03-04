import socket
import struct as s

SERVER_MAGIC = 0x4B414D41

MAX_POINTS_PER_PACKET = 100000

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
    def write(self, series_id, points, dp_type='double', point_size=None):
        pages_source = range(0, len(points) + MAX_POINTS_PER_PACKET, MAX_POINTS_PER_PACKET)
        pages = [points[x:x + MAX_POINTS_PER_PACKET] for x in pages_source]
        pages = filter(lambda x: x != [], pages)

        for page in pages:
            self._write_batch(series_id, page, dp_type, point_size)

    def _write_batch(self, series_id, points, dp_type='double', point_size=None):
        type_spec = {
            'double': (8 + 8, 'd'),
            'float': (4 + 8, 'f'),
            'int32': (4 + 8, 'i'),
            'int64': (8 + 8, 'q'),
            'string': (None, None)
        }[dp_type]

        point_size = point_size if point_size is not None else type_spec[0]

        if point_size is None:
            raise ShakaDbError('Unknown point size')

        payload = s.pack('=IQI', series_id, len(points), point_size)

        for dp in points:
            payload = payload + s.pack('Q', dp[0])

            if type_spec[0] is None:
                bytes_value = dp[1].encode('ASCII')
                # this is for padding
                bytes_value = bytes_value + b"\0" * (point_size - len(bytes_value) - 8)
                payload = payload + bytes_value
            else:
                payload = payload + s.pack(type_spec[1], dp[1])

        self._send(payload, WRITE_REQUEST)
        self._assert_simple_response()

    @ensure_reading_closed
    def latest(self, series_id, dp_type='double'):
        self._send(s.pack('I', series_id), READ_LATEST_REQUEST)
        points = self._read_points(dp_type)
        return None if len(points) == 0 else points[0]

    @ensure_reading_closed
    def read(self, series_id, begin, end, dp_type='double'):
        payload = s.pack('=IQQ', series_id, begin, end)
        self._send(payload, READ_REQUEST)
        self._reading_open = True

        while True:
            points = self._read_points(dp_type)

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

    def _read_points(self, dp_type):
        type_spec = {
            'double': 'd',
            'float': 'f',
            'int32': 'i',
            'int64': 'q',
            'string': 's'
        }[dp_type]

        payload = self._receive(READ_RESPONSE)
        (points_count, point_size) = s.unpack('=QI', payload[:12])
        points = []

        for i in range(0, points_count):
            offset = 12 + point_size * i
            time = s.unpack_from('Q', payload, offset)[0]

            if type_spec != 's':
                value = s.unpack_from(type_spec, payload, offset + 8)[0]
            else:
                value = payload[offset + 8:offset + point_size].decode('ASCII').strip(b"\00".decode('ASCII'))

            points.append((time, value))

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
    # session.write(1, [(1, 3.14), (2, 6.77)])
    session.truncate(99)
    session.write(99, [(1, 'ala ma kota'), (2, 'kamis ma pieska'), (3, 'ola ma asa')], 'string', 100)
    data = list(session.read(99, 0, 100, dp_type='string'))
    print(data)
