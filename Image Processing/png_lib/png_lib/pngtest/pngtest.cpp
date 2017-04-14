// pngtest.cpp : Defines the entry point for the console application.
//

#define USE_LITERAL true

#define INPUT_FILE_NAME "_Count.png" //_Contrast, _Noise, _Impulse, _Count
#define OUTPUT_FILE_NAME "_Count1.png" //_Contrast1, _Noise1, _Impulse1, _Count1

#define WIDTH_SCALE 5
#define HEIGHT_SCALE 5

#define INPUT1_FILE_NAME "input1.png"
#define INPUT2_FILE_NAME "input2.png"
#define INPUT3_FILE_NAME "input3.png"
#define OUTPUT_GAUSS_FILE_NAME "output_gauss.png"
#define OUTPUT_IMP_FILE_NAME "output_imp.png"
#define OUTPUT_BLEND_FILE_NAME "output_blend.png"

#define fxONE 0x0400 // Единица в формате с фиксированной точкой
#define fxFRACTIONAL_NUM 10 // Количество бит в дробной части
#define fxFRACTIONAL_MASK 0x03FF // Маска дробной части чисел с фиксированной т.

#include "PngProc.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <iostream>
#include <algorithm>

using namespace std;

void ImageInversion(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight);

void ResizeBilinearFixedPoint(const unsigned char* pbIn, int lWidthIn,
	int lHeightIn, unsigned char *pbOut, int lWidthOut, int lHeightOut);

void ResizeBicubicFixedPoint(const unsigned char* pbIn, int lWidthIn,
	int lHeightIn, unsigned char *pbOut, int lWidthOut, int lHeightOut);

void BinaryByFloidSteinberg(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight);

void Binary(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight,
	unsigned char limit);

unsigned char GetPixel(const unsigned char* pb, size_t row, size_t col, int Width, int Height);
unsigned char GetPixelExpand(const unsigned char* pb, size_t row, size_t col, int Width, int Height, unsigned char expand);
void SetPixel(unsigned char* pb, size_t row, size_t col, int Width, int Height, unsigned char value);
int Lab1();
int Lab2();
int Lab3();
int Lab4();
int Lab5();
int Lab6();
int Lab7();
int K(int del);
int FixedMul(int a, int b);
int BicubicFixedPoint(int f_1, int f0, int f1, int f2, int u);
void AddGauseNoise(unsigned char* pOut, unsigned char* pIn, size_t nWidth, size_t nHeight);
void AddImpulseNoise(unsigned char* pOut, unsigned char* pIn, size_t nWidth, size_t nHeight);
void Blend(unsigned char* pOut, unsigned char* pIn1, unsigned char* pIn2, unsigned char* pInAlp, size_t nWidth, size_t nHeight);
int B(int p1, int p2);
void LinearFilter(const unsigned char* pSrc, unsigned char* pRes, int iWidth,
	int iHeight, const int* pKernel, int pWidth, int pHeight);
void MedianFilter(const unsigned char* pSrc, unsigned char* pRes, int iWidth,
	int iHeight, const int* pKernel, int pWidth, int pHeight);
int partitions(unsigned char* A, int low, int high);
int selection_algorithm(unsigned char* A, int width, int kth);
int FourConnectionRegionsCount(unsigned char* pIn, size_t nWidth, size_t nHeight);
void IncreaseContrast(unsigned char *pbRaw, int nWidth, int nHeight, int HT, int CT);
unsigned char HistogramSelection(unsigned char* Array, int width, int kth);



int main(int argc, char* argv[])
{
	Lab7();
}


int Lab1()
{
	// входные аргументы
	char szInputFileName[256];
	char szOutputFileName[256];
	strcpy(szInputFileName, INPUT_FILE_NAME);
	strcpy(szOutputFileName, OUTPUT_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInputBits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);

	//выделение памяти на выход
	unsigned char* pOutputBits = new unsigned char[nRetSize];

	// Обработка изображения 
	BinaryByFloidSteinberg(pOutputBits, pInputBits, nWidth, nHeight);

	//Запись результата в файл
	NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP);
	return 0;
}

