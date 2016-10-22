using System;
using System.Collections.Generic;
using System.IO;
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
        public static extern IntPtr test(string name, out int size);

        private BitmapImage getImageFromIntPtr(IntPtr ptr,int size)
        {
            byte[] buf = new byte[size];
            Marshal.Copy(ptr, buf, 0, size);
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

                return bitmap;
            }
            catch (Exception ex)
            {
                string message = ex.Message;
            }
            return null;
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
    }
}
