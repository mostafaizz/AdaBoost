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
    /// Interaction logic for AdaBoostControl.xaml
    /// </summary>
    public partial class AdaBoostControl : UserControl
    {
        public AdaBoostControl()
        {
            InitializeComponent();

            TextBox[] texts = new TextBox[] { textBoxT};
            foreach (TextBox obj in texts)
            {
                DataObject.AddPastingHandler(obj, PasteHandler);
            }
        }

        private void PasteHandler(object sender, DataObjectPastingEventArgs e)
        {
            TextBox tb = sender as TextBox;
            bool textOK = false;

            if (e.DataObject.GetDataPresent(typeof(string)))
            {
                // Allow pasting only alphabetic
                string pasteText = e.DataObject.GetData(typeof(string)) as string;
                object[] intTexts = new object[] { textBoxT};
                object[] doubleTexts = new object[] { };
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

        private void TextBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            // Prohibit space
            if (e.Key == Key.Space)
                e.Handled = true;
        }

        // Handling integer inputs
        private void TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            //int[] test = new int[] { 1, 2, 3 };
            List<object> intTexts = new List<object>(new object[] { textBoxT});
            object[] doubleTexts = new object[] { };
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

        private void buttonBrowseTraining_Click(object sender, RoutedEventArgs e)
        {
            string str = MainWindow.openPngFile();
            if (str != null)
            {
                textBoxIn.Text = str;
                imageTrain.Source = MainWindow.getImageFromFile(str);
            }
        }

        private void buttonBrowseTesting_Click(object sender, RoutedEventArgs e)
        {
            string str = MainWindow.openPngFile();
            if (str != null)
            {
                textBoxOut.Text = str;
                imageTest.Source = MainWindow.getImageFromFile(str);
            }
        }

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void testAdaBoostEdgeDetection(int h, int v, int t, string inp, string oup,
            out IntPtr trainImg, out int trainImgSize, out IntPtr testImg, out int testImgSize);

        private void button_Click(object sender, RoutedEventArgs e)
        {
            int h = ((bool)checkBoxHorizontal.IsChecked) ? 1 : 0;
            int v = ((bool)checkBoxVertical.IsChecked) ? 1 : 0;
            int T = int.Parse(textBoxT.Text);
            IntPtr trainImg, testImg;
            int trainImgSize, testImgSize;
            testAdaBoostEdgeDetection(h, v, T, textBoxIn.Text, textBoxOut.Text, out trainImg, out trainImgSize, out testImg, out testImgSize);
            imageTrain.Source = MainWindow.getImageFromIntPtr(trainImg, trainImgSize);
            imageTest.Source = MainWindow.getImageFromIntPtr(testImg, testImgSize);
        }
    }
}
