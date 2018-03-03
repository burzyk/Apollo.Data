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

        public static Task<ShakaDbSession> Open(Uri url)
        {
            return Open(url.Host, url.Port);
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
            // TODO: send paged
            await WithSimpleResponse(Packet.WriteRequest(seriesId, dataPoints.ToList()), "Failed to write points");
        }

        public async Task<DataPoint> GetLatest(uint seriesId)
        {
            EnsurePreConditions();

            await Transmitter.Send(Packet.ReadLatestRequest(seriesId), _stream);
            var response = Packet.ReadResponse(await Transmitter.Receive(_stream));

            return response.SingleOrDefault();
        }

        public async Task<IEnumerable<DataPoint>> Read(
            uint seriesId,
            ulong? begin = null,
            ulong? end = null)
        {
            EnsurePreConditions();

            _readOpen = true;

            var request = Packet.ReadRequest(
                seriesId,
                begin ?? Constants.ShakadbMinTimestamp,
                end ?? Constants.ShakadbMaxTimestamp);

            await Transmitter.Send(request, _stream);

            return Enumerable.Range(0, int.MaxValue)
                .Select(async x => await Transmitter.Receive(_stream))
                .Select(x => Packet.ReadResponse(x.Result))
                .TakeWhile(x => x.Any())
                .SelectMany(x => x)
                .Concat(new[] {new DataPoint(0, new byte[] { })}.Select(x =>
                {
                    _readOpen = false;
                    return x;
                }))
                .Where(x => x.Timestamp != 0);
        }

        public async Task Truncate(uint seriesId)
        {
            await WithSimpleResponse(Packet.TruncateRequest(seriesId), "Failed to truncate data series");
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

        private async Task WithSimpleResponse(byte[] request, string errorMessage)
        {
            EnsurePreConditions();

            await Transmitter.Send(request, _stream);
            var responseCode = Packet.ReadSimpleResponse(await Transmitter.Receive(_stream));

            if (responseCode == ResponseCode.Failure)
            {
                throw new ShakaDbException(errorMessage);
            }
        }
    }
}