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
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_AUTO 1000
#define wxID_OBLIVON 1001
#define wxID_SKYRIM 1002
#define wxID_NHDR 1003
#define wxID_NMTS 1004
#define wxID_NMSP 1005
#define wxID_CMGM 1006
#define wxID_BRD0 1007
#define wxID_BRD1 1008
#define wxID_BRD2 1009
#define wxID_BRD4 1010
#define wxID_BRD8 1011
#define wxID_DOCT 1012
#define wxID_DOOT 1013
#define wxID_SKIPP 1014
#define wxID_SKIPO 1015
#define wxID_PASST 1016
#define wxID_DOZ0 1017
#define wxID_DOZ1 1018
#define wxID_DOZ2 1019
#define wxID_DOZ3 1020
#define wxID_DOZ4 1021
#define wxID_DOZ5 1022
#define wxID_DOZ6 1023
#define wxID_DOZ7 1024
#define wxID_DOZ8 1025
#define wxID_DOZ9 1026
#define wxID_DOZ10Q 1027
#define wxID_DOZ10T 1028
#define wxID_FORCE 1029
#define wxID_SKIPE 1030
#define wxID_SKIPN 1031
#define wxID_SKIPH 1032
#define wxID_SKIPC 1033
#define wxID_SKIPB 1034
#define wxID_SKIPX 1035
#define wxID_LOGF 1036

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
		wxMenu* DOBorderIgnore;
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
		wxChoice* DOSaveFormat;
		wxChoice* DOSaveRes;
		wxButton* DOSaveAs;
		wxPanel* DOConstraints;
		wxPanel* m_panel8;
		wxStaticText* m_staticText13;
		wxCheckBox* DOCompressMSN;
		wxChoice* DOCompressMSNFormat;
		wxStaticText* m_staticText131;
		wxCheckBox* DOCompressN;
		wxChoice* DOCompressNFormat;
		wxStaticText* m_staticText1311;
		wxCheckBox* DOCompressNS;
		wxChoice* DOCompressNSFormat;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText132;
		wxCheckBox* DOCompressC;
		wxChoice* DOCompressCFormat;
		wxStaticText* m_staticText1322;
		wxCheckBox* DOCompressCA;
		wxChoice* DOCompressCAFormat;
		wxStaticText* m_staticText1321;
		wxCheckBox* DOCompressG;
		wxChoice* DOCompressGFormat;
		wxStaticText* m_staticText13211;
		wxCheckBox* DOCompressGA;
		wxChoice* DOCompressGAFormat;
		
		wxStaticText* m_staticText11;
		wxChoice* DOLimitResPlain;
		wxStaticText* m_staticText111;
		wxChoice* DOLimitResCompressed;
		
		wxStaticText* m_staticText112;
		wxChoice* DOLimitSzeLand;
		wxStaticText* m_staticText1111;
		wxChoice* DOLimitSzeChar;
		wxStaticText* m_staticText11111;
		wxChoice* DOLimitSzeOther;
		wxStaticText* m_staticText23;
		
		wxFilePickerCtrl* DOOutputC;
		wxTextCtrl* DOOutCText;
		wxButton* DOOutCBrowse;
		wxButton* DOProcessC;
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
		virtual void ChangeSkipOptimized( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangePassthrough( wxCommandEvent& event ) { event.Skip(); }
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
		virtual void TypedInDone( wxFocusEvent& event ) { event.Skip(); }
		virtual void TypedIn( wxCommandEvent& event ) { event.Skip(); }
		virtual void BrowseIn( wxCommandEvent& event ) { event.Skip(); }
		virtual void ProcessingStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void ActivateTreeItem( wxTreeEvent& event ) { event.Skip(); }
		virtual void ChangeTreeItem( wxTreeEvent& event ) { event.Skip(); }
		virtual void ChangeFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void ApplyFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void ResetFileList( wxCommandEvent& event ) { event.Skip(); }
		virtual void ClearFileList( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeRecursion( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSelectedPreview( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeSelectedFiles( wxCommandEvent& event ) { event.Skip(); }
		virtual void TypedOutDone( wxFocusEvent& event ) { event.Skip(); }
		virtual void TypedOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void BrowseOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangePreview( wxCommandEvent& event ) { event.Skip(); }
		virtual void PaintH( wxPaintEvent& event ) { event.Skip(); }
		virtual void SaveAsStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressMSN( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressMSNFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressN( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressNFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressNS( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressNSFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressC( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressCFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressCA( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressCAFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressG( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressGFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressGA( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeCompressGAFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLimitResPlain( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLimitResCompressed( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLimitSzeLand( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLimitSzeChar( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeLimitSzeOther( wxCommandEvent& event ) { event.Skip(); }
		virtual void TypedOutCDone( wxFocusEvent& event ) { event.Skip(); }
		virtual void TypedOutC( wxCommandEvent& event ) { event.Skip(); }
		virtual void BrowseOutC( wxCommandEvent& event ) { event.Skip(); }
		
	
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
