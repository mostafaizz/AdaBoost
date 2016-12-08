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
    /// Interaction logic for GrayHistControl.xaml
    /// </summary>
    public partial class GrayHistControl : UserControl
    {
        public GrayHistControl()
        {
            InitializeComponent();
        }

        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr getGrayHist(string imageName, out int retSize);


        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png");
            textInputFileName.Text = fileName;
            origImage.Source = MainWindow.getImageFromFile(textInputFileName.Text);
        }

        private void applyColorHistogram_Click(object sender, RoutedEventArgs e)
        {
            int retSize = 0;
            IntPtr oup = getGrayHist(textInputFileName.Text, out retSize);
            outImage.Source = MainWindow.getImageFromIntPtr(oup, retSize);
        }
    }
}
