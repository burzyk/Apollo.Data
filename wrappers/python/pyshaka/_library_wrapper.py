from ctypes import Structure, c_uint64, c_float, c_void_p, c_int, POINTER, cdll
from .common import Constants, ShakaDbError
import sys


class SdbDataPoint(Structure):
    _pack_ = 1
    _fields_ = [("time", c_uint64),
                ("value", c_float)]


class SdbSession(Structure):
    _fields_ = [("_session", c_void_p)]


class SdbDataPointsIterator(Structure):
    _fields_ = [("points", POINTER(SdbDataPoint)),
                ("points_count", c_int),
                ("_iterator", c_void_p)]


def _safe_invoke(func):
    def do_safe_invoke(*args, **kwargs):
        result = func(*args, **kwargs)
        if result == Constants.SHAKADB_RESULT_ERROR:
            raise ShakaDbError('ShakaDB method call failed')
        return result

    return do_safe_invoke


_shakadb_lib = None


def _get_lib():
    global _shakadb_lib

    if _shakadb_lib is None:
        lib_name = 'libshakadbc.dylib' if sys.platform == 'darwin' else 'libshakadbc.so'
        _shakadb_lib = cdll.LoadLibrary('/usr/local/lib/' + lib_name)

    return _shakadb_lib


@_safe_invoke
def shakadb_session_open(session, server, port):
    return _get_lib().shakadb_session_open(session, server, port)


def shakadb_session_close(session):
    _get_lib().shakadb_session_close(session)


@_safe_invoke
def shakadb_write_points(session, series_id, points, points_count):
    return _get_lib().shakadb_write_points(session, series_id, points, points_count)


@_safe_invoke
def shakadb_truncate_data_series(session, series_id):
    return _get_lib().shakadb_truncate_data_series(session, series_id)


@_safe_invoke
def shakadb_read_points(session, series_id, begin, end, iterator):
    return _get_lib().shakadb_read_points(session, series_id, begin, end, iterator)


@_safe_invoke
def shakadb_data_points_iterator_next(iterator):
    return _get_lib().shakadb_data_points_iterator_next(iterator)
