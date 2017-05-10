from ctypes import byref

from ._library_wrapper import (
    shakadb_data_points_iterator_next,
    shakadb_session_open,
    shakadb_session_close,
    SdbDataPoint,
    shakadb_write_points,
    shakadb_truncate_data_series,
    SdbDataPointsIterator,
    shakadb_read_points,
    SdbSession)


class DataPointsIterator:
    def __init__(self, iterator):
        self._iterator = iterator
        self._points = []

    def next(self):
        result = shakadb_data_points_iterator_next(byref(self._iterator))

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

    def close(self):
        shakadb_session_close(byref(self._session))

    def write(self, series_id, points):
        points_count = len(points)
        points_raw = (SdbDataPoint * points_count)()

        for i in range(points_count):
            points_raw[i] = SdbDataPoint(points[i][0], points[i][1])

        shakadb_write_points(byref(self._session), series_id, points_raw, points_count)

    def truncate(self, series_id):
        shakadb_truncate_data_series(byref(self._session), series_id)

    def read(self, series_id, begin, end):
        it = SdbDataPointsIterator()
        shakadb_read_points(byref(self._session), series_id, begin, end, byref(it))
        return DataPointsIterator(it)

    def read_all(self, series_id, begin, end):
        result = []
        it = self.read(series_id, begin, end)

        while it.next() != 0:
            result += it.points()
        return result

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()