int Lab4()
{
	// входные аргументы
	char szInputFileName[256];
	char szOutputFileName[256];
	strcpy(szInputFileName, INPUT_FILE_NAME);
	strcpy(szOutputFileName, OUTPUT_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInputBits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);

	//выделение памяти на выход
	unsigned char* pOutputBits = new unsigned char[nRetSize];

	// Обработка изображения 
	IncreaseContrast(pInputBits, nWidth, nHeight, nWidth*nHeight / 255, nWidth*nHeight / 50);

	//Запись результата в файл
	NPngProc::writePngFile(szOutputFileName, pInputBits, nWidth, nHeight, nBPP);
	return 0;
}


int Lab7()
{
	// входные аргументы
	char szInputFileName[256];
	char szOutputFileName[256];
	strcpy(szInputFileName, INPUT_FILE_NAME);
	strcpy(szOutputFileName, OUTPUT_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInputBits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);

	//выделение памяти на выход
	unsigned char* pOutputBits = new unsigned char[nRetSize];

	// Обработка изображения 
	unsigned char limit;
	if (USE_LITERAL)
		limit = 127;
	else
	{
		cout << "Enter limit:" << endl;
		cin >> limit;
	}

	Binary(pOutputBits, pInputBits, nWidth, nHeight, limit);
	NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP);

	cout << "Count of 4-connected regions: " << FourConnectionRegionsCount(pOutputBits, nWidth, nHeight) << endl;
	system("pause");

	return 0;
}


int Lab5()
{
	// входные аргументы
	char szInputFileName[256];
	char szOutputFileName[256];
	strcpy(szInputFileName, INPUT_FILE_NAME);
	strcpy(szOutputFileName, OUTPUT_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInputBits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);

	//выделение памяти на выход
	unsigned char* pOutputBits = new unsigned char[nRetSize];


	//ввод ядра
	int kW = 3, kH = 3;
	int* kernel;

	if (USE_LITERAL)
	{
		kernel = new int[kW*kH]{ 0, -1, 0, -1, 4, -1, 0, -1, 0 };//new int[kW*kH];// {1, 1, 1, 1, 1, 1};

		//for (int i = 0; i < kW*kH; i++)
		//	kernel[i] = 1;
	}
	else
	{
		cout << "You have to enter kernel matrix" << endl << "Enter width:" << endl;
		cin >> kW;
		cout << "Enter height:" << endl;
		cin >> kH;
		cout << "Elements:" << endl;
		kernel = new int[kW*kH];
		for (int i = 0; i < kW*kH; i++)
			cin >> kernel[i];
	}


	// Обработка изображения 
	LinearFilter(pInputBits, pOutputBits, nWidth, nHeight, kernel, kW, kH);

	//Запись результата в файл
	NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP);
	return 0;
}

int Lab6()
{
	// входные аргументы
	char szInputFileName[256];
	char szOutputFileName[256];
	strcpy(szInputFileName, INPUT_FILE_NAME);
	strcpy(szOutputFileName, OUTPUT_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInputBits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);

	//выделение памяти на выход
	unsigned char* pOutputBits = new unsigned char[nRetSize];


	//ввод ядра
	int kW = 3, kH = 3;
	int* kernel;

	if (USE_LITERAL)
	{
		kernel = new int[kW*kH];// {1, 1, 1, 1, 1, 1};

		for (int i = 0; i < kW*kH; i++)
			kernel[i] = 1;
	}
	else
	{
		cout << "You have to enter aperture matrix" << endl << "Enter width:" << endl;
		cin >> kW;
		cout << "Enter height:" << endl;
		cin >> kH;
		cout << "Elements:" << endl;
		kernel = new int[kW*kH];
		for (int i = 0; i < kW*kH; i++)
			cin >> kernel[i];
	}


	// Обработка изображения 
	MedianFilter(pInputBits, pOutputBits, nWidth, nHeight, kernel, kW, kH);

	//Запись результата в файл
	NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP);
	return 0;
}

