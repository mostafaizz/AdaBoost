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
    /// Interaction logic for ViolaJonesTester.xaml
    /// </summary>
    public partial class ViolaJonesTester : UserControl
    {
        public ViolaJonesTester()
        {
            InitializeComponent();
        }

        private void buttonSelectImage_Click(object sender, RoutedEventArgs e)
        {
            //captureImage();
            string imageFileName =MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png");
            if (imageFileName != null)
            {
                imageCamera.Source = MainWindow.getImageFromFile(imageFileName);
                labelFaceImage.Content = imageFileName;
                labelFaceImage.ToolTip = imageFileName;
            }
        }

        private void buttonSelectModel_Click(object sender, RoutedEventArgs e)
        {
            string modelFile = MainWindow.openFile("XML Files (*.xml)|*.xml");
            labelModel.Content = modelFile;
            labelModel.ToolTip = modelFile;
        }

        BitmapImage detectedFaces = null;

        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr faceDetect(string fileName, string modelName, out int imgSize);

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //captureImage();
            int size;
            IntPtr faceImage = faceDetect(labelFaceImage.Content.ToString(), labelModel.Content.ToString(), out size);
            detectedFaces = MainWindow.getImageFromIntPtr(faceImage, size);
            imageCamera.Source = detectedFaces;
        }

        private void buttonSaveFaces_Click(object sender, RoutedEventArgs e)
        {
            if (detectedFaces != null)
            {
                //detectedFaces
                ImageWindow iw = new ImageWindow();
                iw.image.Source = detectedFaces;
                iw.WindowState = WindowState.Maximized;
                iw.Show();
            }
        }
    }
}
