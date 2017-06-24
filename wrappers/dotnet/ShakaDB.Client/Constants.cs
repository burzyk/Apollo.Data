namespace ShakaDB.Client
{
    internal class Constants
    {
        public const ulong ShakadbMaxTimestamp = 0xFFFFFFFFFFFFFFFF;

        public const ulong ShakadbMinTimestamp = 1;

        public const int ShakadbResultOk = 0;

        public const int ShakadbResultGenericError = -1;

        public const int ShakadbResultConnectError = -2;

        public const int ShakadbResultMultipleReadsError = -3;
    }
}