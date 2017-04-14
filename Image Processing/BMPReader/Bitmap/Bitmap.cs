using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bitmap
{
    #region BitReader

    /// <summary>
    /// Содержит статические методы для приведения битов к типу
    /// </summary>
    static class BitReader
    {
        public static int ReadWORD(byte[] bytes, int position)
        {
            return (int)BitConverter.ToUInt16(bytes, position);
        }

        public static byte ReadBYTE(byte[] bytes, int position)
        {
            return bytes[position];
        }

        public static int ReadDWORD(byte[] bytes, int position)
        {
            return (int)BitConverter.ToUInt32(bytes, position);
        }

        public static int ReadLONG(byte[] bytes, int position)
        {
            return BitConverter.ToInt32(bytes, position);
        }
    }

    #endregion

    #region Доп. классы

    /// <summary>
    /// Цвет в RGB
    /// </summary>
    public class RGBColor
    {
        public byte r { get; set; }
        public byte g { get; set; }
        public byte b { get; set; }
    }

    /// <summary>
    /// Тип алгоритма в медианной фильтрации
    /// </summary>
    public enum MedianFilterType
    {
        Selection,
        Histogram,
        HistogramOptimized
    }

    #endregion

    public class Bitmap
    {
        #region Поля

        /// <summary>
        /// битность
        /// </summary>
        int bpp;

        /// <summary>
        /// размер infoHeadera(определяет версию)
        /// </summary>
        int iSize; 

        /// <summary>
        /// Ширина
        /// </summary>
        public int Width { get; private set; }

        /// <summary>
        /// Высота
        /// </summary>
        public int Height { get; private set; }

        //пиксели в RGB (или палитра)
        byte[] r;
        byte[] g;
        byte[] b;

        /// <summary>
        /// пиксели с палитрой
        /// </summary>
        byte[] p;

        #endregion

        #region Вычисляемые Свойства

        /// <summary>
        /// Это палитровое изображение
        /// </summary>
        bool IsPalette
        {
            get
            {
                return bpp <= 8;
            }
        }

        /// <summary>
        /// Это Win-формат
        /// </summary>
        bool IsWin
        {
            get
            {
                return iSize == 40;
            }
        }

        /// <summary>
        /// Это OS-формат
        /// </summary>
        bool IsOs
        {
            get
            {
                return iSize == 12;
            }
        }

        #endregion

        #region Конструкторы

        //создать чистый Bitmap
        private Bitmap() { }

        /// <summary>
        /// считать Bmp из файла
        /// </summary>
        public Bitmap(string fileName)
        {
            byte[] bytes = File.ReadAllBytes(fileName);

            //считываем параметры файла
            int bfType = BitReader.ReadWORD(bytes, 0x00);
            if (bfType != 0x4D42 && bfType != 0x424D)
                throw new FileLoadException("Файл помечен не как bmp");
            int Position = BitReader.ReadDWORD(bytes, 0x0A);//позиция старта пикс. данных

            iSize = BitReader.ReadDWORD(bytes, 0x0E);

            int compression;
            if (IsOs)
            {
                //Os-2 версия
                Width = BitReader.ReadWORD(bytes, 0x12);
                Height = BitReader.ReadWORD(bytes, 0x14);
                bpp = BitReader.ReadWORD(bytes, 0x18);
                compression = 0;
            }
            else if (IsWin)
            {
                //Windows версия
                Width = BitReader.ReadDWORD(bytes, 0x12);
                Height = BitReader.ReadDWORD(bytes, 0x16);
                bpp = BitReader.ReadWORD(bytes, 0x1C);
                compression = BitReader.ReadDWORD(bytes, 0x1E);
            }
            else
            {
                throw new FileLoadException("Поддерживаются только версии OS/2 и Windows");
            }


            //чтение поддерживаемых вариантов
            switch (bpp)
            {
                case 24:
                    {
                        //беспалитровое изображение
                        InitRgb(Width * Height);

                        int i = Position;
                        int pixelsInLineCount = 0;//для детектирования смещения

                        //читаем пиксели
                        while (i < bytes.Length - 2)
                        {
                            AddRgb(bytes[i + 2], bytes[i + 1], bytes[i]);

                            i += 3;
                            pixelsInLineCount += 1;
                            if (pixelsInLineCount == Width)//конец строки
                            {
                                i += 4 - (Width * 3) % 4; //смещение
                                pixelsInLineCount = 0;
                            }
                        }
                    }
                    break;
                case 8:
                    {
                        //палитра
                        InitRgb(256);

                        int i;
                        int shift = IsWin ? 4 : 3;//смещение
                        int palSize = IsWin ? 1024 : 768;//размер палитры

                        //читаем палитру
                        for (i = Position - palSize; i < Position; i += shift)
                        {
                            AddRgb(bytes[i + 2], bytes[i + 1], bytes[i]);
                        }

                        InitP(Width * Height);
                        if (compression == 0)
                        {
                            //пиксели
                            i = Position;
                            int pixelsInLineCount = 0;
                            while (i < bytes.Length)
                            {
                                AddP(bytes[i]);

                                pixelsInLineCount++;
                                i++;
                                if (pixelsInLineCount == Width)//конец строки
                                {
                                    i += 4 - Width % 4; //смещение
                                    pixelsInLineCount = 0;
                                }
                            }
                        }
                        else if (compression == 1)
                        {
                            //RLE
                            int AbsoluteMode = 0;//число пикселей до конца AbsoluteMode
                            i = Position;
                            while (i < bytes.Length - 2)
                            {
                                if (AbsoluteMode > 0)
                                {
                                    AddP(bytes[i]);//непосредственно значение
                                    AbsoluteMode--;
                                    i++;
                                }
                                else//ввод команды
                                {
                                    if (i % 2 != 0)//команда должна начинаться с четной позиции
                                    {
                                        i++;
                                        continue;
                                    }

                                    byte first = bytes[i];
                                    byte second = bytes[i + 1];
                                    if (first != 0)
                                    {
                                        //повторить пиксель
                                        for (int j = 0; j < first; j++)
                                            AddP(second);
                                    }
                                    else
                                    {
                                        if (second > 2)//конец строки и файла - ничего не делаем
                                        {
                                            AbsoluteMode = second;
                                        }
                                    }
                                    i += 2;
                                }
                            }
                        }
                        else
                        {
                            throw new FileLoadException("Данный тип сжатия не поддерживается");
                        }
                    }
                    break;
                default:
                    throw new FileLoadException(String.Format("Bmp битностью {0}bpp не поддерживается", bpp));
            }

        }

        #endregion

        #region Public методы

        /// <summary>
        /// получить значение пикселя
        /// </summary>
        public RGBColor GetRGBPixel(int row, int col)
        {
            int n = IsPalette ? row * Width + col > p.Count() - 1 ? 0 : p[row * Width + col] : row * Width + col;
            return new RGBColor()
            {
                r = n > r.Count() - 1 ? (byte)0 : r[n],
                g = n > r.Count() - 1 ? (byte)0 : g[n],
                b = n > r.Count() - 1 ? (byte)0 : b[n]
            };
        }

        /// <summary>
        /// получить значение пикселя
        /// </summary>
        public byte GetPixel(int row, int col)
        {
            if (!IsPalette) throw new Exception("Bitmap не палитровый");

            if (row < 0)
                row = 0;
            if (col < 0)
                col = 0;
            if (row >= Height)
                row = Height - 1;
            if (col >= Width)
                col = Width - 1;

            return p[col + row * Width];
        }


        /// <summary>
        /// Клонировать объект в другой Bitmap
        /// </summary>
        public Bitmap Clone()
        {
            Bitmap clone = new Bitmap();
            clone.bpp = this.bpp;
            clone.iSize = this.iSize;
            clone.Width = this.Width;
            clone.Height = this.Height;
            clone.r = this.r.ToArray();
            clone.g = this.g.ToArray();
            clone.b = this.b.ToArray();
            clone.p = this.p.ToArray();
            return clone;
        }


        #endregion

        #region Private методы

        /// <summary>
        /// Текущая позиция записи в массив RGB
        /// </summary>
        private int ArrPosRGB;
        /// <summary>
        /// Создать массивы под RGB
        /// </summary>
        private void InitRgb(int size)
        {
            r = new byte[size];
            g = new byte[size];
            b = new byte[size];
            ArrPosRGB = 0;
        }

        /// <summary>
        /// Добавить в след. позицию (ArrPosRgb) новое значение
        /// </summary>
        private void AddRgb(byte _r, byte _g, byte _b)
        {
            b[ArrPosRGB] = _b;
            g[ArrPosRGB] = _g;
            r[ArrPosRGB] = _r;
            ArrPosRGB++;
        }

        /// <summary>
        /// Текущая позиция записи в массив P
        /// </summary>
        private int ArrPosP;

        /// <summary>
        /// Создать массивы под пикслели в палитровом изображении
        /// </summary>
        private void InitP(int size)
        {
            p = new byte[size];
            ArrPosP = 0;
        }
        
        /// <summary>
        /// Добавить в след. позицию (ArrPosP) новое значение
        /// </summary>
        private void AddP(byte _p)
        {
            if (ArrPosP < p.Length)
            {
                p[ArrPosP] = _p;
                ArrPosP++;
            }
        }

        /// <summary>
        /// Установить значение value в пикслель 
        /// </summary>
        private void SetPixel(int row, int col, byte value)
        {
            if (row < 0 || row >= Height || col < 0 || col >= Width)
                return;

            p[row * Width + col] = value;
        }

        #endregion

        #region Алгоритмы


        /// <summary>
        /// применить медианную фильтрацию
        /// </summary>
        public static Bitmap MedianFilter(Bitmap source, byte[] pKernel, int pWidth, int pHeight, out double time, MedianFilterType Type)
        {
            if (!source.IsPalette)
                throw new Exception("Медианный фильтр применим только к палитровым изображениям");

            var result = source.Clone();

            DateTime start = DateTime.Now;//подсчет времени

            int count = 0;
            for (int i = 0; i < pKernel.Length; i++)
                count += pKernel[i];

            int halfRow = pHeight >> 1;
            int halfCol = pWidth >> 1;

            if (Type != MedianFilterType.HistogramOptimized)
            {
                //алгоритм с построением массива
                for (int row = 0; row < source.Height; ++row)
                {
                    for (int col = 0; col < source.Width; ++col)
                    {
                        //строим массив
                        byte[] values = new byte[count];
                        int n = 0;
                        for (int m_row = 0; m_row < pHeight; ++m_row)
                        {
                            for (int m_col = 0; m_col < pWidth; ++m_col)
                            {
                                byte pixel = source.GetPixel(row + m_row - halfRow, col + m_col - halfCol);

                                for (int i = 0; i < pKernel[m_row * pWidth + m_col]; i++)
                                {
                                    values[n] = pixel;
                                    n++;
                                }
                            }
                        }

                        //ищем медиану
                        byte newValue = Convert.ToByte(Type == MedianFilterType.Histogram ?
                            HistogramSelection(values, count / 2) :
                            SelectionAlgorithm(values, count / 2));
                        result.SetPixel(row, col, newValue);
                    }
                }
            }
            else
            {
                //постоение гр=истограммы всемто массива
                int[] hist = new int[256];
                var median = count / 2 > 0 ? count / 2 : 1;

                for (int row = 0; row < source.Height; ++row)
                {
                    for (int _col = 0; _col < source.Width; ++_col)
                    {
                        bool isTurn = row % 2 == 1;
                        int col = isTurn ? source.Width - _col - 1 : _col;//для прохода змейкой

                        if (_col == 0 && row == 0)
                        {
                            //первый раз заполняем честно
                            for (int i = 0; i < 256; i++)
                                hist[i] = 0;

                            for (int m_row = 0; m_row < pHeight; ++m_row)
                            {
                                for (int m_col = 0; m_col < pWidth; ++m_col)
                                {
                                    hist[source.GetPixel(row + m_row - halfRow, col + m_col - halfCol)]++;
                                }
                            }
                        }
                        else if (_col == 0)
                        {
                            //произошло сдвигание по вертикали 
                            for (int m_col = 0; m_col < pWidth; ++m_col)
                            {
                                int m_row = -1;//вычитание старого
                                hist[source.GetPixel(row + m_row - halfRow, col + m_col - halfCol)]--;

                                m_row = pWidth - 1;//добавление нового
                                hist[source.GetPixel(row + m_row - halfRow, col + m_col - halfCol)]++;
                            }
                        }
                        else
                        {
                            //произошло сдвигание по горизонтали 
                            for (int m_row = 0; m_row < pHeight; ++m_row)
                            {
                                int m_col = isTurn ? pWidth : - 1;//вычитание старого
                                hist[source.GetPixel(row + m_row - halfRow, col + m_col - halfCol)]--;

                                m_col = isTurn ? 0 : pWidth - 1;//добавление нового
                                hist[source.GetPixel(row + m_row - halfRow, col + m_col - halfCol)]++;
                            }
                        }

                        //поиск медианы
                        byte newValue = 0;
                        int countInHist = 0;
                        while (countInHist < median)
                        {
                            countInHist += hist[newValue];
                            newValue++;
                        }
                        newValue--;

                        result.SetPixel(row, col, newValue);
                    }
                }
            }

            DateTime end = DateTime.Now;
            time = (end - start).TotalMilliseconds;

            return result;
        }

        /// <summary>
        /// Выбрать kth элемент массива Array с помощью гистограммы
        /// </summary>
        static byte HistogramSelection(byte[] Array, int kth)
        {
            if (kth == 0) kth++;

            //построение гистограммы
            int[] hist = new int[256];
            foreach (byte a in Array)
            {
                hist[a]++;
            }

            //поиск
            byte res = 0;
            int count = 0;
            while (count < kth)
            {
                count += hist[res];
                res++;
            }
            res--;

            return res;
        }


        /// <summary>
        /// Выбрать kth элемент массива Array с помощью SelectionAlgorithm
        /// </summary>
        static byte SelectionAlgorithm(byte[] arr, int k)
        {
            //границы внешнего цикла
            int from = 0, to = arr.Length - 1;

            // Если from==to мы достигли k-й элемент
            while (from < to)
            {
                //границы внутреннего цикла
                int r = from, w = to;
                int mid = arr[(r + w) / 2];

                // тормозим, когдп r==w
                while (r < w)
                {

                    if (arr[r] >= mid)
                    {
                        //вставляем большое значение в конец
                        byte tmp = arr[w];
                        arr[w] = arr[r];
                        arr[r] = tmp;
                        w--;
                    }
                    else
                    {
                        //значение меньше mid, двигаемся
                        r++;
                    }
                }

                if (arr[r] > mid)
                    r--;

                // определяем для какого куска повторять
                if (k <= r)
                {
                    to = r;
                }
                else
                {
                    from = r + 1;
                }
            }

            return arr[k];
        }


        #endregion
    }



}



