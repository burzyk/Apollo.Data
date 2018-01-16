namespace ShakaDB.Client.Protocol
{
    public enum PacketType : byte
    {
        WriteRequest = 1,
        ReadRequest = 3,
        ReadResponse = 4,
        SimpleResponse = 2,
        TruncateRequest = 5,
        ReadLatestRequest = 6
    }
}