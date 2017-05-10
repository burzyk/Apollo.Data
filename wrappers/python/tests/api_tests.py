from pytest import raises

import pyshaka


class TestConstants:
    USD_AUD = 1


def test_server_running():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)


def test_invalid_server():
    with raises(pyshaka.ShakaDbError):
        pyshaka.Session('blah.blah', 8487)


def test_server_read_and_write_with_iterator():
    with pyshaka.Session('localhost', 8487) as session:
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        it = session.read(
            TestConstants.USD_AUD,
            pyshaka.Constants.SHAKADB_MIN_TIMESTAMP,
            pyshaka.Constants.SHAKADB_MAX_TIMESTAMP)
        points = []

        while it.next() != 0:
            points = points + it.points()

        assert len(points) == 2
        assert points[0][0] == 1
        assert points[1][0] == 2
        assert points[0][1] == 12
        assert points[1][1] == 13


def test_server_read_and_write_with_iterator_and_limit():
    with pyshaka.Session('localhost', 8487) as session:
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        it = session.read(TestConstants.USD_AUD, 0, 2)
        points = []

        while it.next() != 0:
            points = points + it.points()

        assert len(points) == 1
        assert points[0][0] == 1
        assert points[0][1] == 12


def test_server_read_and_write():
    with pyshaka.Session('localhost', 8487) as session:
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        points = session.read_all(
            TestConstants.USD_AUD,
            pyshaka.Constants.SHAKADB_MIN_TIMESTAMP,
            pyshaka.Constants.SHAKADB_MAX_TIMESTAMP)

        assert len(points) == 2
        assert points[0][0] == 1
        assert points[1][0] == 2
        assert points[0][1] == 12
        assert points[1][1] == 13


def test_server_read_and_write_and_limit():
    with pyshaka.Session('localhost', 8487) as session:
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        points = session.read_all(TestConstants.USD_AUD, 0, 2)

        assert len(points) == 1
        assert points[0][0] == 1
        assert points[0][1] == 12


def test_server_truncate_test():
    with pyshaka.Session('localhost', 8487) as session:
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        points = session.read_all(TestConstants.USD_AUD, 0, 2)

        assert len(points) == 1
        assert points[0][0] == 1
        assert points[0][1] == 12

        session.truncate(TestConstants.USD_AUD)
        points = session.read_all(TestConstants.USD_AUD, 0, 2)

        assert len(points) == 0


if __name__ == '__main__':
    test_server_read_and_write_with_iterator()
