using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace EvaluateExecutor
{
    public partial class EvaluateExecutor : ServiceBase
    {
        [DllImport(@"EvaluateExecutorDLL.dll", EntryPoint = "EvaluatorExecutorMain", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void EvaluatorExecutorMain();


        public EvaluateExecutor()
        {
            InitializeComponent();
            eventLog1 = new EventLog();
            if (!EventLog.SourceExists("EvaluateExecutorSource"))
            {
                EventLog.CreateEventSource(
                    "EvaluateExecutorSource", "EvaluateExecutorLog");
            }
            eventLog1.Source = "EvaluateExecutorSource";
            eventLog1.Log = "EvaluateExecutorLog";
        }

        protected override void OnStart(string[] args)
        {
            eventLog1.WriteEntry("In OnStart");
            ThreadStart start = new ThreadStart(Worker);
            worker = new Thread(start);
            worker.Start();
        }

        private int eventId = 0;
        private Thread worker;

        private void Worker()
        {
            eventLog1.WriteEntry("Start Application", EventLogEntryType.Information, eventId++);
            try
            {
                EvaluatorExecutorMain();
            }
            catch(Exception e)
            {
                eventLog1.WriteEntry(e.Message, EventLogEntryType.Information, eventId++);
            }
        }
        
        protected override void OnStop()
        {
            eventLog1.WriteEntry("In onStop.");
        }
    }
}
