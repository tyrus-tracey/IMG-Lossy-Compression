#include "myIMGFile.h"

colSpace::colSpace()
	: Y(0), Co(0), Cg(0)
{
}

colSpace::colSpace(wxColor triColor)
{
	double temp;
	Co = triColor.Red() - triColor.Blue();
	temp = double(triColor.Blue()) + (Co >> 1);
	Cg = double(triColor.Green()) - temp;
	Y = temp + (Cg >> 1);
}

myIMGFile::myIMGFile()
	: imageSize(wxSize(0,0)), downsampleCount(0), data(vector<vector<colSpace>>(0)), dataY(0), dataCo(0), dataCg(0)
{
}

myIMGFile::myIMGFile(const wxString filepath)
	: imageSize(wxSize(0, 0)), downsampleCount(0), data(vector<vector<colSpace>>(0)), dataY(0), dataCo(0), dataCg(0)
{
	readFromFile(filepath);
	reconstructPixels();
}

// Copy pixel data to .IMG file, converting to YCoCg in the process
myIMGFile::myIMGFile(myBMPFile& bmp)
	: imageSize(bmp.getImageSize())
{
	dataY  = vector<int>(bmp.getPixelCount());
	dataCo = vector<int>(bmp.getPixelCount());
	dataCg = vector<int>(bmp.getPixelCount());
	readBMP(bmp);
	downsampleColor();
	//quantizePixels();
	//flattenExtremes();
	convertToStrings();
}

void myIMGFile::readFromFile(const wxString filepath)
{
	char charBuff[4];
	int intBuff;
	wxFFile file(filepath, "rb");
	// Read header tag
	file.Read(charBuff, 4);
	if (strcmp(charBuff, "IMG")) {
		wxMessageBox("Error: Selected file is not IMG format.");
		return;
	}
	// Read image dimensions
	file.Read(&intBuff, sizeof(int));
	imageSize.SetWidth(intBuff);
	file.Read(&intBuff, sizeof(int));
	imageSize.SetHeight(intBuff);
	// Read downsample size
	file.Read(&intBuff, sizeof(int));
	downsampleCount = intBuff;

	dataY.resize(imageSize.GetWidth() * imageSize.GetHeight());
	dataCo.resize(downsampleCount); 
	dataCg.resize(downsampleCount); 
	int i;
	for (i = 0; i < imageSize.GetWidth() * imageSize.GetHeight(); i++) {
		file.Read(&intBuff, sizeof(int));
		dataY[i] = intBuff;
	}

	for (i = 0; i < downsampleCount; i++) { 
		file.Read(&intBuff, sizeof(int));
		dataCo[i] = intBuff;
	}

	for (i = 0; i < downsampleCount; i++) { 
		file.Read(&intBuff, sizeof(int));
		dataCg[i] = intBuff;
	}
	file.Close();
}

void myIMGFile::readBMP(myBMPFile& bmp)
{
	vector<colSpace> width(imageSize.GetX());
	data = vector<vector<colSpace>>(imageSize.GetY(), width);
	int pixels = imageSize.GetHeight() * imageSize.GetWidth();
	pixels -= 20;
	int count = 0;
	row = data.begin();
	bmp.row = bmp.getPixelVector()->begin();
	while (row != data.end()) {
		col = row->begin();
		bmp.col = bmp.row->begin();
		while (col != row->end()) {
			*col = colSpace(*bmp.col);
			col++, bmp.col++;
			count++;
		}
		row++, bmp.row++;
	}
}

// Group CoCg values into averaged blocks of 4, then set block CoCg to averaged value
void myIMGFile::downsampleColor()
{
	int count = 0;
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
			
			count++;
		}
	}
	downsampleCount = count;
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
	while (row != data.end()) { // Convert grayscale values
		col = row->begin();
		while (col != row->end()) {
			dataY[index] = col->Y;
			index++, col++;
		}
		row++;
	}
	index = 0;
	for (int x1 = 0; x1 < imageSize.GetWidth() - 1; x1 += 2) { 
		for (int y1 = 0; y1 < imageSize.GetHeight() - 1; y1 += 2) {
			dataCo[index] = data[y1][x1].Co;
			dataCg[index] = data[y1][x1].Cg;
			index++;
		}
	}
	
	dataCo.resize(index); 
	dataCg.resize(index); 
}

// Huffman-based encoding of pixel values
void myIMGFile::encode()
{

}

void myIMGFile::decode()
{
}

void myIMGFile::reconstructPixels()
{
	int index = 0;
	vector<colSpace> colWidth(imageSize.GetWidth());
	data = vector<vector<colSpace>>(imageSize.GetHeight(), colWidth);

	row = data.begin();
	// Write Y values to colSpace matrix
	while (row != data.end()) {
		col = row->begin();
		while (col != row->end()) {
			col->Y	= dataY[index];
			col++, index++;
		}
		row++;
	}
	
	index = 0;
	for (int x1 = 0; x1 < imageSize.GetWidth() - 1; x1 += 2) {
		int x2 = x1 + 1;
		for (int y1 = 0; y1 < imageSize.GetHeight() - 1; y1 += 2) {
			int y2 = y1 + 1;
			data[y1][x1].Co = dataCo[index];
			data[y2][x1].Co = dataCo[index];
			data[y2][x2].Co = dataCo[index];
			data[y1][x2].Co = dataCo[index];
							  
			data[y1][x1].Cg = dataCg[index];
			data[y2][x1].Cg = dataCg[index];
			data[y1][x2].Cg = dataCg[index];
			data[y2][x2].Cg = dataCg[index];
			index++;
		}
	}
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
	//downsample size
	intBuff = downsampleCount;
	file.Write(&intBuff, sizeof(int));

	//pixel data
	for (int i = 0; i < imageSize.GetWidth() * imageSize.GetHeight(); i++) {
		intBuff = dataY[i];
		file.Write(&intBuff, sizeof(int));
	}

	for (int i = 0; i < downsampleCount; i++) { 
		intBuff = dataCo[i];
		file.Write(&intBuff, sizeof(int));
	}

	for (int i = 0; i < downsampleCount; i++) { 
		intBuff = dataCg[i];
		file.Write(&intBuff, sizeof(int));
	}
	file.Close();
}

wxSize myIMGFile::getSize() const
{
	return imageSize;
}

const vector<vector<colSpace>>& myIMGFile::getPixelVector()
{
	return data;
}
