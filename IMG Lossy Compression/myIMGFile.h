#pragma once
#include "wx/wx.h"
#include "wx/string.h"
#include "wx/wfstream.h"
#include "myBMPFile.h"
#include <vector>

// simple YCoCg struct
struct colSpace {
	colSpace();
	colSpace(wxColor triColor);
	int Y; 
	int Co; 
	int Cg; 
};

class myIMGFile
{
public:
	myIMGFile();
	myIMGFile(myBMPFile& bmp);
	void writeToFile(const wxString filepath);

private:
	void readBMP(myBMPFile& bmp);
	void downsampleColor();
	void quantizePixels();
	void flattenExtremes();
	void convertToStrings();
	void encode();
	wxSize imageSize;
	int downsampleCount;
	vector<vector<colSpace>> data;
	vector<string> dataY;
	vector<string> dataCo;
	vector<string> dataCg;
	vector<vector<colSpace>>::iterator row;
	vector<colSpace>::iterator col;
};

