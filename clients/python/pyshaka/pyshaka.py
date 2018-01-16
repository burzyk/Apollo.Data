from ctypes import byref

from .common import SessionClosedError
from .library_wrapper import (
    shakadb_data_points_iterator_next,
    shakadb_session_open,
    shakadb_session_close,
    SdbDataPoint,
    shakadb_write_points,
    shakadb_truncate_data_series,
    SdbDataPointsIterator,
    shakadb_read_points,
    shakadb_read_latest_point,
    SdbSession)


def ensure_session_open(func):
    def do_ensure_session_open(self, *args, **kwargs):
        if not self.is_open():
            raise SessionClosedError

        return func(self, *args, **kwargs)

    return do_ensure_session_open


class DataPointsIterator:
    def __init__(self, iterator):
        self._iterator = iterator
        self._points = []

    def next(self):
        result = shakadb_data_points_iterator_next(byref(self._iterator))
        self._points = []

        if result != 0:
            for i in range(0, self._iterator.points_count):
                self._points.append((self._iterator.points[i].time, self._iterator.points[i].value))

        return result

    def points(self):
        return self._points


class Session:
    def __init__(self, hostname, port):
        self.hostname = hostname
        self.port = port
        self._session = SdbSession()
        shakadb_session_open(byref(self._session), hostname.encode('ASCII'), port)

    @ensure_session_open
    def close(self):
        shakadb_session_close(byref(self._session))
        self._session = None

    @ensure_session_open
    def write(self, series_id, points):
        points_count = len(points)
        points_raw = (SdbDataPoint * points_count)()

        for i in range(points_count):
            points_raw[i] = SdbDataPoint(points[i][0], points[i][1])

        shakadb_write_points(byref(self._session), series_id, points_raw, points_count)

    @ensure_session_open
    def truncate(self, series_id):
        shakadb_truncate_data_series(byref(self._session), series_id)

    @ensure_session_open
    def read(self, series_id, begin, end, points_per_packet=655360):
        it = SdbDataPointsIterator()
        shakadb_read_points(byref(self._session), series_id, begin, end, points_per_packet, byref(it))
        return DataPointsIterator(it)

    @ensure_session_open
    def latest(self, series_id):
        latest = SdbDataPoint()
        shakadb_read_latest_point(byref(self._session), series_id, byref(latest))
        return (latest.time, latest.value) if latest.time != 0 else None

    @ensure_session_open
    def read_all(self, series_id, begin, end):
        result = []
        it = self.read(series_id, begin, end, 655360)

        while it.next() != 0:
            result += it.points()
        return result

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def is_open(self):
        return self._session is not None
