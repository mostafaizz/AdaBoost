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

namespace Biometrics
{
    /// <summary>
    /// Interaction logic for ColorHistControl.xaml
    /// </summary>
    public partial class ColorHistControl : UserControl
    {
        public ColorHistControl()
        {
            InitializeComponent();
        }

        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr getColorHist(string imageName, out int retSize);


        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png");
            textInputFileName.Text = fileName;
            origImage.Source = MainWindow.getImageFromFile(textInputFileName.Text);
        }

        private void applyColorHistogram_Click(object sender, RoutedEventArgs e)
        {
            int retSize = 0;
            IntPtr oup = getColorHist(textInputFileName.Text, out retSize);
            outImage.Source = MainWindow.getImageFromIntPtr(oup, retSize);
        }

        private void origImage_MouseDown(object sender, MouseButtonEventArgs e)
        {
            ViolaJonesTester.image_MouseDown(sender, e);

        }
    }
}