int Lab2()
{
	// входные аргументы
	char szInputFileName[256];
	char szOutputFileName[256];
	strcpy(szInputFileName, INPUT_FILE_NAME);
	strcpy(szOutputFileName, OUTPUT_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInputBits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);

	//расчет выходных размеров и выделение под них памяти
	int nWidthOut = (nWidth*WIDTH_SCALE) >> 1, nHeightOut = (nHeight*HEIGHT_SCALE) >> 1;
	size_t nReqSizeOut = nReqSize*WIDTH_SCALE*HEIGHT_SCALE;
	unsigned char* pOutputBits = new unsigned char[nReqSizeOut];

	// Обработка изображения 
	ResizeBicubicFixedPoint(pInputBits, nWidth, nHeight, pOutputBits, nWidthOut, nHeightOut);

	//Запись результата в файл
	NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidthOut, nHeightOut, nBPP);
	return 0;
}

int Lab3()
{
	// входные аргументы
	char szInput1FileName[256];
	char szInput2FileName[256];
	char szInput3FileName[256];
	char szOutputGaussFileName[256];
	char szOutputImpFileName[256];
	char szOutputBlendFileName[256];
	strcpy(szInput1FileName, INPUT1_FILE_NAME);
	strcpy(szInput2FileName, INPUT2_FILE_NAME);
	strcpy(szInput3FileName, INPUT3_FILE_NAME);
	strcpy(szOutputGaussFileName, OUTPUT_GAUSS_FILE_NAME);
	strcpy(szOutputImpFileName, OUTPUT_IMP_FILE_NAME);
	strcpy(szOutputBlendFileName, OUTPUT_BLEND_FILE_NAME);

	//попытка чтения входного файла
	size_t nReqSize = NPngProc::readPngFile(szInput1FileName, 0, 0, 0, 0);

	//выделение памяти для входа
	unsigned char* pInput1Bits = new unsigned char[nReqSize];
	unsigned char* pInput2Bits = new unsigned char[nReqSize];
	unsigned char* pInput3Bits = new unsigned char[nReqSize];

	//чтение grayScale изображения
	size_t nWidth, nHeight;
	unsigned int nBPP = 8;
	size_t nRetSize = NPngProc::readPngFileGray(szInput1FileName, pInput1Bits, &nWidth, &nHeight/*, &nBPP*/);
	NPngProc::readPngFileGray(szInput2FileName, pInput2Bits, &nWidth, &nHeight/*, &nBPP*/);
	NPngProc::readPngFileGray(szInput3FileName, pInput3Bits, &nWidth, &nHeight/*, &nBPP*/);

	//выделение памяти на выход
	unsigned char* pbOutputGaussBits = new unsigned char[nRetSize];
	unsigned char* pbOutputImpBits = new unsigned char[nRetSize];
	unsigned char* pbOutputBlendBits = new unsigned char[nRetSize];

	// Обработка изображения 
	AddGauseNoise(pbOutputGaussBits, pInput2Bits, nWidth, nHeight);
	AddImpulseNoise(pbOutputImpBits, pInput1Bits, nWidth, nHeight);
	Blend(pbOutputBlendBits, pInput1Bits, pInput2Bits, pInput3Bits, nWidth, nHeight);

	//Запись результата в файл
	NPngProc::writePngFile(szOutputGaussFileName, pbOutputGaussBits, nWidth, nHeight, nBPP);
	NPngProc::writePngFile(szOutputImpFileName, pbOutputImpBits, nWidth, nHeight, nBPP);
	NPngProc::writePngFile(szOutputBlendFileName, pbOutputBlendBits, nWidth, nHeight, nBPP);
	return 0;
}


