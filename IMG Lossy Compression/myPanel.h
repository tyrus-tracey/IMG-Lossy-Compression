#pragma once
#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/panel.h"
#include "myBMPFile.h"
#include "myIMGFile.h"
/*
	Panels are windows within frames that allow for drawing operations.
*/

class myPanel : public wxPanel
{
public:
	myPanel(wxFrame* parent, const wxString filepath);
	~myPanel();
	void resizeToImage();
	void paintEvent(wxPaintEvent& event);
	void drawImage(wxDC& dc);
	
	myBMPFile* getFile();

private:
	void loadNormal();
	void RGBtoHSL(wxColor rgb, double& H, double& S, double& L);
	void HSLtoRGB(wxColor& rgb, double H, double S, double L);
	wxColor getPixelColor(const int row, const int col) const;
	myBMPFile* bmpFile;
	myIMGFile* imgFile;

	vector<vector<wxColor>> image;
	wxSize maxSize;
	int	maxHeight;
	int maxWidth;

	DECLARE_EVENT_TABLE();
};

