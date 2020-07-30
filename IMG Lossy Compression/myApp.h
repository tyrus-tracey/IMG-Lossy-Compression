#pragma once
#include "wx/wx.h"
#include "myFrame.h"

/*h
	Main application class used in wxWidgets program. Handles program start and close.

	Note: As per wxWidgets documentation all basic wxClasses do not require deletion when allocated
			to memory, so do not worry about memory leaks here. This is handled by wxWidgets
			to ensure that elements are deleted after they are set to stop displaying,
			and not before.
*/
class myApp : public wxApp
{
public:
	myApp();
	~myApp();
	virtual bool OnInit();
	wxSize getDimensions();

private:
	myFrame* frame = NULL;
	wxSize dimensions = wxSize(400, 400);

};



