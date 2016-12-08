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
    /// Interaction logic for IrisControl.xaml
    /// </summary>
    public partial class IrisControl : UserControl
    {
        public IrisControl()
        {
            InitializeComponent();
        }

        IntPtr iRec = IntPtr.Zero; // iris recognizer

        [DllImport("opencv31_2015.dll", SetLastError = true)]
        public static extern IntPtr createIrisRecognizer(int w, int h);

        [DllImport("opencv31_2015.dll", SetLastError = true)]
        public static extern void extractIris(string imgFileName, IntPtr iRec,
            out IntPtr stripData, out int stripSize,
            out IntPtr origData, out int origSize);

        [DllImport("opencv31_2015.dll", SetLastError = true)]
        public static extern void extractIrisFeatures(IntPtr iRec, out IntPtr codeData, out int codeSize);

        [DllImport("opencv31_2015.dll", SetLastError = true)]
        public static extern void deleteIrisRecognizer(IntPtr iRec);

        private void OpenImage_Click(object sender, RoutedEventArgs e)
        {
            string imageFileName = MainWindow.openFile("PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg");
            if (imageFileName != null)
            {
                origIrisImage.Source = MainWindow.getImageFromFile(imageFileName);
                origIrisImage.ToolTip = imageFileName;
                origIrisImageIrisExtracted.Source = null;
                IrisNormalized.Source = null;
                IrisCode.Source = null;
            }
        }

        private void FindIris_Click(object sender, RoutedEventArgs e)
        {
            if (iRec == IntPtr.Zero)
            {
                iRec = createIrisRecognizer(512, 64);
            }
            IntPtr stripData = IntPtr.Zero, origData = IntPtr.Zero;
            int stripSize = 0, dataSize = 0;
            extractIris((string)(origIrisImage.ToolTip), iRec, out stripData, out stripSize, out origData, out dataSize);
            origIrisImageIrisExtracted.Source = MainWindow.getImageFromIntPtr(origData, dataSize);
            IrisNormalized.Source = MainWindow.getImageFromIntPtr(stripData, stripSize);
        }

        private void ExtractIrisFeatures_Click(object sender, RoutedEventArgs e)
        {
            if (iRec == IntPtr.Zero)
            {
                FindIris_Click(sender, e);
            }
            IntPtr codeData = IntPtr.Zero;
            int codeSize = 0;
            extractIrisFeatures(iRec, out codeData, out codeSize);
            IrisCode.Source = MainWindow.getImageFromIntPtr(codeData, codeSize);
        }
    }
}
