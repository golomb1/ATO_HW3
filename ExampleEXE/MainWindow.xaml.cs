using System;

namespace ExampleEXE
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();
            bool isSystem;
            using (var identity = System.Security.Principal.WindowsIdentity.GetCurrent())
            {
                isSystem = identity.IsSystem;
            }
            TextB.Text = Environment.UserName + "->" + isSystem;
        }
    }
}
