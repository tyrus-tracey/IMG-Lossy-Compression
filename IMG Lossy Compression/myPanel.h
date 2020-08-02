#pragma once
#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/panel.h"
#include "wx/filename.h"
#include "myBMPFile.h"
#include "myIMGFile.h"
/*
	Panels are windows within frames that allow for drawing operations.
*/

class myPanel : public wxPanel
{
public:
	myPanel(wxFrame* parent, const wxFileName filepath);
	~myPanel();
	void resizeToImage();
	void paintEvent(wxPaintEvent& event);
	void drawImage(wxDC& dc);
	void writeIMG(wxString filepath);

private:
	void loadBMP();
	void loadIMG();
	wxColor YCoCgtoRGB(colSpace yCoCg);
	wxColor getPixelColor(const int row, const int col) const;
	myBMPFile bmpFile;
	myIMGFile imgFile;

	vector<vector<wxColor>> image;
	wxSize maxSize;
	int	maxHeight;
	int maxWidth;

	DECLARE_EVENT_TABLE();
};

