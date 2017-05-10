class Constants:
    SHAKADB_MAX_TIMESTAMP = 0xFFFFFFFFFFFFFFFF
    SHAKADB_MIN_TIMESTAMP = 1
    SHAKADB_RESULT_OK = 0
    SHAKADB_RESULT_ERROR = -1


class ShakaDbError(Exception):
    def __init__(self, message):
        self.message = message
