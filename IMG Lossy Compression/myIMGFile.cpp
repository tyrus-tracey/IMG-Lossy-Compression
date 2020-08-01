#include "myIMGFile.h"

colSpace::colSpace()
	: Y(0), Co(0), Cg(0)
{
}

colSpace::colSpace(wxColor triColor)
{
	Co = triColor.Red() - triColor.Blue();
	int temp = triColor.Blue() + Co / 2;
	Cg = triColor.Green() - temp;
	Y = temp + Cg / 2;
}

myIMGFile::myIMGFile()
	:imageSize(wxSize(0,0))
{
}

// Copy pixel data to .IMG file, converting to YCoCg in the process
myIMGFile::myIMGFile(myBMPFile& bmp)
	: imageSize(bmp.getImageSize())
{
	dataY  = vector<string>(bmp.getPixelCount());
	dataCo = vector<string>(bmp.getPixelCount());
	dataCg = vector<string>(bmp.getPixelCount());
	readBMP(bmp);
	downsampleColor();
	quantizePixels();
	flattenExtremes();
	convertToStrings();
}

void myIMGFile::readBMP(myBMPFile& bmp)
{
	vector<colSpace> width(imageSize.GetX());
	data = vector<vector<colSpace>>(imageSize.GetY(), width);

	row = data.begin();
	bmp.row = bmp.getPixelVector()->begin();
	while (row != data.end()) {
		col = row->begin();
		bmp.col = bmp.row->begin();
		while (col != row->end()) {
			*col = colSpace(*bmp.col);
			col++, bmp.col++;
		}
		row++, bmp.row++;
	}
}

// Group CoCg values into averaged blocks of 4, then set block CoCg to averaged value
void myIMGFile::downsampleColor()
{
	colSpace avg;
	int x1;
	int y1;
	int x2;
	int y2;

	for (x1 = 0; x1 < imageSize.GetWidth()-1; x1 += 2) {
		x2 = x1 + 1; 
		for (y1 = 0; y1 < imageSize.GetHeight()-1; y1 += 2) {
			y2 = y1 + 1;
			
			// sum 4x4 blocks
			avg.Co = data[y1][x1].Co + data[y2][x1].Co + data[y1][x2].Co + data[y2][x2].Co;
			avg.Cg = data[y1][x1].Cg + data[y2][x1].Cg + data[y1][x2].Cg + data[y2][2].Cg;
			// divide by 4
			avg.Co / 4, avg.Cg / 4;

			data[y1][x1].Co = avg.Co;  
			data[y2][x1].Co = avg.Co;
			data[y1][x2].Co = avg.Co;
			data[y2][x2].Co = avg.Co;
				 	 
			data[y1][x1].Cg = avg.Cg;
			data[y2][x1].Cg = avg.Cg;
			data[y1][x2].Cg = avg.Cg;
			data[y2][x2].Cg = avg.Cg;
		}
	}
}

// Quantize YCoCg values to even numbers
void myIMGFile::quantizePixels()
{
	row = data.begin();
	while (row != data.end()) {
		col = row->begin();
		while (col != row->end()) {
			col->Y	-= col->Y % 2;
			col->Co -= col->Co % 2;
			col->Cg -= col->Cg % 2;
			col++;
		}
		row++;
	}
}

// Near-max values get pushed to max values. (harder to discern)
void myIMGFile::flattenExtremes()
{
	row = data.begin();
	while (row != data.end()) {
		col = row->begin();
		while (col != row->end()) {
			if (col->Y < 5) {
				col->Y = 5;
			}
			else if (col->Y > 250) {
				col->Y = 255;
			}

			if (col->Co < 5) {
				col->Co = 5;
			}
			else if (col->Co > 250) {
				col->Co = 255;
			}

			if (col->Cg < 5) {
				col->Cg = 5;
			}
			else if (col->Cg > 250) {
				col->Cg = 255;
			}
			col++;
		}
		row++;
	}
}

void myIMGFile::convertToStrings()
{
	int index = 0;
	row = data.begin();
	while (row != data.end()) {
		col = row->begin();
		while (col != row->end()) {
			dataY[index] = to_string(col->Y);
			dataCo[index] = to_string(col->Co);
			dataCg[index] = to_string(col->Cg);
			index++, col++;
		}
		row++;
	}
}

// Huffman-based encoding of pixel values
void myIMGFile::encode()
{

}

void myIMGFile::writeToFile(const wxString filepath)
{
	wxFFile file(filepath, "wb");
	int intBuff;
	char charBuff[4];
	charBuff[0] = 'I';
	charBuff[1] = 'M';
	charBuff[2] = 'G';
	charBuff[3] = '\0';

	//write header tag
	file.Write(charBuff, sizeof(charBuff));
	//image width
	intBuff = imageSize.GetWidth();
	file.Write(&intBuff, sizeof(int));
	//image height
	intBuff = imageSize.GetHeight();
	file.Write(&intBuff, sizeof(int));

	//write Y values;
	for (int i = 0; i < imageSize.GetWidth() * imageSize.GetHeight(); i++) {
		file.Write(dataY[i]);
	}

	//write Co values
	for (int i = 0; i < imageSize.GetWidth() * imageSize.GetHeight(); i++) {
		file.Write(dataCo[i]);

	}

	//write Cg values
	for (int i = 0; i < imageSize.GetWidth() * imageSize.GetHeight(); i++) {
		file.Write(dataCg[i]);
	}

	file.Close();
}