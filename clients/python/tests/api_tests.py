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


def test_multiple_open_iterators():
    with raises(pyshaka.ShakaDbError):
        s = pyshaka.Session('localhost', 8487)
        s.read(0, 100, 1000)
        s.read(0, 100, 1000)


def test_server_read_and_write_with_iterator():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        it = session.read(TestConstants.USD_AUD, 0, 10)
        points = list(it)

        assert len(points) == 2
        assert points[0][0] == 1
        assert points[1][0] == 2
        assert points[0][1] == 12
        assert points[1][1] == 13


def test_server_read_and_write_with_iterator_and_points_per_packet_limit():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13), (3, 14)])
        it = session.read(TestConstants.USD_AUD, 0, 10, 2)
        points = list(it)

        assert len(points) == 3


def test_server_read_and_write_with_iterator_and_limit():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        it = session.read(TestConstants.USD_AUD, 0, 2)
        points = list(it)

        assert len(points) == 1
        assert points[0][0] == 1
        assert points[0][1] == 12


def test_read_latest_no_data():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        assert session.latest(TestConstants.USD_AUD) is None


def test_read_latest():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])

        latest = session.latest(TestConstants.USD_AUD)

        assert latest[0] == 2
        assert latest[1] == 13


def test_server_read_and_write_and_limit():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        points = list(session.read(TestConstants.USD_AUD, 0, 2))

        assert len(points) == 1
        assert points[0][0] == 1
        assert points[0][1] == 12


def test_server_truncate_test():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)
        session.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        points = list(session.read(TestConstants.USD_AUD, 0, 2))

        assert len(points) == 1
        assert points[0][0] == 1
        assert points[0][1] == 12

        session.truncate(TestConstants.USD_AUD)
        points = list(session.read(TestConstants.USD_AUD, 0, 2))

        assert len(points) == 0


if __name__ == '__main__':
    test_server_read_and_write_with_iterator_and_points_per_packet_limit()
