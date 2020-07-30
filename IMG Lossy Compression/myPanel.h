#pragma once
#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/panel.h"
#include "myBMPFile.h"

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
	void loadNext();
	
	myBMPFile* getFile();

private:
	void loadNormal();
	void loadGrayscale();
	void loadDarker();
	void loadVivid();
	void RGBtoHSL(wxColor rgb, double& H, double& S, double& L);
	void HSLtoRGB(wxColor& rgb, double H, double S, double L);
	wxColor getPixelColor(const int index) const;
	myBMPFile* bmpFile;
	vector<wxColor> image;
	wxSize maxSize;
	int	maxHeight;
	int maxWidth;
	const enum display{ NORMAL = 1, GRAYSCALE = 2, DARK = 3, VIVID = 4 };
	int displayStatus;

	DECLARE_EVENT_TABLE();
};

