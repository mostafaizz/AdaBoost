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

namespace Biometrics
{
    /// <summary>
    /// Interaction logic for PointClassificationControl.xaml
    /// </summary>
    public partial class PointClassificationControl : UserControl
    {
        public PointClassificationControl()
        {
            InitializeComponent();
        }

        [DllImport("opencv31_2015.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr test2DPoints(out int retSize, out int numClassifiers, out double accuracy,
            int maximum, double percenatge);
        private void buttonRun_Click(object sender, RoutedEventArgs e)
        {
            int retSize = 0;
            int numClassifiers = 0;
            double accuracy = 0;
            int maxClass;
            double minAccuracy;
            if (int.TryParse(textBoxMaxNum.Text, out maxClass)
                && double.TryParse(textBoxMinAccuracy.Text, out minAccuracy))
            {
                IntPtr res = test2DPoints(out retSize, out numClassifiers, out accuracy, maxClass, minAccuracy);
                image.Source = MainWindow.getImageFromIntPtr(res, retSize);
                textBoxAccuracy.Text = accuracy.ToString();
                textBoxClassifiers.Text = numClassifiers.ToString();
            }

        }

        private void image_MouseDown(object sender, MouseButtonEventArgs e)
        {
            ViolaJonesTester.image_MouseDown(sender, e);

        }
    }
}
