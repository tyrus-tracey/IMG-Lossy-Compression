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
	: imageSize(wxSize(0,0)), data(vector<vector<colSpace>>(0)), dataY(0), dataCo(0), dataCg(0)
{
}

myIMGFile::myIMGFile(const wxString filepath)
	: imageSize(wxSize(0, 0)), data(vector<vector<colSpace>>(0)), dataY(0), dataCo(0), dataCg(0)
{
	readFromFile(filepath);
	reconstructPixels();
}

// Copy pixel data to .IMG file, converting to YCoCg in the process
myIMGFile::myIMGFile(myBMPFile& bmp)
	: imageSize(bmp.getImageSize())
{
	dataY  = vector<int16_t>(bmp.getPixelCount());
	dataCo = vector<int16_t>(bmp.getPixelCount());
	dataCg = vector<int16_t>(bmp.getPixelCount());
	readBMP(bmp);
	//quantizePixels();
	flattenExtremes();
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
	int pixelCount = imageSize.GetHeight() * imageSize.GetWidth();
	dataY.resize(pixelCount);
	dataCo.resize(pixelCount);
	dataCg.resize(pixelCount);
	
	for (int i = 0; i < pixelCount; i++) {
		file.Read(&dataY[i], sizeof(int16_t));
		file.Read(&dataCo[i], sizeof(int16_t));
		file.Read(&dataCg[i], sizeof(int16_t));
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
			dataCo[index] = col->Co;
			dataCg[index] = col->Cg;
			index++, col++;
		}
		row++;
	}
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
			col->Co = dataCo[index];
			col->Cg = dataCg[index];
			col++, index++;
		}
		row++;
	}
	
}

void myIMGFile::writeToFile(const wxString filepath)
{
	wxFFile file(filepath, "wb");
	int intBuff;
	short shortBuff;
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

	//pixel data
	for (int i = 0; i < imageSize.GetWidth() * imageSize.GetHeight(); i++) {
		file.Write(&dataY[i], sizeof(int16_t));
		file.Write(&dataCo[i], sizeof(int16_t));
		file.Write(&dataCg[i], sizeof(int16_t));
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
