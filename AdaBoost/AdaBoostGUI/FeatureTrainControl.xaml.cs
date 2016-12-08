using System;
using System.Collections.Generic;
using System.ComponentModel;
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
    /// Interaction logic for FeatureTrainControl.xaml
    /// </summary>
    public partial class FeatureTrainControl : UserControl
    {
        public FeatureTrainControl()
        {
            InitializeComponent();
        }

        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            textInputFileName.Text = MainWindow.openFile("CSV file|*.csv");
        }

        private void buttonFeatModelName_Click(object sender, RoutedEventArgs e)
        {
            textFeatModelName.Text = MainWindow.saveFile("xml file|*.xml");
        }

        private void buttonHaarModelName_Click(object sender, RoutedEventArgs e)
        {
            textHaarModelName.Text = MainWindow.openFile("xml file|*.xml");
        }

        private void buttonRegModelName_Click(object sender, RoutedEventArgs e)
        {
            textRegModelName.Text = MainWindow.openFile("txt file|*.txt");
        }

        [DllImport("facial_points.dll", CallingConvention = CallingConvention.Cdecl)]
        static public extern void learnFeat(string inputfilename, string featmodel, string face_cascade_name, string reg_name);

        private void buttonTrain_Click(object sender, RoutedEventArgs e)
        {
            BackgroundWorker bkgWorker = new BackgroundWorker();
            bkgWorker.WorkerSupportsCancellation = true;
            bkgWorker.DoWork += BkgWorker_DoWork; ;
            bkgWorker.ProgressChanged += BkgWorker_ProgressChanged; ;
            bkgWorker.RunWorkerCompleted += BkgWorker_RunWorkerCompleted; ;
            bkgWorker.WorkerReportsProgress = true;
            progressBarTrain.Maximum = 10;
            labelTraining.Content = "Training";
            bkgWorker.RunWorkerAsync();
        }

        private void BkgWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            progressBarTrain.Value = progressBarTrain.Maximum;
            labelTraining.Content = "Done";
        }

        private void BkgWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBarTrain.Value++;
            if(progressBarTrain.Value == progressBarTrain.Maximum)
            {
                progressBarTrain.Value = 1;
            }
        }

        private void BkgWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            learnFeat(textInputFileName.Text, textFeatModelName.Text, textHaarModelName.Text, textRegModelName.Text);
        }
    }
}
