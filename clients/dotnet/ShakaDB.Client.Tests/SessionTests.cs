namespace ShakaDB.Client.Tests
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Xunit;

    public class SessionTests
    {
        [Fact]
        public async Task ServerRunningTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
            }
        }

        [Fact]
        public async Task ReuseClosedSessionTest()
        {
            var s = await ShakaDbSession.Open("localhost", 8487);
            s.Dispose();

            await Assert.ThrowsAsync<ObjectDisposedException>(async () => await s.Read(0, 0, 0));
            await Assert.ThrowsAsync<ObjectDisposedException>(async () => await s.Write(0, new List<DataPoint>()));
            await Assert.ThrowsAsync<ObjectDisposedException>(async () => await s.Truncate(0));
        }


        [Fact]
        public async Task ServerReadAndWriteIteatorTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
                await session.Write(TestConstants.UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var result = (await session.Read(TestConstants.UsdAud)).ToList();

                Assert.Equal(2, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(2, (int) result[1].Timestamp);
                Assert.Equal(12, result[0].ValueAsFloat);
                Assert.Equal(13, (int) result[1].ValueAsFloat);
            }
        }

        [Fact]
        public async Task ServerReadAndWriteWithIteatorAndLimitTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
                await session.Write(TestConstants.UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var result = (await session.Read(TestConstants.UsdAud, 0, 2)).ToList();

                Assert.Equal(1, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(12, result[0].ValueAsFloat);
            }
        }

        [Fact]
        public async Task ReadLatestNoDataTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
                var latest = await session.GetLatest(TestConstants.UsdAud);

                Assert.Null(latest);
            }
        }

        [Fact]
        public async Task ReadLatestTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
                await session.Write(TestConstants.UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13),
                    new DataPoint(3, 15)
                });

                var latest = await session.GetLatest(TestConstants.UsdAud);

                Assert.Equal(3, (int) latest.Timestamp);
                Assert.Equal(15, latest.ValueAsFloat);
            }
        }

        [Fact]
        public async Task ServerTruncateTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
                await session.Write(TestConstants.UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var result = (await session.Read(TestConstants.UsdAud, 0, 2)).ToList();

                Assert.Equal(1, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(12, result[0].ValueAsFloat);

                await session.Truncate(TestConstants.UsdAud);

                result = (await session.Read(TestConstants.UsdAud)).ToList();
                Assert.Equal(0, result.Count);
            }
        }

        [Fact]
        public async Task MultipleOpenedReadsTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(TestConstants.UsdAud);
                await session.Write(TestConstants.UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var read = await session.Read(TestConstants.UsdAud);
                await Assert.ThrowsAsync<ShakaDbException>(async () => await session.Read(TestConstants.UsdAud));

                read.ToList();
                (await session.Read(TestConstants.UsdAud)).ToList();
            }
        }
    }
}