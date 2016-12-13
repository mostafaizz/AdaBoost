using System;
using System.Collections.Generic;
using System.Data;
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
    /// Interaction logic for FeatureTestControl.xaml
    /// </summary>
    public partial class FeatureTestControl : UserControl
    {
        public FeatureTestControl()
        {
            InitializeComponent();
        }

        private void buttonInputFileName_Click(object sender, RoutedEventArgs e)
        {
            textInputFileName.Text = MainWindow.openFile("Image files (*.jpg, *.jpeg, *.jpe, *.jfif, *.png) | *.jpg; *.jpeg; *.jpe; *.jfif; *.png");
            if (File.Exists(textInputFileName.Text))
            {
                imageTest.Source = MainWindow.getImageFromFile(textInputFileName.Text);
            }
        }

        private void buttonFeatModelName_Click(object sender, RoutedEventArgs e)
        {
            textFeatModelName.Text = MainWindow.openFile("xml file|*.xml");
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
        static public extern IntPtr recognizeFeat(string image, string featmodel, string face_cascade_name, string reg_name, out string labelArr, out int size);

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            string label;
            int size;
            IntPtr ptr = recognizeFeat(textInputFileName.Text, textFeatModelName.Text, 
                textHaarModelName.Text, textRegModelName.Text, out label, out size);
            if (File.Exists(textInputFileName.Text))
            {
                imageTest.Source = MainWindow.getImageFromFile(textInputFileName.Text);
            }
            float[] result = new float[size];
            Marshal.Copy(ptr, result, 0, size);
            string[] labels = label.Split(',');
            Dictionary<string, float> res = new Dictionary<string, float>();

            for (int i = 0; i < result.Length; i++)
            {
               
                if (res.ContainsKey(labels[i]))
                {
                    if (res[labels[i]] > result[i])
                    {
                        res[labels[i]] = result[i];
                    }
                }
                else { res.Add(labels[i], result[i]); }
            }
            //labelTesting.Content = output;
            labelTesting.ItemsSource = null;
            labelTesting.ItemsSource = res;
            labelTesting.Columns[0].Header = "Name";
            labelTesting.Columns[1].Header = "Distance";
            
        }

        private void imageTest_MouseDown(object sender, MouseButtonEventArgs e)
        {
            ViolaJonesTester.image_MouseDown(sender, e);

        }
    }
}
