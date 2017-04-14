using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
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
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using Microsoft.Research.DynamicDataDisplay.PointMarkers;
using System.Reflection;
using Microsoft.Research.DynamicDataDisplay.ViewportRestrictions;
using Microsoft.Research.DynamicDataDisplay.Common;
using DynamicDataDisplay.Markers.DataSources;
using System.Threading;
using System.Windows.Threading;

namespace BmpShow
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region Поля

        private const int SIZE = 512;
        private WriteableBitmap ImageField;
        private Bitmap.Bitmap MainBmp;
        private Bitmap.Bitmap ChangedBmp;
        private string FileName;
        private List<double> matrixSide = new List<double>();
        private List<double> timeSelection = new List<double>();
        private List<double> timeHistogram = new List<double>();
        private List<double> timeHistogramOptimized = new List<double>();
        private bool IsRunning = false;
        private int IterNum;
        Thread thread;
        System.Windows.Threading.DispatcherTimer dispatcherTimer;

        #endregion

        public MainWindow()
        {
            InitializeComponent();
            ImageField = BitmapFactory.New(SIZE, SIZE);
            field.Source = ImageField;
            FileName = "D:/Study/Image Processing/images4dz/Gray_Win.bmp";
            LoadImage(null, null);
        }

        /// <summary>
        /// загрузить картинку из файла
        /// </summary>
        void LoadImage(object sender, EventArgs e)
        {
            if (dispatcherTimer != null) dispatcherTimer.Stop();
            try
            {
                MainBmp = new Bitmap.Bitmap(FileName);
                ChangedBmp = null;
                matrixSide.Clear();
                timeSelection.Clear();
                timeHistogram.Clear();
                timeHistogramOptimized.Clear();

                ShowImages();
                ShowPlot();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                MainBmp = ChangedBmp = null;
                matrixSide.Clear();
                ShowImages();
                ShowPlot();
            }
        }

        /// <summary>
        /// отобразить изображения
        /// </summary>
        void ShowImages()
        {
            if (MainBmp != null)
            {
                //перегоняем
                var newWb = ToWritebleBmp(MainBmp);

                //добавляем в окно
                Rect rec;
                if ((int)newWb.Width == SIZE / 2)
                    rec = new Rect(0, SIZE / 2 - (int)newWb.Height / 2, (int)newWb.Width, (int)newWb.Height);
                else
                    rec = new Rect(SIZE / 4 - (int)newWb.Width / 2, 0, (int)newWb.Width, (int)newWb.Height);

                using (newWb.GetBitmapContext())
                {
                    using (ImageField.GetBitmapContext())
                    {
                        ImageField.Blit(rec, newWb, new Rect(0, 0, (int)newWb.Width, (int)newWb.Height), WriteableBitmapExtensions.BlendMode.None);
                    }
                }

                if (ChangedBmp != null)
                {
                    //перегоняем
                    newWb = ToWritebleBmp(ChangedBmp);

                    if ((int)newWb.Width == SIZE / 2)
                        rec = new Rect(SIZE / 2, SIZE / 2 - (int)newWb.Height / 2, (int)newWb.Width, (int)newWb.Height);
                    else
                        rec = new Rect(SIZE * 3 / 4 - (int)newWb.Width / 2, 0, (int)newWb.Width, (int)newWb.Height);

                    using (newWb.GetBitmapContext())
                    {
                        using (ImageField.GetBitmapContext())
                        {
                            ImageField.Blit(rec, newWb, new Rect(0, 0, (int)newWb.Width, (int)newWb.Height), WriteableBitmapExtensions.BlendMode.None);
                        }
                    }
                }

                //отделяем
                ImageField.FillRectangle(SIZE / 2 - 1, SIZE / 2 - (int)newWb.Height / 2, SIZE / 2 + 1, SIZE / 2 + (int)newWb.Height / 2, Colors.Black);
            }
        }

        /// <summary>
        /// отобразить график
        /// </summary>
        void ShowPlot()
        {
            GraphField.RemoveUserElements();
            if (matrixSide.Count() > 0)
            {

                GraphField.AddLineGraph(matrixSide.ToList().AsXDataSource().Join(timeSelection.ToList().AsYDataSource()), Colors.Red, 1, "Selection");
                GraphField.AddLineGraph(matrixSide.ToList().AsXDataSource().Join(timeHistogram.ToList().AsYDataSource()), Colors.Blue, 1, "Histogram");
                GraphField.AddLineGraph(matrixSide.ToList().AsXDataSource().Join(timeHistogramOptimized.ToList().AsYDataSource()), Colors.Green, 1, "HistogramOptimized");

                ViewportAxesRangeRestriction restr = new ViewportAxesRangeRestriction();
                restr.YRange = new DisplayRange(0, Math.Max(timeSelection.Max(), timeHistogram.Max()) * 1.3);
                restr.XRange = new DisplayRange(1, Math.Max(matrixSide.Max() + 1, 6));
                GraphField.Viewport.Restrictions.Clear();
                GraphField.Viewport.Restrictions.Add(restr);
            }
            else
            {
                ViewportAxesRangeRestriction restr = new ViewportAxesRangeRestriction();
                restr.YRange = new DisplayRange(0, 1);
                restr.XRange = new DisplayRange(1, 6);
                GraphField.Viewport.Restrictions.Clear();
                GraphField.Viewport.Restrictions.Add(restr);
            }
        }

        /// <summary>
        /// перегон нашего Bitmap во внешний
        /// </summary>
        static WriteableBitmap ToWritebleBmp(Bitmap.Bitmap b)
        {
            //перегон
            var newWb = BitmapFactory.New(b.Width, b.Height);
            for (int row = 0; row < b.Height; row++)
                for (int col = 0; col < b.Width; col++)
                {
                    Bitmap.RGBColor pixel = b.GetRGBPixel(row, col);
                    newWb.SetPixel(col, row, Color.FromRgb(pixel.r, pixel.g, pixel.b));
                }

            //ресайз под окно
            newWb = newWb.Resize(SIZE / 2, b.Height * (SIZE / 2) / b.Width, WriteableBitmapExtensions.Interpolation.NearestNeighbor);

            return newWb;
        }

        /// <summary>
        /// Итерация подсчета
        /// </summary>
        private void DoIteration()
        {
            try
            {
                //генерим ядро
                byte[] kernel = new byte[IterNum * IterNum];
                for (int i = 0; i < kernel.Length; i++)
                {
                    kernel[i] = 1;
                }
                double time;

                //вызываем все виды
                ChangedBmp = Bitmap.Bitmap.MedianFilter(MainBmp, kernel, IterNum, IterNum, out time,
                    Bitmap.MedianFilterType.HistogramOptimized);
                timeHistogramOptimized.Add(time);
                matrixSide.Add(IterNum);

                Bitmap.Bitmap.MedianFilter(MainBmp, kernel, IterNum, IterNum, out time,
                    Bitmap.MedianFilterType.Histogram);
                timeHistogram.Add(time);

                Bitmap.Bitmap.MedianFilter(MainBmp, kernel, IterNum, IterNum, out time,
                    Bitmap.MedianFilterType.Selection);
                timeSelection.Add(time);

                IterNum += 2;

                // Получить диспетчер от текущего окна и использовать его для вызова кода обновления
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate()
                    {
                        //обновить все и запустить следующую
                        if (IsRunning)
                        {
                            ShowImages();
                            ShowPlot();

                            thread.Abort();
                            thread = new Thread(DoIteration);
                            thread.Start();
                        }
                    }
                );
            }
            catch (System.Threading.ThreadAbortException)
            {
                //поток остановлен
            }
            catch (Exception ex)
            {
                //настоящая ошибка
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
               (ThreadStart)delegate()
               {
                   MessageBox.Show(ex.Message);
                   matrixSide.Clear();
                   ShowImages();
                   ShowPlot();
                   StartBtn.IsEnabled = true;
                   StopBtn.IsEnabled = false;
                   ChooseFile.IsEnabled = true;
               });
            }
        }

        #region События

        private void Start_Click(object sender, RoutedEventArgs e)
        {
            //unfocus button
            var scope = FocusManager.GetFocusScope((DependencyObject)sender);
            FocusManager.SetFocusedElement(scope, null);
            Keyboard.ClearFocus();

            thread = new Thread(DoIteration);
            IterNum = 1;
            IsRunning = true;
            StartBtn.IsEnabled = false;
            StopBtn.IsEnabled = true;
            ChooseFile.IsEnabled = false;
            matrixSide.Clear();
            timeSelection.Clear();
            timeHistogram.Clear();
            timeHistogramOptimized.Clear();
            thread.Start();
        }
        private void ChooseFile_Click(object sender, RoutedEventArgs e)
        {
            //unfocus button
            var scope = FocusManager.GetFocusScope((DependencyObject)sender);
            FocusManager.SetFocusedElement(scope, null);
            Keyboard.ClearFocus();

            //выбор файла
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Bitmap|*.bmp;";
            if (openFileDialog.ShowDialog() == true)
            {
                ImageField.FillRectangle(0, 0, (int)ImageField.Width, (int)ImageField.Height, Colors.White);
                FileName = openFileDialog.FileName;

                dispatcherTimer = new System.Windows.Threading.DispatcherTimer();
                dispatcherTimer.Tick += new EventHandler(LoadImage);
                dispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, 20);
                dispatcherTimer.Start();
            }
        }
        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            //unfocus button
            var scope = FocusManager.GetFocusScope((DependencyObject)sender);
            FocusManager.SetFocusedElement(scope, null);
            Keyboard.ClearFocus();

            thread.Abort();
            IsRunning = false;
            StartBtn.IsEnabled = true;
            StopBtn.IsEnabled = false;
            ChooseFile.IsEnabled = true;
        }

        #endregion
    }

    #region Доп. классы
    public class ViewportAxesRangeRestriction : IViewportRestriction
    {
        public DisplayRange XRange = null;
        public DisplayRange YRange = null;

        public Rect Apply(Rect oldVisible, Rect newVisible, Viewport2D viewport)
        {
            if (XRange != null)
            {
                newVisible.X = XRange.Start;
                newVisible.Width = XRange.End - XRange.Start;
            }

            if (YRange != null)
            {
                newVisible.Y = YRange.Start;
                newVisible.Height = YRange.End - YRange.Start;
            }

            return newVisible;
        }

        public event EventHandler Changed;

    }

    public class DisplayRange
    {
        public double Start { get; set; }
        public double End { get; set; }

        public DisplayRange(double start, double end)
        {
            Start = start;
            End = end;
        }
    }

    #endregion
}