//Получение негатива
void ImageInversion(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight)
{
	for (size_t y = 0; y < nHeight; ++y)
	{
		for (size_t x = 0; x < nWidth; ++x)
		{
			pOut[x + nWidth*y] = 255 - pIn[x + nWidth*y];
		}
	}
	return;
}

void LinearFilter(const unsigned char* pSrc, unsigned char* pRes, int nWidth,
	int nHeight, const int* pKernel, int pWidth, int pHeight){

	int sum = 0;
	for (int i = 0; i < pWidth*pHeight; i++)
		sum += pKernel[i];
	int halfRow = pHeight >> 1;
	int halfCol = pWidth >> 1;

	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			int newValue = 0;

			for (int m_row = 0; m_row < pHeight; ++m_row)
			{
				for (int m_col = 0; m_col < pWidth; ++m_col)
				{
					newValue += GetPixel(pSrc, row + m_row - halfRow, col + m_col - halfCol, nWidth, nHeight)*pKernel[m_row*pWidth + m_col];
				}
			}


			if (sum != 0)
				newValue /= sum;


			if (newValue > 255)
				newValue = 255;
			if (newValue < 0)
				newValue = 0;

			
			SetPixel(pRes, row, col, nWidth, nHeight, newValue);
		}
	}

}


void MedianFilter(const unsigned char* pSrc, unsigned char* pRes, int nWidth,
	int nHeight, const int* pKernel, int pWidth, int pHeight){

	int count = 0;
	for (int i = 0; i < pWidth*pHeight; i++)
		count += pKernel[i];
	int halfRow = pHeight >> 1;
	int halfCol = pWidth >> 1;

	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			unsigned char *values = new unsigned char[count];
			int n = 0;

			for (int m_row = 0; m_row < pHeight; ++m_row)
			{
				for (int m_col = 0; m_col < pWidth; ++m_col)
				{
					unsigned char pixel = GetPixel(pSrc, row + m_row - halfRow, col + m_col - halfCol, nWidth, nHeight);

					for (int i = 0; i < pKernel[m_row*pWidth + m_col]; i++)
					{
						values[n] = pixel;
						n++;
					}
				}
			}

			unsigned char newValue = HistogramSelection(values, count, count);
			SetPixel(pRes, row, col, nWidth, nHeight, newValue);
		}
	}

}



//изменение размера с помощью билинейной интерполяции
void ResizeBilinearFixedPoint(const unsigned char* pbIn, int lWidthIn,
	int lHeightIn, unsigned char *pbOut, int lWidthOut, int lHeightOut)
{
	unsigned char* pbRawOut = pbOut;
	for (int row = 0; row < lHeightOut; ++row)
	{
		int yy = (int)(((long long)row << fxFRACTIONAL_NUM)*lHeightIn / lHeightOut),
			y = yy >> fxFRACTIONAL_NUM, // целая часть y
			u = yy & fxFRACTIONAL_MASK; // дробная часть y
		for (int col = 0; col < lWidthOut; ++col)
		{
			int xx = (int)(((long long)col << fxFRACTIONAL_NUM)*lWidthIn / lWidthOut),
				x = xx >> fxFRACTIONAL_NUM, // целая часть x
				v = xx & fxFRACTIONAL_MASK; // дробная часть x
			const unsigned char* pbRawIn = pbIn + y*lWidthIn + x; // значения соседних пикселов
			bool isLastCol = x == lWidthIn - 1, isLastRow = y == lHeightIn - 1; //учет крайних пикселов
			int lP00 = *pbRawIn,
				lP01 = !isLastCol ? *(pbRawIn + 1) : *pbRawIn,
				lP10 = !isLastRow ? *(pbRawIn + lWidthIn) : *pbRawIn,
				lP11 = (!isLastCol && !isLastRow) ? *(pbRawIn + lWidthIn + 1) :
				(isLastRow ? lP01 : lP10);
			// билинейная интерполяция
			*pbRawOut = (unsigned char)(((fxONE - u)*(fxONE - v)*lP00 +
				u*(fxONE - v)*lP10 + v*(fxONE - u)*lP01 +
				u*v*lP11) >> (2 * fxFRACTIONAL_NUM));
			++pbRawOut;
		}
	}
}


