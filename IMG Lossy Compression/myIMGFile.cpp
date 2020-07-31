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
	vector<colSpace> width(imageSize.GetX());
	data = vector<vector<colSpace>>(imageSize.GetY(), width);

	row		= data.begin();
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

	downsampleColor();
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

