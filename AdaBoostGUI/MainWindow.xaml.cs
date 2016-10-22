using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
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
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void testAdaBoostEdgeDetection(int h,int v,int t,string inp, string oup, 
            out IntPtr trainImg, out int trainImgSize, out IntPtr testImg, out int testImgSize);

        [DllImport("opencv31_2015.dll")]
        public static extern void test(ref int size);

        [DllImport("opencv31_2015.dll")]
        public static extern void captureCamera(out IntPtr Img, out int size, int id);

        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr faceDetect(string fileName, string modelName, out int imgSize);

        private BitmapImage getImageFromBytes(byte[] buf)
        {
            try
            {
                MemoryStream memoryStream = new MemoryStream(buf);

                BitmapImage bitmap = new BitmapImage();
                bitmap.BeginInit();
                //bitmap.DecodePixelWidth = 400;
                //bitmap.DecodePixelHeight = 400;
                bitmap.StreamSource = memoryStream;
                bitmap.EndInit();
                bitmap.Freeze();

                return bitmap;
            }
            catch (Exception ex)
            {
                string message = ex.Message;
            }
            return null;
        }

        private BitmapImage getImageFromIntPtr(IntPtr ptr,int size)
        {
            byte[] buf = new byte[size];
            Marshal.Copy(ptr, buf, 0, size);
            return getImageFromBytes(buf);
        }

        private BitmapImage getImageFromFile(string fileName)
        {
            byte [] buf = File.ReadAllBytes(fileName);
            return getImageFromBytes(buf);
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            int h = ((bool)checkBoxHorizontal.IsChecked) ? 1 : 0;
            int v = ((bool)checkBoxVertical.IsChecked) ? 1 : 0;
            int T = int.Parse(textBoxT.Text);

            /*int r = 0;
            IntPtr ptr = test("orig.png", out r);
            byte []buf = new byte[r];
            Marshal.Copy(ptr, buf,0 , r);
            try
            {
                //byte [] buf1 = File.ReadAllBytes("orig.png");
                MemoryStream memoryStream = new MemoryStream(buf);

                BitmapImage bitmap = new BitmapImage();
                bitmap.BeginInit();
                bitmap.DecodePixelWidth = 400;
                bitmap.DecodePixelHeight = 400;
                bitmap.StreamSource = memoryStream;
                bitmap.EndInit();
                bitmap.Freeze();

                imageTrain.Source = bitmap;
            }
            catch (Exception ex)
            {
                string message = ex.Message;
            }
            */
            //var img = Image.(stream);
            //BitmapImage img = new BitmapImage(new Uri("test.png"));
            //image.Source = img;
            IntPtr trainImg, testImg;
            int trainImgSize, testImgSize;
            testAdaBoostEdgeDetection(h, v, T, textBoxIn.Text, textBoxOut.Text, out trainImg, out trainImgSize, out testImg, out testImgSize);
            imageTrain.Source = getImageFromIntPtr(trainImg, trainImgSize);
            imageTest.Source = getImageFromIntPtr(testImg, testImgSize);
        }

        private void checkBox_Checked(object sender, RoutedEventArgs e)
        {

        }

        private string openPngFile()
        {
            return openFile("PNG Files (*.png)|*.png");
        }
        private string openFile(string filters)
        {
            OpenFileDialog opf = new OpenFileDialog();
            opf.Filter = filters;
            opf.Multiselect = false;
            opf.InitialDirectory = Directory.GetCurrentDirectory();
            if (opf.ShowDialog() == true)
            {
                return opf.FileName;
            }
            return null;
        }
        private void buttonBrowseTraining_Click(object sender, RoutedEventArgs e)
        {
            textBoxIn.Text = openPngFile();
            imageTrain.Source = getImageFromFile(textBoxIn.Text);
        }

        private void buttonBrowseTesting_Click(object sender, RoutedEventArgs e)
        {
            textBoxOut.Text = openPngFile();
            imageTest.Source = getImageFromFile(textBoxIn.Text);
        }

        private void captureImage()
        {
            IntPtr img;
            int size = 0;
            int id = 0;
            captureCamera(out img, out size, id);
            imageCamera.Source = getImageFromIntPtr(img, size);
        }



        

        private void buttonSelectImage_Click(object sender, RoutedEventArgs e)
        {
            //captureImage();
            string imageFileName = openFile("PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg");
            imageCamera.Source = getImageFromFile(imageFileName);
            labelFaceImage.Content = imageFileName;
            labelFaceImage.ToolTip = imageFileName;
        }

        private void buttonSelectModel_Click(object sender, RoutedEventArgs e)
        {
            string modelFile = openFile("XML Files (*.xml)|*.xml");
            labelModel.Content = modelFile;
            labelModel.ToolTip = modelFile;
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //captureImage();
            int size;
            IntPtr faceImage = faceDetect(labelFaceImage.Content.ToString(), labelModel.Content.ToString(), out size);
            imageCamera.Source = getImageFromIntPtr(faceImage, size);
        }

        private void buttonPos_Click(object sender, RoutedEventArgs e)
        {
            string pos = openFile("VEC Files (*.vec)|*.vec");
            labelPos.Content = pos;
            labelPos.ToolTip = pos;
        }

        private void buttonNeg_Click(object sender, RoutedEventArgs e)
        {
            string neg = openFile("TEXT Files (*.txt)|*.txt");
            labelNeg.Content = neg;
            labelNeg.ToolTip = neg;
        }

        private void buttonTrain_Click(object sender, RoutedEventArgs e)
        {
            // opencv_traincascade -data data -vec faces\pos-samples.vec 
            // -bg no-face\bg.txt -precalcValBufSize 2048 -precalcIdxBufSize 2048 
            // -numPos 20 -numNeg 20 -nstages 5 -minhitrate 0.999 
            // -maxfalsealarm 0.5 -w 24 -h 24 -nonsym -baseFormatSave
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = "opencv_traincascade.exe";
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            startInfo.Arguments = "";
            startInfo.Arguments += string.Format("-data {0} -vec {1} -bg {2}", labelOutput.Content, labelPos.Content, labelNeg.Content);

            try
            {
                // Start the process with the info we specified.
                // Call WaitForExit and then the using statement will close.
                using (Process exeProcess = Process.Start(startInfo))
                {
                    exeProcess.WaitForExit();
                }
            }
            catch
            {
                // Log error.
            }

        }
    }
}
