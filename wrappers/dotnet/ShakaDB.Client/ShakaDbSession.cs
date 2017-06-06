namespace ShakaDB.Client
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Linq;
    using Wrapper;


//    class Session:
//        def __init__(self, hostname, port):
//        self.hostname = hostname
//        self.port = port
//        self._session = SdbSession()
//        shakadb_session_open(byref(self._session), hostname.encode('ASCII'), port)
//
//
//
//    @ensure_session_open
//    def read(self, series_id, begin, end):
//    it = SdbDataPointsIterator()
//    shakadb_read_points(byref(self._session), series_id, begin, end, byref(it))
//    return DataPointsIterator(it)
//
//    @ensure_session_open
//    def read_all(self, series_id, begin, end):
//    result = []
//    it = self.read(series_id, begin, end)
//
//    while it.next() != 0:
//    result += it.points()
//    return result
//


    public class ShakaDbSession : IDisposable
    {
        private SdbSession _session;

        protected ShakaDbSession(string hostname, int port)
        {
            Hostname = hostname;
            Port = port;
        }

        ~ShakaDbSession()
        {
            Dispose(false);
        }

        public string Hostname { get; }

        public int Port { get; }

        public bool IsDisposed { get; private set; }

        public ShakaDbSession Open(string hostname, int port)
        {
            var session = new ShakaDbSession(hostname, port);
            CallWrapper(() => SdbWrapper.ShakaDbSessionOpen(ref session._session, hostname, port), "Failed to connect");
            return session;
        }

        public void Dispose()
        {
            Dispose(true);
        }

        public void Close()
        {
            Dispose();
        }

        public void Write(uint seriesId, IEnumerable<ShakaDbDataPoint> dataPoints)
        {
            var content = dataPoints
                .Select(x => new SdbDataPoint {Time = x.Timestamp, Value = x.Value})
                .ToArray();

            CallWrapper(
                () => SdbWrapper.ShakaDbWritePoints(ref _session, seriesId, content, content.Length),
                $"Failed to write data to {seriesId}");
        }

        public IEnumerable<ShakaDbDataPoint> Read(uint seriesId, ulong begin, ulong end)
        {
            var iterator = new SdbDataPointsIterator();
            CallWrapper(
                () => SdbWrapper.ShakaDbReadPoints(ref _session, seriesId, begin, end, ref iterator),
                $"Failed to read data: {seriesId}");
            return new PointsEnumerable(iterator);
        }

        public void Truncate(uint seriesId)
        {
            CallWrapper(
                () => SdbWrapper.ShakaDbTruncateDataSeries(ref _session, seriesId),
                $"Failed to truncate the series: {seriesId}");
        }

        protected virtual void Dispose(bool disposing)
        {
            EnsureNotDisposed();
            IsDisposed = true;
            SdbWrapper.ShakaDbSessionClose(ref _session);
        }

        private static void CallWrapper(Func<int> action, string message)
        {
            if (action() != Constants.ShakadbResultOk)
            {
                throw new ShakaDbException($"Call failed: {message}");
            }
        }

        private void EnsureNotDisposed()
        {
            if (IsDisposed)
            {
                throw new ObjectDisposedException("Session has been disposed");
            }
        }

        private class PointsEnumerable : IEnumerable<ShakaDbDataPoint>
        {
            private readonly SdbDataPointsIterator _iterator;

            public PointsEnumerable(SdbDataPointsIterator iterator)
            {
                _iterator = iterator;
            }

            public IEnumerator<ShakaDbDataPoint> GetEnumerator()
            {
                return new PointsEnumerator(_iterator);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return GetEnumerator();
            }
        }

        private class PointsEnumerator : IEnumerator<ShakaDbDataPoint>
        {
            private SdbDataPointsIterator _iterator;

            private int _offset = int.MaxValue;

            public PointsEnumerator(SdbDataPointsIterator iterator)
            {
                _iterator = iterator;
            }

            public bool MoveNext()
            {
                if (_offset >= _iterator.PointsCount)
                {
                    _offset = 0;

                    if (SdbWrapper.ShakaDbDataPointsIteratorNext(ref _iterator) == 0)
                    {
                        return false;
                    }
                }

                var point = _iterator.ReadAt(_offset++);
                Current = new ShakaDbDataPoint {Timestamp = point.Time, Value = point.Value};
                return true;
            }

            public void Reset()
            {
                throw new NotSupportedException();
            }

            public ShakaDbDataPoint Current { get; private set; }

            object IEnumerator.Current => Current;

            public void Dispose()
            {
                while (SdbWrapper.ShakaDbDataPointsIteratorNext(ref _iterator) != 0)
                {
                    // read all remaining data and close the iterator
                }
            }
        }
    }
}