void BinaryByFloidSteinberg(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight)
{
	//копируем
	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			SetPixel(pOut, row, col, nWidth, nHeight, GetPixel(pIn, row, col, nWidth, nHeight));
		}
	}

	unsigned char *mx = new unsigned char[6] {0, 0, 7, 3, 5, 1};
	size_t mWidth = 3, mHeight = 2, shift = 4;

	//начинаем алгоритм
	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t _col = 0; _col < nWidth; ++_col)
		{

			bool turn = (row & 1 != 0);

			size_t col = (!turn) ? _col : nWidth - 1 - _col;


			//вычисляем ошибку
			unsigned char p = GetPixel(pOut, row, col, nWidth, nHeight);
			signed char E = p > 127 ? (p - 255) : p;

			//распространяем ошибку
			for (size_t m_row = 0; m_row < mHeight; ++m_row)
			{
				for (size_t m_col = 0; m_col < mWidth; ++m_col)
				{
					short right = m_col - (mWidth >> 1);

					if (turn)
						right = -right;

					if (m_row == 0 && right <= 0)
						continue;

					short newValue = GetPixel(pOut, row + m_row, col + right, nWidth, nHeight) + ((mx[m_row*mWidth + m_col] * E) >> shift);
					if (newValue > 255)
						newValue = 255;
					if (newValue < 0)
						newValue = 0;

					SetPixel(pOut, row + m_row, col + right, nWidth, nHeight, newValue);

				}
			}

			SetPixel(pOut, row, col, nWidth, nHeight, GetPixel(pOut, row, col, nWidth, nHeight) > 128 ? 255 : 0);
		}
	}
	return;
}


void Binary(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight,
	unsigned char limit)
{
	//начинаем алгоритм
	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			SetPixel(pOut, row, col, nWidth, nHeight, GetPixel(pIn, row, col, nWidth, nHeight) > limit ? 255 : 0);
		}
	}




	return;
}


