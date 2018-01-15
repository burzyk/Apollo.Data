namespace ShakaDB.Client
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Net.Sockets;
    using System.Threading.Tasks;
    using Protocol;

    public class ShakaDbSession : IDisposable
    {
        private readonly Stream _stream;

        public ShakaDbSession(Stream stream)
        {
            _stream = stream;
        }

        public static async Task<ShakaDbSession> Open(string hostname, int port)
        {
            var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            await socket.ConnectAsync(hostname, port);

            return new ShakaDbSession(new NetworkStream(socket, true));
        }

        public void Dispose()
        {
            _stream.Dispose();
        }

        public async Task Write(uint seriesId, IEnumerable<DataPoint> dataPoints)
        {
            await WithSimpleResponse(new WriteRequest(seriesId, dataPoints), "Failed to write points");
        }

        public async Task<DataPoint> GetLatest(uint seriesId)
        {
            var request = new ReadLatestRequest(seriesId);
            await Transmitter.Write(request.Serialize(), _stream);

            var response = new ReadResponse(await Transmitter.Read(_stream));

            if (!response.Points.Any())
            {
                return null;
            }

            // read the end of transmission
            await Transmitter.Read(_stream);
            return response.Points.Single();
        }

        public async Task<IEnumerable<DataPoint>> Read(
            uint seriesId,
            ulong? begin = null,
            ulong? end = null,
            int pointsPerPacket = 655360)
        {
            var request = new ReadRequest(
                seriesId,
                begin ?? Constants.ShakadbMinTimestamp,
                end ?? Constants.ShakadbMaxTimestamp,
                pointsPerPacket);

            await Transmitter.Write(request.Serialize(), _stream);

            return Enumerable.Range(0, int.MaxValue)
                .Select(async x => await Transmitter.Read(_stream))
                .Select(x => new ReadResponse(x.Result))
                .TakeWhile(x => x.Points.Any())
                .SelectMany(x => x.Points);
        }

        public async Task Truncate(uint seriesId)
        {
            await WithSimpleResponse(new TruncateRequest(seriesId), "Failed to truncate data series");
        }

        private async Task WithSimpleResponse(BasePacket request, string errorMessage)
        {
            await Transmitter.Write(request.Serialize(), _stream);
            var response = new SimpleResponse(await Transmitter.Read(_stream));

            if (response.ResponseCode == ResponseCode.Failure)
            {
                throw new ShakaDbException(errorMessage);
            }
        }
    }
}