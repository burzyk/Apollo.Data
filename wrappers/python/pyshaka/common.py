class Constants:
    SHAKADB_MAX_TIMESTAMP = 0xFFFFFFFFFFFFFFFF
    SHAKADB_MIN_TIMESTAMP = 1
    SHAKADB_RESULT_OK = 0
    SHAKADB_RESULT_GENERIC_ERROR = -1
    SHAKADB_RESULT_CONNECT_ERROR = -2


class ShakaDbError(Exception):
    def __init__(self, message):
        self.message = message


class SessionClosedError(ShakaDbError):
    def __init__(self):
        self.message = 'The session has been closed'
