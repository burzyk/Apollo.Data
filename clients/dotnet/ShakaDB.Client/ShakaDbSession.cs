namespace ShakaDB.Client
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Net.Sockets;
    using System.Threading.Tasks;
    using Protocol;

    public class ShakaDbSession : IDisposable
    {
        private readonly Stream _stream;
        private bool _readOpen;
        private bool _disposed;

        public ShakaDbSession(Stream stream)
        {
            _stream = stream;
        }

        public static async Task<ShakaDbSession> Open(string hostname, int port)
        {
            var client = new TcpClient
            {
                ReceiveTimeout = 10000,
                SendTimeout = 10000
            };

            await client.ConnectAsync(hostname, port);
            return new ShakaDbSession(client.GetStream());
        }

        public void Dispose()
        {
            _stream.Dispose();
            _disposed = true;
        }

        public async Task Write(uint seriesId, IEnumerable<DataPoint> dataPoints)
        {
            await WithSimpleResponse(new WriteRequest(seriesId, dataPoints), "Failed to write points");
        }

        public async Task<DataPoint> GetLatest(uint seriesId)
        {
            EnsurePreConditions();

            var request = new ReadLatestRequest(seriesId);
            await Transmitter.Send(request.Serialize(), _stream);

            var response = new ReadResponse(await Transmitter.Receive(_stream));

            return response.Points.SingleOrDefault();
        }

        public async Task<IEnumerable<DataPoint>> Read(
            uint seriesId,
            ulong? begin = null,
            ulong? end = null,
            int pointsPerPacket = 655360)
        {
            EnsurePreConditions();

            _readOpen = true;

            var request = new ReadRequest(
                seriesId,
                begin ?? Constants.ShakadbMinTimestamp,
                end ?? Constants.ShakadbMaxTimestamp,
                pointsPerPacket);

            await Transmitter.Send(request.Serialize(), _stream);

            return Enumerable.Range(0, int.MaxValue)
                .Select(async x => await Transmitter.Receive(_stream))
                .Select(x => new ReadResponse(x.Result))
                .TakeWhile(x => x.Points.Any())
                .SelectMany(x => x.Points)
                .Concat(new[] {new DataPoint(0, 0)}.Select(x =>
                {
                    _readOpen = false;
                    return x;
                }))
                .Where(x => x.Timestamp != 0);
        }

        public async Task Truncate(uint seriesId)
        {
            await WithSimpleResponse(new TruncateRequest(seriesId), "Failed to truncate data series");
        }

        private void EnsurePreConditions()
        {
            if (_disposed)
            {
                throw new ObjectDisposedException("This object has been disposed");
            }

            if (_readOpen)
            {
                throw new ShakaDbException("Multiple active reads are not alliwed");
            }
        }

        private async Task WithSimpleResponse(BasePacket request, string errorMessage)
        {
            EnsurePreConditions();

            await Transmitter.Send(request.Serialize(), _stream);
            var response = new SimpleResponse(await Transmitter.Receive(_stream));

            if (response.ResponseCode == ResponseCode.Failure)
            {
                throw new ShakaDbException(errorMessage);
            }
        }
    }
}