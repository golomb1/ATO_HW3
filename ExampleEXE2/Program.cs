
using System;
using System.IO;

namespace ExampleEXE2
{
    class Program
    {
        static void Main(string[] args)
        {
            using (StreamWriter file = new StreamWriter(new FileStream(@"C:\B\Log.txt",FileMode.OpenOrCreate)))
            {
               file.WriteLine(Environment.UserName);
            }
        }
    }
}