namespace ShakaDB.Client
{
    using System;

    public class ShakaDbException : Exception
    {
        public ShakaDbException(string message)
            : base(message)
        {
        }
    }
}