int FourConnectionRegionsCount(unsigned char* pIn, size_t nWidth, size_t nHeight)
{
	unsigned char* Q1 = new unsigned char[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	int nQ1 = 0;
	unsigned char* Q3 = new unsigned char[16]{ 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0 };
	int nQ3 = 0;
	unsigned char* Q5 = new unsigned char[16]{ 0, 1, 1, 0, 1, 0, 0, 1};
	int nQ5 = 0;

	//пробегаемся по четверкам
	for (int row = -1; row < (int)nHeight; ++row)
	{
		for (int col = -1; col < (int)nWidth; ++col)
		{
			unsigned char* quart = new unsigned char[4]{
				GetPixelExpand(pIn, row, col, nWidth, nHeight, 255) == 255 ? (unsigned char)1 : (unsigned char)0,
					GetPixelExpand(pIn, row, col + 1, nWidth, nHeight, 255) == 255 ? (unsigned char)1 : (unsigned char)0,
					GetPixelExpand(pIn, row + 1, col, nWidth, nHeight, 255) == 255 ? (unsigned char)1 : (unsigned char)0,
					GetPixelExpand(pIn, row + 1, col + 1, nWidth, nHeight, 255) == 255 ? (unsigned char)1 : (unsigned char)0};

			for (int i = 0; i < 16; i += 4)
			{
				int a0 = quart[0], a1 = quart[1], a2 = quart[2], a3 = quart[3];

				if (quart[0] == Q1[i] && quart[1] == Q1[i + 1] && quart[2] == Q1[i + 2] && quart[3] == Q1[i + 3])
				{
					nQ1++;
					break;
				}
				if (quart[0] == Q3[i] && quart[1] == Q3[i + 1] && quart[2] == Q3[i + 2] && quart[3] == Q3[i + 3])
				{
					nQ3++;
					break;
				}

				if (i < 5 && quart[0] == Q5[i] && quart[1] == Q5[i + 1] && quart[2] == Q5[i + 2] && quart[3] == Q5[i + 3])
				{
					nQ5++;
					break;
				}
			}
		}
	}

	double a = (nQ3 - nQ1 - 2*nQ5) / 4.0;

	return (int)(ceil(a));
}

//изменение размера с помощью бикубической интерполяции
void ResizeBicubicFixedPoint(const unsigned char* pbIn, int lWidthIn,
	int lHeightIn, unsigned char *pbOut, int lWidthOut, int lHeightOut)
{

	int percent = 0;
	for (int row = 0; row < lHeightOut; ++row)
	{
		/*	if (row * 10 / lHeightOut != percent)
			{
			percent = row * 10 / lHeightOut;
			printf("%d0 percent complete.\n", percent);
			}*/

		int yy = (int)(((long long)row << fxFRACTIONAL_NUM)*lHeightIn / lHeightOut),
			y = yy >> fxFRACTIONAL_NUM, // целая часть y
			u = yy & fxFRACTIONAL_MASK; // дробная часть y
		for (int col = 0; col < lWidthOut; ++col)
		{
			int xx = (int)(((long long)col << fxFRACTIONAL_NUM)*lWidthIn / lWidthOut),
				x = xx >> fxFRACTIONAL_NUM, // целая часть x
				v = xx & fxFRACTIONAL_MASK; // дробная часть x

			//считаем координаты
			int p_1_1 = GetPixel(pbIn, y - 1, x - 1, lWidthIn, lHeightIn),
				p_10 = GetPixel(pbIn, y - 1, x, lWidthIn, lHeightIn),
				p_11 = GetPixel(pbIn, y - 1, x + 1, lWidthIn, lHeightIn),
				p_12 = GetPixel(pbIn, y - 1, x + 2, lWidthIn, lHeightIn),
				p0_1 = GetPixel(pbIn, y, x - 1, lWidthIn, lHeightIn),
				p00 = GetPixel(pbIn, y, x, lWidthIn, lHeightIn),
				p01 = GetPixel(pbIn, y, x + 1, lWidthIn, lHeightIn),
				p02 = GetPixel(pbIn, y, x + 2, lWidthIn, lHeightIn),
				p1_1 = GetPixel(pbIn, y + 1, x - 1, lWidthIn, lHeightIn),
				p10 = GetPixel(pbIn, y + 1, x, lWidthIn, lHeightIn),
				p11 = GetPixel(pbIn, y + 1, x + 1, lWidthIn, lHeightIn),
				p12 = GetPixel(pbIn, y + 1, x + 2, lWidthIn, lHeightIn),
				p2_1 = GetPixel(pbIn, y + 2, x - 1, lWidthIn, lHeightIn),
				p20 = GetPixel(pbIn, y + 2, x, lWidthIn, lHeightIn),
				p21 = GetPixel(pbIn, y + 2, x + 1, lWidthIn, lHeightIn),
				p22 = GetPixel(pbIn, y + 2, x + 2, lWidthIn, lHeightIn);


			// бикубическая интерполяция
			unsigned char f_1 = (unsigned char)(BicubicFixedPoint(p_1_1, p_10, p_11, p_12, v) >> (fxFRACTIONAL_NUM));
			unsigned char f0 = (unsigned char)(BicubicFixedPoint(p0_1, p00, p01, p02, v) >> (fxFRACTIONAL_NUM));
			unsigned char f1 = (unsigned char)(BicubicFixedPoint(p1_1, p10, p11, p12, v) >> (fxFRACTIONAL_NUM));
			unsigned char f2 = (unsigned char)(BicubicFixedPoint(p2_1, p20, p21, p22, v) >> (fxFRACTIONAL_NUM));
			unsigned char value = (unsigned char)(BicubicFixedPoint(f_1, f0, f1, f2, u) >> (fxFRACTIONAL_NUM));

			/*unsigned char value = (unsigned char)(((fxONE - u)*(fxONE - v)*p00 +
				u*(fxONE - v)*p10 + v*(fxONE - u)*p01 +
				u*v*p11) >> (2 * fxFRACTIONAL_NUM));*/

			SetPixel(pbOut, row, col, lWidthOut, lHeightOut, value);
		}
	}
}



unsigned char GetPixel(const unsigned char* pb, size_t row, size_t col, int Width, int Height)
{
	if (row < 0)
		row = 0;
	if (col < 0)
		col = 0;
	if (row >= Height)
		row = Height - 1;
	if (col >= Width)
		col = Width - 1;
	return pb[col + row*Width];
}

unsigned char GetPixelExpand(const unsigned char* pb, size_t row, size_t col, int Width, int Height, unsigned char expand)
{
	if (row < 0 || col < 0 || row >= Width || col >= Height)
		return expand;
	return pb[col + row*Width];
}


void SetPixel(unsigned char* pb, size_t row, size_t col, int Width, int Height, unsigned char value)
{
	if (row < 0 || row >= Height || col < 0 || col >= Width)
		return;

	pb[row*Width + col] = value;
}


//кубическая одномерная интерполяция
int BicubicFixedPoint(int f_1, int f0, int f1, int f2, int u)
{
	int res = f_1*K(u + fxONE) + f0*K(u) + f1*K(fxONE - u) + f2*K(2 * fxONE - u);
	if (res < 0)
		res = 0;
	return res;
}

//вспомогательная ф-ия для кубическая одномерная интерполяция
int K(int del)
{
	int d = (del >= 0) ? del : -del;
	if (d <= fxONE)
		return FixedMul(d, FixedMul(d, d)) - 2 * FixedMul(d, d) + fxONE;
	else
	if (d <= 2 * fxONE)
		return -FixedMul(d, FixedMul(d, d)) + 5 * FixedMul(d, d) - 8 * d + 4 * fxONE;
	else
		return 0;
}

//умножение 2 fixedPoint
int FixedMul(int a, int b)
{
	long d = (long)a * (long)b;
	return a*b >> fxFRACTIONAL_NUM;
}

void AddGauseNoise(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0.0, 20.0);

	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			int noise = (int)distribution(generator);
			int newPixel = GetPixel(pIn, row, col, nWidth, nHeight) + noise;
			if (newPixel > 255)
				newPixel = 255;
			if (newPixel < 0)
				newPixel = 0;

			SetPixel(pOut, row, col, nWidth, nHeight, newPixel);
		}
	}

	return;
}


