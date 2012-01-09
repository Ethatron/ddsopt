///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DDSopt_Window__
#define __DDSopt_Window__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/toolbar.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/splitter.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_AUTO 1000
#define wxID_OBLIVON 1001
#define wxID_SKYRIM 1002
#define wxID_NHDR 1003
#define wxID_NMTS 1004
#define wxID_NMSP 1005
#define wxID_CMGM 1006
#define wxID_DOCT 1007
#define wxID_DOOT 1008
#define wxID_SKIPP 1009
#define wxID_DOZ0 1010
#define wxID_DOZ1 1011
#define wxID_DOZ2 1012
#define wxID_DOZ3 1013
#define wxID_DOZ4 1014
#define wxID_DOZ5 1015
#define wxID_DOZ6 1016
#define wxID_DOZ7 1017
#define wxID_DOZ8 1018
#define wxID_DOZ9 1019
#define wxID_DOZ10Q 1020
#define wxID_DOZ10T 1021
#define wxID_FORCE 1022
#define wxID_SKIPE 1023
#define wxID_SKIPN 1024
#define wxID_SKIPH 1025
#define wxID_SKIPC 1026
#define wxID_SKIPB 1027
#define wxID_SKIPX 1028
#define wxID_LOGF 1029

///////////////////////////////////////////////////////////////////////////////
/// Class wxDDSopt
///////////////////////////////////////////////////////////////////////////////
class wxDDSopt : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* DOMenuBar;
		wxMenu* DOGame;
		wxMenu* DOBehaviour;
		wxMenu* DOBTextures;
		wxMenu* DOIgnore;
		wxMenu* DOCompression;
		wxMenu* DOSettings;
		wxNotebook* DOToolSwitch;
		wxPanel* DOBrowser;
		wxSplitterWindow* m_splitter1;
		wxPanel* DOPanelContents;
		wxDirPickerCtrl* DOInput;
		wxTextCtrl* DOInText;
		wxButton* DOInBrowse;
		wxButton* DOParse;
		wxTreeCtrl* DOArchiveTree;
		wxPanel* m_panel4;
		wxComboBox* DOFilter;
		wxButton* DOFilterApply;
		wxToolBar* DOPluginToolbar;
		wxCheckBox* DORecursive;
		wxCheckListBox* DOArchiveList;
		wxFilePickerCtrl* DOOutput;
		wxTextCtrl* DOOutText;
		wxButton* DOOutBrowse;
		wxButton* DOProcess;
		wxPanel* DOInfo;
		wxStaticText* DOTextureFirst;
		wxStaticText* DOTexturePath;
		wxChoice* DOPreviewSelector;
		wxPanel* DOPreview;
		wxPropertyGrid* DOTextureInfos;
		wxStatusBar* DOStatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void ChangeToOblivion( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeToSkyrim( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLeaveHDR( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeNormalTS( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeNormalSteep( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeColorGamma( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeDoCompressImages( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeDoOptimizeImages( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSkipProcessing( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeForceCompression( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSkipExisting( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSkipNewer( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSkipHidden( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSkipHash( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSkipBroken( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeUnselectExtras( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLogFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeTool( wxNotebookEvent& event ) { event.Skip(); }
		virtual void ChangePluginDir( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void TypedIn( wxCommandEvent& event ) { event.Skip(); }
		virtual void BrowseIn( wxCommandEvent& event ) { event.Skip(); }
		virtual void ProcessingStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeTreeItem( wxTreeEvent& event ) { event.Skip(); }
		virtual void ChangeFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void ApplyFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void ResetFileList( wxCommandEvent& event ) { event.Skip(); }
		virtual void ClearFileList( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeRecursion( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSelectedPreview( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSelectedFiles( wxCommandEvent& event ) { event.Skip(); }
		virtual void TypedOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void BrowseOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangePreview( wxCommandEvent& event ) { event.Skip(); }
		virtual void PaintH( wxPaintEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxDDSopt( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 525,774 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~wxDDSopt();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxDDSopt::m_splitter1OnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class wxProgress
///////////////////////////////////////////////////////////////////////////////
class wxProgress : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel9;
		wxStaticText* DOSubject;
		wxGauge* DOTask;
		wxStaticText* DOReport;
		wxGauge* DOEfficiency;
		wxStaticText* m_staticText25;
		wxStaticText* DORunning;
		wxStaticText* m_staticText27;
		wxStaticText* DORemaining;
		wxStaticText* m_staticText29;
		wxStaticText* DOTotal;
		wxStaticLine* m_staticline7;
		wxButton* DOPause;
		wxButton* DOAbort;
		
		// Virtual event handlers, overide them in your derived class
		virtual void AbortProgress( wxCloseEvent& event ) { event.Skip(); }
		virtual void IdleProgress( wxIdleEvent& event ) { event.Skip(); }
		virtual void PauseProgress( wxCommandEvent& event ) { event.Skip(); }
		virtual void AbortProgress( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxProgress( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DDSopt processing in progress ..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 569,265 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~wxProgress();
	
};

#endif //__DDSopt_Window__
