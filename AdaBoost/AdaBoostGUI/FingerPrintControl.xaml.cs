using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace AdaBoostGUI
{
    /// <summary>
    /// Interaction logic for FingerPrintControl.xaml
    /// </summary>
    public partial class FingerPrintControl : UserControl
    {
        public FingerPrintControl()
        {
            InitializeComponent();
        }

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr createFingerPrintMatcher();

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void deleteFingerPrintMatcher(IntPtr ptr);

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void trainFingerPrintOneImage(IntPtr ptr, string name, string fileName);

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr computeFingerFeatures(IntPtr ptr, string fileName, out int retSize);

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double testFingerPrintOneImage(IntPtr ptr, string name);
    }
}