void AddImpulseNoise(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight)
{
	srand(time(NULL));
	int p0 = RAND_MAX / 32;
	int p255 = RAND_MAX / 32;

	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			unsigned char value = GetPixel(pIn, row, col, nWidth, nHeight);
			int p = rand();
			if (p < p0 + p255)
			{
				value = p < p0 ? 0 : 255;
			}

			SetPixel(pOut, row, col, nWidth, nHeight, value);
		}
	}

	return;
}

void Blend(unsigned char* pOut, unsigned char* pIn1, unsigned char* pIn2, unsigned char* pInAlp, size_t nWidth, size_t nHeight)
{
	for (size_t row = 0; row < nHeight; ++row)
	{
		for (size_t col = 0; col < nWidth; ++col)
		{
			unsigned int p1 = (unsigned int)GetPixel(pIn1, row, col, nWidth, nHeight);
			unsigned int p2 = (unsigned int)GetPixel(pIn2, row, col, nWidth, nHeight);
			unsigned char a = (unsigned int)GetPixel(pInAlp, row, col, nWidth, nHeight);

			unsigned char a1 = a;
			unsigned char a2 = 255 - a;
			/*	short newPixel = ((p1*a + p2*(255 - a)) >> 8);
				if (newPixel > 255)
				newPixel = 255;
				if (newPixel < 0)
				newPixel = 0;*/

			unsigned char newPixel = (unsigned char)(((255 - a1)*a2*p2 + (255 - a2)*a1*p1 + a1*a2*B(p1, p2)) >> 16);
			SetPixel(pOut, row, col, nWidth, nHeight, (unsigned char)newPixel);
		}
	}

	return;
}

