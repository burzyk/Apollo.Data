namespace ShakaDB.Client
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Linq;
    using Wrapper;

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

        public static ShakaDbSession Open(string hostname, int port)
        {
            var session = new ShakaDbSession(hostname, port);
            CallWrapper(
                () => SdbWrapper.ShakaDbSessionOpen(ref session._session, hostname, port),
                $"{hostname}:{port}");
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
            EnsureNotDisposed();

            var content = dataPoints
                .Select(x => new SdbDataPoint {Time = x.Timestamp, Value = x.Value})
                .ToArray();

            CallWrapper(
                () => SdbWrapper.ShakaDbWritePoints(ref _session, seriesId, content, content.Length),
                $"Failed to write data to {seriesId}");
        }

        public IEnumerable<ShakaDbDataPoint> Read(
            uint seriesId,
            ulong? begin = null,
            ulong? end = null,
            int pointsPerPacket = 655360)
        {
            EnsureNotDisposed();

            begin = begin ?? Constants.ShakadbMinTimestamp;
            end = end ?? Constants.ShakadbMaxTimestamp;

            var iterator = new SdbDataPointsIterator();
            CallWrapper(
                () => SdbWrapper.ShakaDbReadPoints(
                    ref _session,
                    seriesId,
                    begin.Value,
                    end.Value,
                    pointsPerPacket,
                    ref iterator),
                $"Failed to read data: {seriesId}");
            return new PointsEnumerable(iterator);
        }

        public void Truncate(uint seriesId)
        {
            EnsureNotDisposed();
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
            var result = action();

            if (result == Constants.ShakadbResultConnectError)
            {
                throw new ShakaDbException($"Failed to connect: {message}");
            }

            if (result == Constants.ShakadbResultMultipleReadsError)
            {
                throw new ShakaDbException($"Multiple results active: {message}");
            }

            if (result != Constants.ShakadbResultOk)
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
                Current = new ShakaDbDataPoint(point.Time, point.Value);
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