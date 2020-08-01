#include "myFrame.h"
#include "wx/filename.h"
#include <intrin.h>

wxBEGIN_EVENT_TABLE(myFrame, wxFrame)
END_EVENT_TABLE()

// Using base class constructor, create a frame of a specified size in the app.
myFrame::myFrame(wxSize& appDimensions)
	: wxFrame(NULL, wxID_ANY, "BMP Image Program", wxPoint(0, 0), appDimensions)
{
	createMenuBar();
	Show(true); // Display this frame
}

myFrame::~myFrame() {
}

// Create the menu bar and bind some events to menu options
void myFrame::createMenuBar() {
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Open, "&Open a BMP File");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, &myFrame::OnOpen, this, ID_Open);
	Bind(wxEVT_MENU, &myFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &myFrame::OnExit, this, wxID_EXIT);
}

myPanel* myFrame::getPanel()
{
	return panel;
}

void myFrame::OnAbout(wxCommandEvent& event) {
	wxMessageBox("Please select a BMP File from the top right menu. Then, cycle between pages with any key.",
		"How to Use", wxOK | wxICON_INFORMATION);
}

void myFrame::OnExit(wxCommandEvent& event) {
	Close(true); // Stop displaying
}

// If user selects a BMP file, a new panel will be created using that file.
void myFrame::OnOpen(wxCommandEvent& event)
{

	delete panel; // delete if panel exists
	wxFileDialog openDialog(this, ("Open a BMP file"), "", "", "BMP files (*.bmp)|*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openDialog.ShowModal() == wxID_CANCEL) { // If user cancels the dialog
		wxMessageBox("No file was chosen.");
		return;
	}
	// Create a new panel with given BMP file with the frame as the parent
	wxFileName file = openDialog.GetPath();
	panel = new myPanel(this, file.GetFullPath()); 

	SetClientSize(panel->GetSize());
	panel->writeIMG(file.GetPathWithSep() + wxString("TEST.IMG")); //ensure file is open?

	Refresh(); //Redraw the frame
	Update(); //Force painting of BMP immediately
}
