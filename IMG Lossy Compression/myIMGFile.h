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
	myIMGFile(const wxString filepath);
	myIMGFile(myBMPFile& bmp);
	void readFromFile(const wxString filepath);
	void writeToFile(const wxString filepath);
	wxSize getSize() const;
	const vector<vector<colSpace>>& getPixelVector();
	vector<vector<colSpace>>::iterator row;
	vector<colSpace>::iterator col;

private:
	void readBMP(myBMPFile& bmp);
	//compression functions
	void downsampleColor();
	void quantizePixels();
	void flattenExtremes();
	void convertToStrings();
	void encode();
	//read functions
	void decode();
	void reconstructPixels();

	wxSize imageSize;
	int downsampleCount;
	vector<vector<colSpace>> data;
	vector<int> dataY;
	vector<int> dataCo;
	vector<int> dataCg;
	/*
	vector<string> dataY;
	vector<string> dataCo;
	vector<string> dataCg;
	*/
	
};

