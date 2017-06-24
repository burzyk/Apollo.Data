from ctypes import Structure, c_uint64, c_float, c_void_p, c_int, POINTER, cdll

import functools

from .common import Constants, ShakaDbError
import sys


class SdbDataPoint(Structure):
    _pack_ = 1
    _fields_ = [("time", c_uint64),
                ("value", c_float)]


class SdbSession(Structure):
    _fields_ = [("_session", c_void_p),
                ("_read_opened", c_int)]


class SdbDataPointsIterator(Structure):
    _fields_ = [("points", POINTER(SdbDataPoint)),
                ("points_count", c_int),
                ("_iterator", c_void_p),
                ("_session", c_void_p)]


class SafeInvoke(object):
    def __call__(self, func):
        @functools.wraps(func)
        def do_call(*args, **kwargs):
            result = func(*args, **kwargs)
            if result == Constants.SHAKADB_RESULT_CONNECT_ERROR:
                raise ShakaDbError('Unable to connect to the server')

            if result == Constants.SHAKADB_RESULT_MULTIPLE_READS_ERROR:
                raise ShakaDbError(
                    'There is an outstanding iterator. Only one read connection is allowed at a given time')

            if result != Constants.SHAKADB_RESULT_OK:
                raise ShakaDbError('ShakaDB method call failed')

            return result

        return do_call


_shakadb_lib = None


def _get_lib():
    global _shakadb_lib

    if _shakadb_lib is None:
        lib_name = 'libshakadbc.dylib' if sys.platform == 'darwin' else 'libshakadbc.so'

        try:
            _shakadb_lib = cdll.LoadLibrary(lib_name)
        except OSError:
            _shakadb_lib = cdll.LoadLibrary('/usr/local/lib/' + lib_name)

    return _shakadb_lib


@SafeInvoke()
def shakadb_session_open(session, server, port):
    return _get_lib().shakadb_session_open(session, server, port)


def shakadb_session_close(session):
    _get_lib().shakadb_session_close(session)


@SafeInvoke()
def shakadb_write_points(session, series_id, points, points_count):
    return _get_lib().shakadb_write_points(session, series_id, points, points_count)


@SafeInvoke()
def shakadb_truncate_data_series(session, series_id):
    return _get_lib().shakadb_truncate_data_series(session, series_id)


@SafeInvoke()
def shakadb_read_points(session, series_id, begin, end, iterator):
    return _get_lib().shakadb_read_points(session, series_id, begin, end, iterator)


def shakadb_data_points_iterator_next(iterator):
    return _get_lib().shakadb_data_points_iterator_next(iterator)
