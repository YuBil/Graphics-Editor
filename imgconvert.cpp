#include "imgconvert.h"

QImage MirrorImage(QImage source, int side)
{
	int x, y;
	QImage ret;

	ret = QImage(source); //source.width(), source.height(), source.format());
	for (x = 0; x<source.width(); x++)
	{
		for (y = 0; y<source.height(); y++)
		{
			ret.setPixel(((side & V_MIRROR) ? source.width() - (x + 1) : x), ((side & H_MIRROR) ? source.height() - (y + 1) : y), source.pixel(x, y));
		}
	}

	return ret;
}

QImage ConvertImage(IplImage *source)
{
	int cvIndex, cvLineStart;
	QImage ret;

	if (source->depth != IPL_DEPTH_8U || source->nChannels != 3)
		return ret;

	QImage temp(source->width, source->height, QImage::Format_RGB32);
	ret = temp;
	cvIndex = 0;
	cvLineStart = 0;
	for (int y = 0; y < source->height; y++)
	{
		unsigned char red, green, blue;
		cvIndex = cvLineStart;
		for (int x = 0; x < source->width; x++)
		{
			red = source->imageData[cvIndex + 2];
			green = source->imageData[cvIndex + 1];
			blue = source->imageData[cvIndex + 0];

			ret.setPixel(x, y, qRgb(red, green, blue));
			cvIndex += 3;
		}
		cvLineStart += source->widthStep;
	}

	return ret;
}

QImage ConvertImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS=1
	if (mat.type() == CV_8UC1)
	{
		// Set the color table (used to translate colour indexes to qRgb values)
		QVector<QRgb> colorTable;
		for (int i = 0; i<256; i++)
			colorTable.push_back(qRgb(i, i, i));
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
		img.setColorTable(colorTable);
		return img;
	}
	// 8-bits unsigned, NO. OF CHANNELS=3
	if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return img.rgbSwapped();
	}
	else
	{
		return QImage();
	}
}