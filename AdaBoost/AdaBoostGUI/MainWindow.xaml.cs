using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using System.Windows.Threading;

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


        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr trainCascadeClassifier(string posFile, string negFile, int numPositive, int numNegative,
                                                            int patchWidth, int patchHeight,
                                                            [MarshalAs(UnmanagedType.LPArray)] double[] sizeFactors, int sizeFactorsLength,
                                                            [MarshalAs(UnmanagedType.LPArray)] int[] cascadeSize, int cascadeSizesLength,
                                                            int step);
        

        [DllImport("opencv31_2015.dll")]
        public static extern IntPtr testCascadeClassifier(IntPtr classifier, string imageName, ref int retSize);

        [DllImport("opencv31_2015.dll")]
        public static extern void deleteCascadeClassifier(IntPtr classifier);

        [DllImport("Kernel32.dll", SetLastError = true)]
        public static extern int SetStdHandle(int device, IntPtr handle);

        

       
        public static BitmapImage getImageFromBytes(byte[] buf)
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

        public static BitmapImage getImageFromIntPtr(IntPtr ptr,int size)
        {
            byte[] buf = new byte[size];
            Marshal.Copy(ptr, buf, 0, size);
            return getImageFromBytes(buf);
        }

        public static BitmapImage getImageFromFile(string fileName)
        {
            byte [] buf = File.ReadAllBytes(fileName);
            return getImageFromBytes(buf);
        }

        
        
        public static string openPngFile()
        {
            return openFile("PNG Files (*.png)|*.png");
        }
        public static string openFile(string filters)
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
        public static string saveFile(string filters)
        {
            SaveFileDialog sf = new SaveFileDialog();
            sf.Filter = filters;
            sf.InitialDirectory = Directory.GetCurrentDirectory();
            if (sf.ShowDialog() == true)
            {
                return sf.FileName;
            }
            return null;
        }

        IntPtr cascadeClassifierPtr = IntPtr.Zero;

        private void buttonTrainMyCascade_Click(object sender, RoutedEventArgs e)
        {
            List<double> sizesFactors = new List<double>(new double[]{ 1, 0.6, 0.4 });
            List<int> cascadeSizes = new List<int>(new int[] { 1, 2, 4, 8 });

            cascadeClassifierPtr = trainCascadeClassifier("faces1/ann.txt",
            "bg.txt",
            20, 100, 24, 24,
            sizesFactors.ToArray(), sizesFactors.Count(),
            cascadeSizes.ToArray(), cascadeSizes.Count(),
            3);
        }

        private void buttonTestMyCascade_Click(object sender, RoutedEventArgs e)
        {
            int size = 0;
            IntPtr iamge = testCascadeClassifier(cascadeClassifierPtr, "faces1/9338519.16.jpg", ref size);
            imageCascadeClassifier.Source = getImageFromIntPtr(iamge, size);
        }

    }
}