//функция смешивания
int B(int p1, int p2)
{
	//return 255 - ((255-p1)*(255-p2) >> 8);
	return p1*p2 >> 8;
	//return (p2 > p1) ? p1 : p2;
}



int partitions(unsigned char* A, int low, int high)
{
	int p = low, r = high, x = A[r], i = p - 1;
	for (int j = p; j <= r - 1; j++)
	{
		if (A[j] <= x)
		{

			i = i + 1;
			swap(A[i], A[j]);
		}
	}
	swap(A[i + 1], A[r]);
	return i + 1;
}
int selection_algorithm(unsigned char* A, int width, int kth)
{
	int left = 0, right = width - 1;
	for (;;)
	{
		int pivotIndex = partitions(A, left, right);          //Select the Pivot Between Left and Right
		int len = pivotIndex - left + 1;

		if (kth == len)
			return A[pivotIndex];

		else if (kth < len)
			right = pivotIndex - 1;

		else
		{
			kth = kth - len;
			left = pivotIndex + 1;
		}
	}
}


unsigned char HistogramSelection(unsigned char* Array, int width, int kth)
{

	int* hist = new int[256];

	for (int i = 0; i < 256; i++)
	{
		hist[i] = 0;
	}
	for (int i = 0; i < width; i++)
	{
		hist[Array[i]]++;
	}

	unsigned char res = 0;
	int count = 0;
	while (count < kth)
	{
		count += hist[res];
		res++;
	}
	res--;

	return res;
}



void IncreaseContrast(unsigned char *pbIn, int nWidth, int nHeight, int HT, int CT)
{
	// построекние гистограммы
	long long *hist = new long long[256];
	for (int i = 0; i < 256; i++)
		hist[i] = 0;
	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			hist[GetPixel(pbIn, row, col, nWidth, nHeight)]++;
		}
	}

	// нахождение левой границы для контрастирования
	int lLeftLim, lRightLim;
	long lSum = 0;
	for (lLeftLim = 0; lLeftLim < 100; lLeftLim++)
	{
		if (hist[lLeftLim] > HT)
			break;
		lSum += hist[lLeftLim];
		if (lSum > CT)
			break;
	}

	// нахождение правой границы для контрастирования
	lSum = 0;
	for (lRightLim = 255; lRightLim > 150; lRightLim--)
	{
		if (hist[lRightLim] > HT)
			break;
		lSum += hist[lRightLim];
		if (lSum > CT)
			break;
	}

	// вычисление таблицы перекодировки (LUT, Look-Up Table)
	unsigned char *abLUT = new unsigned char[256];
	for (int i = 0; i < lLeftLim; i++) { abLUT[i] = (unsigned char)0; }
	for (int i = lLeftLim; i < lRightLim; i++)
	{
		double gamma = 0.7;
		double result = 255 * pow(((i - lLeftLim)*1.0 / (lRightLim - lLeftLim)),1.0/gamma);

		abLUT[i] = (unsigned char)trunc(result);
	}
	for (int i = lRightLim; i < 256; i++) { abLUT[i] = (unsigned char)255; }

	// замена пикселей
	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			SetPixel(pbIn, row, col, nWidth, nHeight, abLUT[GetPixel(pbIn, row, col, nWidth, nHeight)]);
		}
	}
}
