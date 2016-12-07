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
            TextBox[] texts = new TextBox[] { textBoxT, textBoxPatchWidth, textBoxPatchHeight, textBoxNumStages, textBoxNumPos, textBoxNumNeg, textBoxMinRate, textBoxMaxFalse };
            foreach (TextBox obj in texts)
            {
                DataObject.AddPastingHandler(obj, PasteHandler);
            }
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

        IntPtr cascadeClassifierPtr = IntPtr.Zero;
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

        // Handling integer inputs
        private void TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            //int[] test = new int[] { 1, 2, 3 };
            List<object> intTexts =  new List<object>(new object[]{textBoxT, textBoxPatchWidth, textBoxPatchHeight, textBoxNumStages, textBoxNumPos, textBoxNumNeg });
            object[] doubleTexts = new object[] { textBoxMinRate, textBoxMaxFalse };
            if (intTexts.Contains(sender))
            {
                // Prohibit non-int
                int i;
                if (!int.TryParse(e.Text, out i))
                    e.Handled = true;
            }
            else
            {
                // Prohibit non-double
                double i;
                if (!double.TryParse(((TextBox)sender).Text, out i))
                    e.Handled = true;
            }
        }

        private void TextBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            // Prohibit space
            if (e.Key == Key.Space)
                e.Handled = true;
        }

        private void PasteHandler(object sender, DataObjectPastingEventArgs e)
        {
            TextBox tb = sender as TextBox;
            bool textOK = false;

            if (e.DataObject.GetDataPresent(typeof(string)))
            {
                // Allow pasting only alphabetic
                string pasteText = e.DataObject.GetData(typeof(string)) as string;
                object[] intTexts = new object[] { textBoxT, textBoxPatchWidth, textBoxPatchHeight, textBoxNumStages, textBoxNumPos, textBoxNumNeg };
                object[] doubleTexts = new object[] { textBoxMinRate, textBoxMaxFalse };
                if (Array.Exists<object>(intTexts, element => element == sender))
                {
                    int i;
                    if (int.TryParse(pasteText, out i))
                        textOK = true;
                }
                else
                {
                    double i;
                    if (double.TryParse(pasteText, out i))
                        textOK = true;
                }
            }

            if (!textOK)
                e.CancelCommand();
        }

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
            IntPtr trainImg, testImg;
            int trainImgSize, testImgSize;
            testAdaBoostEdgeDetection(h, v, T, textBoxIn.Text, textBoxOut.Text, out trainImg, out trainImgSize, out testImg, out testImgSize);
            imageTrain.Source = getImageFromIntPtr(trainImg, trainImgSize);
            imageTest.Source = getImageFromIntPtr(testImg, testImgSize);
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
            string str = openPngFile();
            if (str != null)
            {
                textBoxIn.Text = str;
                imageTrain.Source = getImageFromFile(str);
            }
        }

        private void buttonBrowseTesting_Click(object sender, RoutedEventArgs e)
        {
            string str = openPngFile();
            if (str != null)
            {
                textBoxOut.Text = str;
                imageTest.Source = getImageFromFile(str);
            }
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
            if (imageFileName != null)
            {
                imageCamera.Source = getImageFromFile(imageFileName);
                labelFaceImage.Content = imageFileName;
                labelFaceImage.ToolTip = imageFileName;
            }
        }

        private void buttonSelectModel_Click(object sender, RoutedEventArgs e)
        {
            string modelFile = openFile("XML Files (*.xml)|*.xml");
            labelModel.Content = modelFile;
            labelModel.ToolTip = modelFile;
        }

        BitmapImage detectedFaces = null;

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //captureImage();
            int size;
            IntPtr faceImage = faceDetect(labelFaceImage.Content.ToString(), labelModel.Content.ToString(), out size);
            detectedFaces = getImageFromIntPtr(faceImage, size);
            imageCamera.Source = detectedFaces;
        }

        private void buttonPos_Click(object sender, RoutedEventArgs e)
        {
            string pos = openFile("VEC Files (*.vec)|*.vec");
            labelPos.Text = pos;
            labelPos.ToolTip = pos;
        }

        private void buttonNeg_Click(object sender, RoutedEventArgs e)
        {
            string neg = openFile("TEXT Files (*.txt)|*.txt");
            labelNeg.Text = neg;
            labelNeg.ToolTip = neg;
        }

        int stage = 0;
        BackgroundWorker bkgWorker = null;
        private void buttonTrain_Click(object sender, RoutedEventArgs e)
        {
            if (buttonTrain.Content.ToString() == "Start Training")
            {
                bkgWorker = new BackgroundWorker();
                buttonTrain.Content = "Cancel Training";
                textBlock.Text = "";
                labelTrainingStage.Content = "Training Started";
                pBar.Value = 0;
                stage = 0;
            }
            else
            {
                MessageBoxResult result = MessageBox.Show("Do you want to cancel this training?", "Confirmation", MessageBoxButton.YesNo, MessageBoxImage.Question);
                if (result == MessageBoxResult.Yes)
                {
                    //bkgWorker.CancelAsync();
                    exeProcess.CloseMainWindow();
                    exeProcess.Kill();
                    exeProcess.WaitForExit();
                    bkgWorker.CancelAsync();
                    //
                    buttonTrain.Content = "Start Training";
                    textBlock.Text = "";
                    labelTrainingStage.Content = "";
                    pBar.Value = 0;
                }
                return;
            }

            
            bkgWorker.WorkerSupportsCancellation = true;
            bkgWorker.DoWork += Worker_DoWork;
            bkgWorker.ProgressChanged += Worker_ProgressChanged;
            bkgWorker.RunWorkerCompleted += BkgWorker_RunWorkerCompleted;
            bkgWorker.WorkerReportsProgress = true;
            string args = "";
            args += string.Format("-data {0} -vec {1} -bg {2} ", labelOutput.Text, labelPos.Text, labelNeg.Text);
            args += string.Format("-precalcValBufSize {0} -precalcIdxBufSize {1}  ", 2048, 2048);
            args += string.Format("-numPos {0} -numNeg {1} -numStages {2} ", textBoxNumPos.Text, textBoxNumNeg.Text, textBoxNumStages.Text);
            args += string.Format("-minhitrate {0} -maxfalsealarm {1} ", textBoxMinRate.Text, textBoxMaxFalse.Text);
            args += string.Format("-w {0} -h {1} -nonsym -baseFormatSave ", textBoxPatchWidth.Text, textBoxPatchHeight.Text);

            bkgWorker.RunWorkerAsync(args);
        }

        private void BkgWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            pBar.Value = pBar.Maximum;
            buttonTrain.Content = "Start Training";
            labelTrainingStage.Content = "Training Finished";
            textBlock.ScrollToEnd();
        }

        private void Worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            string txt = (e.UserState as string);
            if (txt.Trim().Length == 0)
            {
                return;
            }
            else
            {
                if (txt.Contains(stage.ToString() + "-stage"))
                {
                    int total = int.Parse(textBoxNumStages.Text);
                    labelTrainingStage.Content = txt;
                    pBar.Value = ((stage++) * 1.0 / total) * pBar.Maximum;

                    //textBlock.Focus();
                    //textBlock.CaretIndex = textBlock.Text.Length;

                }
                textBlock.Text += txt + "\r\n";
                if (e.ProgressPercentage % 20 == 0)
                {
                    textBlock.ScrollToEnd();
                }
            }
        }
        Process exeProcess = null;
        private void Worker_DoWork(object sender, DoWorkEventArgs e)
        {

            // opencv_traincascade -data data -vec faces\pos-samples.vec 
            // -bg no-face\bg.txt -precalcValBufSize 2048 -precalcIdxBufSize 2048 
            // -numPos 20 -numNeg 20 -nstages 5 -minhitrate 0.999 
            // -maxfalsealarm 0.5 -w 24 -h 24 -nonsym -baseFormatSave
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = false;
            startInfo.FileName = "opencv_traincascade.exe";
            //startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            startInfo.RedirectStandardOutput = true;
            string args = e.Argument as string;
            //MessageBox.Show(args);
            startInfo.Arguments = args;
            try
            {
                int i = 0;
                // Start the process with the info we specified.
                // Call WaitForExit and then the using statement will close.
                using (exeProcess = Process.Start(startInfo))
                {
                    exeProcess.OutputDataReceived += ExeProcess_OutputDataReceived;
                    while (!exeProcess.StandardOutput.EndOfStream)
                    {

                        string line = exeProcess.StandardOutput.ReadLine();
                        // do something with line
                        (sender as BackgroundWorker).ReportProgress(i++, line);
                    }
                    exeProcess.WaitForExit();
                    (sender as BackgroundWorker).ReportProgress(0, "MY_END");
                }
            }
            catch(Exception exc)
            {
                // Log error.
                MessageBox.Show(exc.Message);
            }
        }

        private void ExeProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!string.IsNullOrEmpty(e.Data))
            {
                textBlock.Text += e.Data + "\n";
            }
        }

        private void buttonOutput_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.SelectedPath = Directory.GetCurrentDirectory();
            dialog.ShowDialog();
            labelOutput.Text = dialog.SelectedPath;
        }

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

        private void OpenImage_Click(object sender, RoutedEventArgs e)
        {
            string imageFileName = openFile("PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg");
            if (imageFileName != null)
            {
                origIrisImage.Source = getImageFromFile(imageFileName);
                origIrisImage.ToolTip = imageFileName;
                origIrisImageIrisExtracted.Source = null;
                IrisNormalized.Source = null;
                IrisCode.Source = null;
            }
        }

        private void FindIris_Click(object sender, RoutedEventArgs e)
        {
            if(iRec == IntPtr.Zero)
            {
                iRec = createIrisRecognizer(512, 64);
            }
            IntPtr stripData = IntPtr.Zero, origData  = IntPtr.Zero;
            int stripSize = 0, dataSize = 0;
            extractIris((string)(origIrisImage.ToolTip), iRec, out stripData, out stripSize, out origData, out dataSize);
            origIrisImageIrisExtracted.Source = getImageFromIntPtr(origData, dataSize);
            IrisNormalized.Source = getImageFromIntPtr(stripData, stripSize);
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
            IrisCode.Source = getImageFromIntPtr(codeData, codeSize);
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
