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
    /// Interaction logic for FiltersControl.xaml
    /// </summary>
    public partial class FiltersControl : UserControl
    {
        public FiltersControl()
        {
            InitializeComponent();
        }

        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png");
            textInputFileName.Text = fileName;
            origImage.Source = MainWindow.getImageFromFile(textInputFileName.Text);
        }


        enum Operators
        {
            SOBEL3,
            SOBEL5,
            MEDIAN5,
            MEDIAN9,
            GAUSS5,
            GAUSS9
        };

        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr applyOperator(string imageName, [MarshalAs(UnmanagedType.LPArray)] int[] ops, int opsLength, out int retSize);

        private void sobel3_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
