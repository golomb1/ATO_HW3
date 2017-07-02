using System;
using System.IO;

namespace ExampleConsoleEXE
{
    class Program
    {
        static void Main(string[] args)
        {
            bool isSystem;
            using (var identity = System.Security.Principal.WindowsIdentity.GetCurrent())
            {
                isSystem = identity.IsSystem;
            }
            using (StreamWriter file = new StreamWriter(new FileStream(@"C:\B\Log.txt", FileMode.OpenOrCreate)))
            {
                file.WriteLine(Environment.UserName + " -> " + isSystem);
            }
        }
    }
}
