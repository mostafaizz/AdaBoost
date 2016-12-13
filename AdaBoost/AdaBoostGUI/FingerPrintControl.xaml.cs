using System;
using System.Collections.Generic;
using System.ComponentModel;
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

namespace Biometrics
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

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr createFingerPrintMatcher();

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void deleteFingerPrintMatcher(IntPtr ptr);

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void trainFingerPrintOneImage(IntPtr ptr, string name, string fileName);

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr computeFingerFeatures(IntPtr ptr, string fileName, out int retSize);

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double testFingerPrintOneImage(IntPtr ptr, string name);

        string[] files = null;
        private void buttonBrowseFolder_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.SelectedPath = Directory.GetCurrentDirectory();
            System.Windows.Forms.DialogResult result = dialog.ShowDialog();
            if(result == System.Windows.Forms.DialogResult.OK)
            {
                trainingFolderTextBox.Text = dialog.SelectedPath;
                files = Directory.GetFiles(trainingFolderTextBox.Text, "*.bmp");
            }
        }

        IntPtr fingerPrintPtr = IntPtr.Zero;

        private void buttonTrain_Click(object sender, RoutedEventArgs e)
        {
            if(fingerPrintPtr == IntPtr.Zero)
            {
                fingerPrintPtr = createFingerPrintMatcher();
            }

            BackgroundWorker bkgr = new BackgroundWorker();
            bkgr.DoWork += Bkgr_DoWork;
            bkgr.WorkerReportsProgress = true;
            bkgr.ProgressChanged += Bkgr_ProgressChanged;
            bkgr.RunWorkerCompleted += Bkgr_RunWorkerCompleted;
            progressBarTrain.Maximum = files.Length;

            bkgr.RunWorkerAsync();
        }

        private void Bkgr_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            progressBarTrain.Value = progressBarTrain.Maximum;
            label_TrainingProgress.Content = "Training Finished";
        }

        private void Bkgr_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBarTrain.Value = e.ProgressPercentage;
            label_TrainingProgress.Content = string.Format("{0:00.00} %", e.ProgressPercentage * 100.0 / progressBarTrain.Maximum);
        }

        private void Bkgr_DoWork(object sender, DoWorkEventArgs e)
        {
            if (files != null)
            {
                for (int i = 0; i < files.Length; i++)
                {
                    (sender as BackgroundWorker).ReportProgress(i);
                    FileInfo finf = new FileInfo(files[i]);
                    trainFingerPrintOneImage(fingerPrintPtr, finf.Name, finf.FullName);
                }
            }

        }

        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            testingImageTextBox.Text = MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png, *.bmp) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png; *.bmp");
            if (File.Exists(testingImageTextBox.Text))
            {
                testingImage.Source = MainWindow.getImageFromFile(testingImageTextBox.Text);
            }
        }

        private void testingImage_MouseDown(object sender, MouseButtonEventArgs e)
        {
            ViolaJonesTester.image_MouseDown(sender, e);
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            BackgroundWorker bkgr = new BackgroundWorker();
            bkgr.DoWork += Bkgr_DoWork1;
            bkgr.WorkerReportsProgress = true;
            bkgr.ProgressChanged += Bkgr_ProgressChanged1;
            bkgr.RunWorkerCompleted += Bkgr_RunWorkerCompleted1;

            bkgr.RunWorkerAsync(testingImageTextBox.Text);
        }

        private void Bkgr_RunWorkerCompleted1(object sender, RunWorkerCompletedEventArgs e)
        {
            dataGrid.ItemsSource = null;
            dataGrid.ItemsSource = dic;
            dataGrid.Columns[0].Header = "Name";
            dataGrid.Columns[1].Header = "Distance";
        }

        private void Bkgr_ProgressChanged1(object sender, ProgressChangedEventArgs e)
        {
            if (e.ProgressPercentage == 0)
            {
                featuresImage.Source = e.UserState as BitmapImage;
            }
        }

        Dictionary<string, double> dic = null;
        private void Bkgr_DoWork1(object sender, DoWorkEventArgs e)
        {
            int retSize = 0;
            IntPtr oup = computeFingerFeatures(fingerPrintPtr, e.Argument as string, out retSize);
            
            (sender as BackgroundWorker).ReportProgress(0, MainWindow.getImageFromIntPtr(oup, retSize));

            dic = new Dictionary<string, double>();
            for (int i = 0; i < files.Length; i++)
            {
                FileInfo finf = new FileInfo(files[i]);
                double res = testFingerPrintOneImage(fingerPrintPtr, finf.Name);
                dic.Add(finf.Name, res);
                //(sender as BackgroundWorker).ReportProgress(1, dic);
            }
            
        }
    }
}
