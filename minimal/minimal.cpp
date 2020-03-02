/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: minimal.cpp 70789 2012-03-04 00:28:58Z VZ $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------
#include <wx/thread.h>

#include <string>
#include <fstream>
#include <sstream>
#include "wx/grid.h"

#include "forGUI.h"

std::string codeLoadedFile = "";
std::string astLoadedFile = "";
HANDLE hSetGuiLine;
HANDLE hSetValue;

// a thread class that will periodically send events to the GUI thread
class MyThreadEvent : public wxThread {
protected:
    wxEvtHandler* m_parent;
public:
    MyThreadEvent(wxEvtHandler* parent) : wxThread(), m_parent(parent) {    };
    ExitCode Entry();
};
class MyThreadTest : public wxThread {
protected:
    wxEvtHandler* m_parent;
public:
    MyThreadTest(wxEvtHandler* parent) : wxThread(), m_parent(parent) {    };
    ExitCode Entry();
};


wxThread::ExitCode MyThreadEvent::Entry()
{
    for (unsigned n = 0; n < 100; n++)
    {
        // notify the main thread
        WaitForSingleObject(hSetGuiLine, INFINITE);
        wxThreadEvent* evt = new wxThreadEvent(wxEVT_THREAD);
        evt->SetInt(n);
        m_parent->QueueEvent(evt);
        //this->Sleep(1000);
    }
    return NULL;
}

wxThread::ExitCode MyThreadTest::Entry()
{
    //testCompare("ex01");	getchar();
    //testCompare("ex02");	getchar();
    //testCompare("ex03");	getchar();
    //testCompare("ex04");	getchar();
    //testCompare("ex05");	getchar();
    //testCompare("ex06");	getchar();
    //testCompare("ex07");	getchar();
    //testCompare("ex08");	getchar();
    //testCompare("ex09");	getchar();
    //testCompare("ex10");	getchar();
    //testCompare("ex11");	getchar();
    //testCompare("ex12");	getchar();
    //testCompare("ex13");	getchar();
    //testCompare("ex14");	getchar();
    //testCompare("ex15");	getchar();
    //testCompare("ex16");	getchar();
    //testCompare("ex17");	getchar();
    //testCompare("ex18");	getchar();
    //testCompare("ex19");	getchar();
    //testCompare("ex20");	getchar();
    //testCompare("ex21");	getchar();
    //testCompare("ex22");	getchar();
    //testCompare("ex23");	getchar();
    //testCompare("ex24");	getchar();
    testCompare("ex25");	getchar();
    //testCompare("final");	getchar();

    return NULL;
}



// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnThreadEvent(wxThreadEvent& event);
    void OnClick(wxCommandEvent& event);
    void OnKeyMy(wxCommandEvent& event);

private:
    wxStaticText* m_st;
    wxTextCtrl* CodeBox;
    wxGrid* AstBox;
    wxTextCtrl* VarValue;
    wxButton* ValButton;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,
    BUTTON_Event = wxID_HIGHEST + 1,

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
EVT_MENU(Minimal_About, MyFrame::OnAbout)
EVT_THREAD(wxID_ANY, MyFrame::OnThreadEvent)
EVT_BUTTON(BUTTON_Event, MyFrame::OnClick)
EVT_TEXT_ENTER(wxID_ANY, MyFrame::OnKeyMy)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if (!wxApp::OnInit())
        return false;

    // create the main application window
    MyFrame* frame = new MyFrame("Unit Test Generator Support");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    hSetGuiLine = CreateEvent(NULL, false, false, TEXT("SetGuiLine"));
    if (!hSetGuiLine) {
        std::cout << "Can't create Global\\SetGuiLine\n";
        return -1;
    }
    hSetValue = CreateEvent(NULL, false, false, TEXT("SetValue"));
    if (!hSetValue) {
        std::cout << "Can't create Global\\SetValue\n";
        return -1;
    }

    MyThreadEvent* threadEvent = new MyThreadEvent(frame);
    MyThreadTest* threadTest = new MyThreadTest(frame);
    //MyThread* thread = new MyThread(this);
    if (threadEvent->Create() == wxTHREAD_NO_ERROR)
    {
        threadEvent->Run();
    }
    if (threadTest->Create() == wxTHREAD_NO_ERROR)
    {
        threadTest->Run();
    }

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));
    // set size
    SetSize(wxSize(1240, 800));
