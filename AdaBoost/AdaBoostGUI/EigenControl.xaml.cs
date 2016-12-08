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
    /// Interaction logic for EigenControl.xaml
    /// </summary>
    public partial class EigenControl : UserControl
    {
        public EigenControl()
        {
            InitializeComponent();
        }

        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("CSV file|*.csv");
            textInputFileName.Text = fileName;
        }

        private void buttonPCAModelName_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.saveFile("xml file|*.xml");
            textPCAModelName.Text = fileName;
        }

        private void buttonHaarModelName_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("xml file|*.xml");
            textHaarModelName.Text = fileName;
        }

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        static public extern void learnEigenface(string inputfilename, string pcamodel, string face_cascade_name, int nEigen = 0);

        private void buttonTrainEigen_Click(object sender, RoutedEventArgs e)
        {
            int eig = Convert.ToInt32(0);
            learnEigenface(textInputFileName.Text, textPCAModelName.Text, textHaarModelName.Text, eig);
        }

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        static public extern void getEigens(string pcamodel, out IntPtr eigenface, out int size);

        private void buttonDrawEigen_Click(object sender, RoutedEventArgs e)
        {
            IntPtr ptr;
            int size;
            getEigens(textPCAModelName.Text, out ptr, out size);
            BitmapImage img = MainWindow.getImageFromIntPtr(ptr, (int)size);
            ImageEigenFaces.Source = img;
        }

        private void buttonInputFileNameTest_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png");
            textInputFileNameTest.Text = fileName;
            ImageFacesTest.Source = MainWindow.getImageFromFile(textInputFileNameTest.Text);
        }

        private void buttonPCAModelNameTest_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("xml file|*.xml");
            textPCAModelNameTest.Text = fileName;
        }

        private void buttonHaarModelNameTest_Click(object sender, RoutedEventArgs e)
        {
            string fileName = MainWindow.openFile("xml file|*.xml");
            textHaarModelNameTest.Text = fileName;
        }

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        static public extern IntPtr recognizeEigenface(string image, string pcamodel, string face_cascade_name, out string labelArr, out int size);

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        static public extern void getEigenface(string image, string pcamodel, string face_cascade_name, out IntPtr eigenface, out int size, int nEigen = 0);

        private void buttonTestEigen_Click(object sender, RoutedEventArgs e)
        {
            string label;
            int size;
            int eig = Convert.ToInt32(TextEigens.Text);
            IntPtr ptr = recognizeEigenface(textInputFileNameTest.Text, textPCAModelNameTest.Text, textHaarModelNameTest.Text, out label, out size);
            
            float[] result = new float[size];
            Marshal.Copy(ptr, result, 0, size);
            string[] labels = label.Split(',');
            string output = "";
            for (int i = 0; i < result.Length; i++)
            {
                output += labels[i] + "\t \t \t" + result[i].ToString() + "\n";
            }
            TextOut.Text = output;
            getEigenface(textInputFileNameTest.Text, textPCAModelNameTest.Text, textHaarModelNameTest.Text, out ptr, out size, eig);
            ImageEigenFacesTest.Source = MainWindow.getImageFromIntPtr(ptr, (int)size);
        }
    }
}
