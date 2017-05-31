namespace ShakaDB.Client.Wrapper
{
    using System.Runtime.InteropServices;

    internal class SdbWrapper
    {
        private const string ShakaLib = "libshakadbc";

        [DllImport(ShakaLib, EntryPoint = "shakadb_session_open")]
        public extern int ShakaDbSessionOpen(ref SdbSession session, string hostname, int port);

        [DllImport(ShakaLib, EntryPoint = "shakadb_session_close")]
        public extern void ShakaDbSessionClose(ref SdbSession session);

        [DllImport(ShakaLib, EntryPoint = "shakadb_write_points")]
        public extern int ShakaDbWritePoints(
            ref SdbSession session,
            uint seriesId,
            SdbDataPoint[] points,
            int pointCount);

        [DllImport(ShakaLib, EntryPoint = "shakadb_truncate_data_series")]
        public extern int ShakaDbTruncateDataSeries(ref SdbSession session, uint seriesId);

        [DllImport(ShakaLib, EntryPoint = "shakadb_read_points")]
        public extern int ShakaDbReadPoints(
            ref SdbSession session,
            uint seriesId,
            ulong begin,
            ulong end,
            ref SdbDataPointsIterator iterator);

        [DllImport(ShakaLib, EntryPoint = "shakadb_data_points_iterator_next")]
        public extern int ShakaDbDataPointsIteratorNext(ref SdbDataPointsIterator iterator);
    }
}