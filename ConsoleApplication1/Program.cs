using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        [DllImport("opencv31_2015.dll")]
        public static extern void test(string name);

        [DllImport("opencv31_2015.dll")]
        public static extern void testAdaBoostEdgeDetection(int horizontal, int vertical, int t, string trainImg, string testImage);


        static void Main(string[] args)
        {
            //test("orig.png");

            testAdaBoostEdgeDetection(1, 1, 1, "orig.png", "test.png");
        }
    }
}
