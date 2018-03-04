from pytest import raises

import socket
import pyshaka


class TestConstants:
    USD_AUD = 1


def test_server_running():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)


def test_invalid_server():
    with raises(socket.gaierror):
        pyshaka.Session('blah.blah', 8487)


def test_multiple_open_iterators():
    with raises(pyshaka.ShakaDbError):
        s = pyshaka.Session('localhost', 8487)
        s.write(TestConstants.USD_AUD, [(1, 12), (2, 13)])
        f = next(s.read(TestConstants.USD_AUD, 0, 1000))
        s = list(s.read(TestConstants.USD_AUD, 0, 1000))


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
        it = session.read(TestConstants.USD_AUD, 0, 10)
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


def test_server_truncate():
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


def test_server_multiple_data_types():
    with pyshaka.Session('localhost', 8487) as session:
        for i in [101, 102, 103, 104, 105]:
            session.truncate(i)

        session.write(101, [(1, 3.14), (2, 5.66)], dp_type='double')
        session.write(102, [(1, 4.14), (2, 9.66)], dp_type='float')
        session.write(103, [(1, 87), (2, 99)], dp_type='int32')
        session.write(104, [(1, 870), (2, 990)], dp_type='int64')
        session.write(105, [(1, 'ala ma kota'), (2, 'kamis ma pieska')], dp_type='string', point_size=128)

        double = list(session.read(101, 0, 100, dp_type='double'))
        single = list(session.read(102, 0, 100, dp_type='float'))
        int32 = list(session.read(103, 0, 100, dp_type='int32'))
        int64 = list(session.read(104, 0, 100, dp_type='int64'))
        string = list(session.read(105, 0, 100, dp_type='string'))

        assert len(double) == 2
        assert len(single) == 2
        assert len(int32) == 2
        assert len(int64) == 2
        assert len(string) == 2

        assert double[0][1] == 3.14
        assert double[1][1] == 5.66

        assert abs(single[0][1] - 4.14) < 10e-4
        assert abs(single[1][1] - 9.66) < 10e-4

        assert int32[0][1] == 87
        assert int32[1][1] == 99

        assert int64[0][1] == 870
        assert int64[1][1] == 990

        assert string[0][1] == 'ala ma kota'
        assert string[1][1] == 'kamis ma pieska'


def test_server_multiple_batches():
    with pyshaka.Session('localhost', 8487) as session:
        session.truncate(TestConstants.USD_AUD)

        data = [(i, i + 100) for i in range(1, 100000 * 2 + 5)]
        session.write(TestConstants.USD_AUD, data, dp_type='int32')
        response = list(session.read(TestConstants.USD_AUD, 0, 10000000000, dp_type='int32'))

        for i in zip(data, response):
            assert i[0] == i[1]


if __name__ == '__main__':
    test_server_read_and_write_with_iterator_and_limit()
