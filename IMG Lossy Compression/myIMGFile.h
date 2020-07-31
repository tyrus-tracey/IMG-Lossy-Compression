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

private:
	void downsampleColor();

	wxSize imageSize;
	vector<vector<colSpace>> data;
	vector<vector<colSpace>>::iterator row;
	vector<colSpace>::iterator col;
};