#if wxUSE_MENUS
    // create a menu bar
    wxMenu* fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    m_st = new wxStaticText(panel, wxID_ANY, "stringa di prova", wxPoint(10, 0), wxSize(40, 18));
    m_st->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    wxWindowID TEXT_Main = 1234;
    wxWindowID TEXT_Value = 1235;
    wxWindowID BUTTON_Val = 1236;
    long start, end;
    wxTextAttr attr;
    attr.SetTextColour(*wxRED);
    attr.SetBackgroundColour(*wxYELLOW);

    VarValue = new wxTextCtrl(panel, TEXT_Value, "Enter value", wxPoint(10, 20), wxSize(60, 28),
        wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);
    VarValue->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    ValButton = new wxButton(panel, BUTTON_Event, _T("Accept"), wxPoint(80, 20), wxSize(50, 28), 0);

    CodeBox = new wxTextCtrl(panel, TEXT_Main, "Loading module under test", wxPoint(10, 50), wxSize(1200, 400),
        wxTE_MULTILINE | wxTE_RICH2 | wxTE_DONTWRAP, wxDefaultValidator, wxTextCtrlNameStr);
    CodeBox->SetStyle(0, 10, attr);
    CodeBox->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    AstBox = new wxGrid(panel, wxID_ANY, wxPoint(10, 460), wxSize(1200, 400));
    AstBox->CreateGrid(0, 0);
    AstBox->AppendCols(2);
    AstBox->AppendRows(40000);
    AstBox->SetColSize(1, 1000);

}


// event handlers
void MyFrame::OnThreadEvent(wxThreadEvent& event)
{
    wxLogDebug("thread event: %d", event.GetInt());
    static int counter = 1;
    int position, positionHighlightB;
    wxTextAttr attr, attrReset;
    int guiLineT;
    attr.SetTextColour(*wxRED);
    attr.SetBackgroundColour(*wxYELLOW);
    attrReset.SetTextColour(*wxBLACK);
    attrReset.SetBackgroundColour(*wxWHITE);

    m_st->SetLabel(forGui.varName + " " + std::to_string(forGui.line) + " " + std::to_string(forGui.col));
    if (codeLoadedFile.compare(forGui.codeFileName) != 0) {
        CodeBox->LoadFile(forGui.codeFileName);
        codeLoadedFile = forGui.codeFileName;
    }
    if (astLoadedFile.compare(forGui.astFileName) != 0) {
        std::ifstream isAst(forGui.astFileName);
        std::string strAst;
        int ix = 0;
        while (std::getline(isAst, strAst)) {
            if (ix < 40000)
                AstBox->SetCellValue(wxGridCellCoords(ix++, 1), strAst);
        }
    }
    /* Always load values (all the file for now) */
    std::ifstream isValues(forGui.valueFileName);
    std::string strVal;
    int ix = 0;
    while (std::getline(isValues, strVal)) {
        if (ix < 40000)
            AstBox->SetCellValue(wxGridCellCoords(ix++, 0), strVal);
    }
    guiLineT = forGui.line - 3;
    if (guiLineT < 0) guiLineT = 0;
    // Execute the next line 2 times because 
    // the first time it fails ?? !!!
    position = CodeBox->XYToPosition(0, guiLineT);
    position = CodeBox->XYToPosition(0, guiLineT);
    CodeBox->ShowPosition(position);

    if (forGui.strComm.compare("resetHL") == 0) {
        CodeBox->SetStyle(1, CodeBox->GetLastPosition()-1, attrReset);
    }
    else {
        positionHighlightB = CodeBox->XYToPosition(forGui.col, forGui.line);
        CodeBox->SetStyle(positionHighlightB - 1, positionHighlightB + forGui.len, attr);
    }
    AstBox->MakeCellVisible(forGui.astLine+100, 0);
    AstBox->MakeCellVisible(forGui.astLine-5, 0);
    AstBox->SelectRow(forGui.astLine);

}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
}

void  MyFrame::OnClick(wxCommandEvent& WXUNUSED(event)) {
    std::string temp = VarValue->GetValue().mb_str();
    forGui.ValueFromGui = std::stoul(temp);
    SetEvent(hSetValue);
}

void MyFrame::OnKeyMy(wxCommandEvent& event)
{
    std::string temp = VarValue->GetValue().mb_str();
    VarValue->SetValue("");
    forGui.ValueFromGui = std::stoll(temp);
    SetEvent(hSetValue);
}