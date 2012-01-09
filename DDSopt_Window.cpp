///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DDSopt_Window.h"

///////////////////////////////////////////////////////////////////////////

wxDDSopt::wxDDSopt( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	DOMenuBar = new wxMenuBar( 0 );
	DOGame = new wxMenu();
	wxMenuItem* DOAuto;
	DOAuto = new wxMenuItem( DOGame, wxID_AUTO, wxString( wxT("Automatic") ) , wxEmptyString, wxITEM_RADIO );
	DOGame->Append( DOAuto );
	DOAuto->Check( true );
	
	wxMenuItem* DOOblivion;
	DOOblivion = new wxMenuItem( DOGame, wxID_OBLIVON, wxString( wxT("Oblivion") ) , wxEmptyString, wxITEM_RADIO );
	DOGame->Append( DOOblivion );
	
	wxMenuItem* DOSkyrim;
	DOSkyrim = new wxMenuItem( DOGame, wxID_SKYRIM, wxString( wxT("Skyrim") ) , wxEmptyString, wxITEM_RADIO );
	DOGame->Append( DOSkyrim );
	
	DOMenuBar->Append( DOGame, wxT("Game") ); 
	
	DOBehaviour = new wxMenu();
	DOBTextures = new wxMenu();
	wxMenuItem* DOLeaveHDRs;
	DOLeaveHDRs = new wxMenuItem( DOBTextures, wxID_NHDR, wxString( wxT("Don't touch HDRs") ) , wxEmptyString, wxITEM_CHECK );
	DOBTextures->Append( DOLeaveHDRs );
	DOLeaveHDRs->Check( true );
	
	wxMenuItem* DONormalMapTS;
	DONormalMapTS = new wxMenuItem( DOBTextures, wxID_NMTS, wxString( wxT("Produce partial derivative tangent-space normal-maps") ) , wxEmptyString, wxITEM_CHECK );
	DOBTextures->Append( DONormalMapTS );
	DONormalMapTS->Check( true );
	
	wxMenuItem* DONormapMapSteep;
	DONormapMapSteep = new wxMenuItem( DOBTextures, wxID_NMSP, wxString( wxT("Raise normal-map steepness each mip-level") ) , wxEmptyString, wxITEM_CHECK );
	DOBTextures->Append( DONormapMapSteep );
	DONormapMapSteep->Check( true );
	
	wxMenuItem* DOColorMapGamma;
	DOColorMapGamma = new wxMenuItem( DOBTextures, wxID_CMGM, wxString( wxT("Gamma-correct mip-map filtering color-maps") ) , wxEmptyString, wxITEM_CHECK );
	DOBTextures->Append( DOColorMapGamma );
	DOColorMapGamma->Check( true );
	
	wxMenuItem* m_separator7;
	m_separator7 = DOBTextures->AppendSeparator();
	
	wxMenuItem* DOCompressTextures;
	DOCompressTextures = new wxMenuItem( DOBTextures, wxID_DOCT, wxString( wxT("Compress all kown texture-formats") ) , wxEmptyString, wxITEM_CHECK );
	DOBTextures->Append( DOCompressTextures );
	DOCompressTextures->Check( true );
	
	wxMenuItem* DOOptimizeTextures;
	DOOptimizeTextures = new wxMenuItem( DOBTextures, wxID_DOOT, wxString( wxT("Re-compress and re-mip all textures") ) , wxEmptyString, wxITEM_CHECK );
	DOBTextures->Append( DOOptimizeTextures );
	DOOptimizeTextures->Check( true );
	
	DOBehaviour->Append( -1, wxT("Textures"), DOBTextures );
	
	DOMenuBar->Append( DOBehaviour, wxT("Behave") ); 
	
	DOIgnore = new wxMenu();
	wxMenuItem* DOSkipProcessing;
	DOSkipProcessing = new wxMenuItem( DOIgnore, wxID_SKIPP, wxString( wxT("Any known file-type") ) , wxEmptyString, wxITEM_CHECK );
	DOIgnore->Append( DOSkipProcessing );
	
	DOMenuBar->Append( DOIgnore, wxT("Ignore") ); 
	
	DOCompression = new wxMenu();
	wxMenuItem* DOZ0;
	DOZ0 = new wxMenuItem( DOCompression, wxID_DOZ0, wxString( wxT("0 (off)") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ0 );
	
	wxMenuItem* DOZ1;
	DOZ1 = new wxMenuItem( DOCompression, wxID_DOZ1, wxString( wxT("1") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ1 );
	
	wxMenuItem* DOZ2;
	DOZ2 = new wxMenuItem( DOCompression, wxID_DOZ2, wxString( wxT("2") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ2 );
	
	wxMenuItem* DOZ3;
	DOZ3 = new wxMenuItem( DOCompression, wxID_DOZ3, wxString( wxT("3") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ3 );
	
	wxMenuItem* DOZ4;
	DOZ4 = new wxMenuItem( DOCompression, wxID_DOZ4, wxString( wxT("4") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ4 );
	
	wxMenuItem* DOZ5;
	DOZ5 = new wxMenuItem( DOCompression, wxID_DOZ5, wxString( wxT("5") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ5 );
	
	wxMenuItem* DOZ6;
	DOZ6 = new wxMenuItem( DOCompression, wxID_DOZ6, wxString( wxT("6") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ6 );
	
	wxMenuItem* DOZ7;
	DOZ7 = new wxMenuItem( DOCompression, wxID_DOZ7, wxString( wxT("7") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ7 );
	
	wxMenuItem* DOZ8;
	DOZ8 = new wxMenuItem( DOCompression, wxID_DOZ8, wxString( wxT("8") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ8 );
	
	wxMenuItem* DOZ9;
	DOZ9 = new wxMenuItem( DOCompression, wxID_DOZ9, wxString( wxT("9") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ9 );
	
	wxMenuItem* DOZ10Quick;
	DOZ10Quick = new wxMenuItem( DOCompression, wxID_DOZ10Q, wxString( wxT("10 (quick)") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ10Quick );
	
	wxMenuItem* DOZ10Thorough;
	DOZ10Thorough = new wxMenuItem( DOCompression, wxID_DOZ10T, wxString( wxT("10 (maximum)") ) , wxEmptyString, wxITEM_RADIO );
	DOCompression->Append( DOZ10Thorough );
	DOZ10Thorough->Check( true );
	
	wxMenuItem* m_separator1;
	m_separator1 = DOCompression->AppendSeparator();
	
	wxMenuItem* DOForce;
	DOForce = new wxMenuItem( DOCompression, wxID_FORCE, wxString( wxT("Forced") ) , wxEmptyString, wxITEM_CHECK );
	DOCompression->Append( DOForce );
	
	DOMenuBar->Append( DOCompression, wxT("Compression") ); 
	
	DOSettings = new wxMenu();
	wxMenuItem* DOSkipExisting;
	DOSkipExisting = new wxMenuItem( DOSettings, wxID_SKIPE, wxString( wxT("Skip existing files") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOSkipExisting );
	DOSkipExisting->Check( true );
	
	wxMenuItem* DOSkipNewer;
	DOSkipNewer = new wxMenuItem( DOSettings, wxID_SKIPN, wxString( wxT("Skip older files") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOSkipNewer );
	DOSkipNewer->Check( true );
	
	wxMenuItem* DOSkipHidden;
	DOSkipHidden = new wxMenuItem( DOSettings, wxID_SKIPH, wxString( wxT("Skip hidden files") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOSkipHidden );
	DOSkipHidden->Check( true );
	
	wxMenuItem* m_separator2;
	m_separator2 = DOSettings->AppendSeparator();
	
	wxMenuItem* DOSkipHash;
	DOSkipHash = new wxMenuItem( DOSettings, wxID_SKIPC, wxString( wxT("Skip hash-check") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOSkipHash );
	
	wxMenuItem* DOSkipBroken;
	DOSkipBroken = new wxMenuItem( DOSettings, wxID_SKIPB, wxString( wxT("Skip broken files") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOSkipBroken );
	
	wxMenuItem* m_separator3;
	m_separator3 = DOSettings->AppendSeparator();
	
	wxMenuItem* DOUnselectExtras;
	DOUnselectExtras = new wxMenuItem( DOSettings, wxID_SKIPX, wxString( wxT("Unselect inappropriate") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOUnselectExtras );
	DOUnselectExtras->Check( true );
	
	wxMenuItem* m_separator31;
	m_separator31 = DOSettings->AppendSeparator();
	
	wxMenuItem* DOLogfile;
	DOLogfile = new wxMenuItem( DOSettings, wxID_LOGF, wxString( wxT("Write logfile") ) , wxEmptyString, wxITEM_CHECK );
	DOSettings->Append( DOLogfile );
	DOLogfile->Check( true );
	
	DOMenuBar->Append( DOSettings, wxT("Settings") ); 
	
	this->SetMenuBar( DOMenuBar );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	DOToolSwitch = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	DOBrowser = new wxPanel( DOToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( DOBrowser, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_splitter1->SetSashGravity( 0 );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( wxDDSopt::m_splitter1OnIdle ), NULL, this );
	
	DOPanelContents = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );
	
	DOInput = new wxDirPickerCtrl( DOPanelContents, wxID_ANY, wxT("./in"), wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	DOInput->Enable( false );
	DOInput->Hide();
	
	bSizer131->Add( DOInput, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
	DOInText = new wxTextCtrl( DOPanelContents, wxID_ANY, wxT(".\\in"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( DOInText, 1, wxALL, 5 );
	
	DOInBrowse = new wxButton( DOPanelContents, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( DOInBrowse, 0, wxALL, 5 );
	
	bSizer131->Add( bSizer32, 1, wxEXPAND, 5 );
	
	DOParse = new wxButton( DOPanelContents, wxID_ANY, wxT("Parse"), wxDefaultPosition, wxDefaultSize, 0 );
	DOParse->Enable( false );
	DOParse->Hide();
	DOParse->SetToolTip( wxT("Copy/Convert all files from the source into the destination") );
	
	bSizer131->Add( DOParse, 0, wxALL, 5 );
	
	bSizer36->Add( bSizer131, 0, wxEXPAND, 5 );
	
	DOArchiveTree = new wxTreeCtrl( DOPanelContents, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE );
	DOArchiveTree->SetToolTip( wxT("The directories present under the given source") );
	
	bSizer36->Add( DOArchiveTree, 1, wxALL|wxEXPAND, 5 );
	
	DOPanelContents->SetSizer( bSizer36 );
	DOPanelContents->Layout();
	bSizer36->Fit( DOPanelContents );
	m_panel4 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	DOFilter = new wxComboBox( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	DOFilter->Append( wxEmptyString );
	DOFilter->Append( wxT("*.*") );
	DOFilter->Append( wxT("*.dds") );
	DOFilter->Append( wxT("*_e.dds") );
	DOFilter->Append( wxT("*_em.dds") );
	DOFilter->Append( wxT("*_envmap.dds") );
	DOFilter->Append( wxT("*_m.dds") );
	DOFilter->Append( wxT("*_s.dds") );
	DOFilter->Append( wxT("*_sk.dds") );
	DOFilter->Append( wxT("*_n.dds") );
	DOFilter->Append( wxT("*_fn.dds") );
	DOFilter->Append( wxT("*_mns.dds") );
	DOFilter->Append( wxT("*_h.dds") );
	DOFilter->Append( wxT("*_hl.dds") );
	DOFilter->Append( wxT("*_hh.dds") );
	DOFilter->Append( wxT("*_g.dds") );
	DOFilter->Append( wxT("*_glow.dds") );
	DOFilter->Append( wxT("*_emit.dds") );
	DOFilter->Append( wxT("*_b.dds") );
	DOFilter->Append( wxT("*_bl.dds") );
	DOFilter->Append( wxT("*_p.dds") );
	bSizer10->Add( DOFilter, 1, wxALL|wxEXPAND, 5 );
	
	DOFilterApply = new wxButton( m_panel4, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( DOFilterApply, 0, wxALL, 5 );
	
	bSizer13->Add( bSizer10, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	DOPluginToolbar = new wxToolBar( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	DOPluginToolbar->AddTool( wxID_RESET, wxT("Reset"), wxBitmap( wxT("#112"), wxBITMAP_TYPE_RESOURCE ), wxNullBitmap, wxITEM_NORMAL, wxT("Selects all visible files"), wxEmptyString, NULL ); 
	DOPluginToolbar->AddTool( wxID_CLEAR, wxT("tool"), wxBitmap( wxT("#110"), wxBITMAP_TYPE_RESOURCE ), wxNullBitmap, wxITEM_NORMAL, wxT("Clears all visible files from the selection"), wxEmptyString, NULL ); 
	DOPluginToolbar->Realize();
	
	bSizer39->Add( DOPluginToolbar, 0, wxEXPAND, 5 );
	
	DORecursive = new wxCheckBox( m_panel4, wxID_ANY, wxT("Show recursive"), wxDefaultPosition, wxDefaultSize, 0 );
	DORecursive->SetToolTip( wxT("Show files in sub-directories as well") );
	
	bSizer39->Add( DORecursive, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer13->Add( bSizer39, 0, wxEXPAND, 5 );
	
	wxString DOArchiveListChoices[] = { wxT("a.esp"), wxT("b.esp") };
	int DOArchiveListNChoices = sizeof( DOArchiveListChoices ) / sizeof( wxString );
	DOArchiveList = new wxCheckListBox( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, DOArchiveListNChoices, DOArchiveListChoices, 0 );
	DOArchiveList->SetToolTip( wxT("The files in the currently selected directory") );
	
	bSizer13->Add( DOArchiveList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
	DOOutput = new wxFilePickerCtrl( m_panel4, wxID_ANY, wxT("./out"), wxT("Select a file"), wxT("*.land;*.raw"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	DOOutput->Enable( false );
	DOOutput->Hide();
	
	bSizer42->Add( DOOutput, 1, wxALL, 5 );
	
	wxBoxSizer* bSizer321;
	bSizer321 = new wxBoxSizer( wxHORIZONTAL );
	
	DOOutText = new wxTextCtrl( m_panel4, wxID_ANY, wxT(".\\out"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer321->Add( DOOutText, 1, wxALL, 5 );
	
	DOOutBrowse = new wxButton( m_panel4, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer321->Add( DOOutBrowse, 0, wxALL, 5 );
	
	bSizer42->Add( bSizer321, 1, wxEXPAND, 5 );
	
	DOProcess = new wxButton( m_panel4, wxID_ANY, wxT("Process"), wxDefaultPosition, wxDefaultSize, 0 );
	DOProcess->Enable( false );
	DOProcess->SetToolTip( wxT("Copy/Convert all files from the source into the destination") );
	
	bSizer42->Add( DOProcess, 0, wxALL, 5 );
	
	bSizer13->Add( bSizer42, 0, wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer13 );
	m_panel4->Layout();
	bSizer13->Fit( m_panel4 );
	m_splitter1->SplitHorizontally( DOPanelContents, m_panel4, 0 );
	bSizer14->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	DOBrowser->SetSizer( bSizer14 );
	DOBrowser->Layout();
	bSizer14->Fit( DOBrowser );
	DOToolSwitch->AddPage( DOBrowser, wxT("Browser"), false );
	DOInfo = new wxPanel( DOToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	DOTextureFirst = new wxStaticText( DOInfo, wxID_ANY, wxT("Select a texture first\n(double-click in the tree)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	DOTextureFirst->Wrap( -1 );
	bSizer23->Add( DOTextureFirst, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 25 );
	
	DOTexturePath = new wxStaticText( DOInfo, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	DOTexturePath->Wrap( -1 );
	DOTexturePath->Hide();
	
	bSizer23->Add( DOTexturePath, 0, wxALL, 5 );
	
	wxArrayString DOPreviewSelectorChoices;
	DOPreviewSelector = new wxChoice( DOInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, DOPreviewSelectorChoices, 0 );
	DOPreviewSelector->SetSelection( 0 );
	DOPreviewSelector->Hide();
	
	bSizer23->Add( DOPreviewSelector, 0, wxALL|wxEXPAND, 5 );
	
	DOPreview = new wxPanel( DOInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
	DOPreview->Hide();
	
	bSizer23->Add( DOPreview, 1, wxEXPAND | wxALL, 5 );
	
	DOTextureInfos = new wxPropertyGrid(DOInfo, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxPG_DEFAULT_STYLE);
	DOTextureInfos->SetMinSize( wxSize( -1,200 ) );
	
	bSizer23->Add( DOTextureInfos, 0, wxALL|wxEXPAND, 5 );
	
	DOInfo->SetSizer( bSizer23 );
	DOInfo->Layout();
	bSizer23->Fit( DOInfo );
	DOToolSwitch->AddPage( DOInfo, wxT("Preview"), false );
	
	bSizer1->Add( DOToolSwitch, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	DOStatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( DOAuto->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeToOblivion ) );
	this->Connect( DOOblivion->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeToOblivion ) );
	this->Connect( DOSkyrim->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeToSkyrim ) );
	this->Connect( DOLeaveHDRs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeLeaveHDR ) );
	this->Connect( DONormalMapTS->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeNormalTS ) );
	this->Connect( DONormapMapSteep->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeNormalSteep ) );
	this->Connect( DOColorMapGamma->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeColorGamma ) );
	this->Connect( DOCompressTextures->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeDoCompressImages ) );
	this->Connect( DOOptimizeTextures->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeDoOptimizeImages ) );
	this->Connect( DOSkipProcessing->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipProcessing ) );
	this->Connect( DOForce->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeForceCompression ) );
	this->Connect( DOSkipExisting->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipExisting ) );
	this->Connect( DOSkipNewer->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipNewer ) );
	this->Connect( DOSkipHidden->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipHidden ) );
	this->Connect( DOSkipHash->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipHash ) );
	this->Connect( DOSkipBroken->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipBroken ) );
	this->Connect( DOUnselectExtras->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeUnselectExtras ) );
	this->Connect( DOLogfile->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeLogFile ) );
	DOToolSwitch->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( wxDDSopt::ChangeTool ), NULL, this );
	DOInput->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxDDSopt::ChangePluginDir ), NULL, this );
	DOInText->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxDDSopt::TypedIn ), NULL, this );
	DOInBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::BrowseIn ), NULL, this );
	DOParse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::ProcessingStart ), NULL, this );
	DOArchiveTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( wxDDSopt::ChangeTreeItem ), NULL, this );
	DOFilter->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeFilter ), NULL, this );
	DOFilterApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::ApplyFilter ), NULL, this );
	this->Connect( wxID_RESET, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxDDSopt::ResetFileList ) );
	this->Connect( wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxDDSopt::ClearFileList ) );
	DORecursive->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxDDSopt::ChangeRecursion ), NULL, this );
	DOArchiveList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( wxDDSopt::ChangeSelectedPreview ), NULL, this );
	DOArchiveList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( wxDDSopt::ChangeSelectedFiles ), NULL, this );
	DOOutText->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxDDSopt::TypedOut ), NULL, this );
	DOOutBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::BrowseOut ), NULL, this );
	DOProcess->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::ProcessingStart ), NULL, this );
	DOPreviewSelector->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxDDSopt::ChangePreview ), NULL, this );
	DOPreview->Connect( wxEVT_PAINT, wxPaintEventHandler( wxDDSopt::PaintH ), NULL, this );
}

wxDDSopt::~wxDDSopt()
{
	// Disconnect Events
	this->Disconnect( wxID_AUTO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeToOblivion ) );
	this->Disconnect( wxID_OBLIVON, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeToOblivion ) );
	this->Disconnect( wxID_SKYRIM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeToSkyrim ) );
	this->Disconnect( wxID_NHDR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeLeaveHDR ) );
	this->Disconnect( wxID_NMTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeNormalTS ) );
	this->Disconnect( wxID_NMSP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeNormalSteep ) );
	this->Disconnect( wxID_CMGM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeColorGamma ) );
	this->Disconnect( wxID_DOCT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeDoCompressImages ) );
	this->Disconnect( wxID_DOOT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeDoOptimizeImages ) );
	this->Disconnect( wxID_SKIPP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipProcessing ) );
	this->Disconnect( wxID_FORCE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeForceCompression ) );
	this->Disconnect( wxID_SKIPE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipExisting ) );
	this->Disconnect( wxID_SKIPN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipNewer ) );
	this->Disconnect( wxID_SKIPH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipHidden ) );
	this->Disconnect( wxID_SKIPC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipHash ) );
	this->Disconnect( wxID_SKIPB, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeSkipBroken ) );
	this->Disconnect( wxID_SKIPX, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeUnselectExtras ) );
	this->Disconnect( wxID_LOGF, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeLogFile ) );
	DOToolSwitch->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( wxDDSopt::ChangeTool ), NULL, this );
	DOInput->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxDDSopt::ChangePluginDir ), NULL, this );
	DOInText->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxDDSopt::TypedIn ), NULL, this );
	DOInBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::BrowseIn ), NULL, this );
	DOParse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::ProcessingStart ), NULL, this );
	DOArchiveTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( wxDDSopt::ChangeTreeItem ), NULL, this );
	DOFilter->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( wxDDSopt::ChangeFilter ), NULL, this );
	DOFilterApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::ApplyFilter ), NULL, this );
	this->Disconnect( wxID_RESET, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxDDSopt::ResetFileList ) );
	this->Disconnect( wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxDDSopt::ClearFileList ) );
	DORecursive->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxDDSopt::ChangeRecursion ), NULL, this );
	DOArchiveList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( wxDDSopt::ChangeSelectedPreview ), NULL, this );
	DOArchiveList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( wxDDSopt::ChangeSelectedFiles ), NULL, this );
	DOOutText->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxDDSopt::TypedOut ), NULL, this );
	DOOutBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::BrowseOut ), NULL, this );
	DOProcess->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxDDSopt::ProcessingStart ), NULL, this );
	DOPreviewSelector->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxDDSopt::ChangePreview ), NULL, this );
	DOPreview->Disconnect( wxEVT_PAINT, wxPaintEventHandler( wxDDSopt::PaintH ), NULL, this );
	
}

wxProgress::wxProgress( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,265 ), wxDefaultSize );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	m_panel9 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	DOSubject = new wxStaticText( m_panel9, wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0 );
	DOSubject->Wrap( -1 );
	bSizer26->Add( DOSubject, 0, wxALL, 5 );
	
	DOTask = new wxGauge( m_panel9, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,25 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	bSizer26->Add( DOTask, 0, wxALL|wxEXPAND, 5 );
	
	DOReport = new wxStaticText( m_panel9, wxID_ANY, wxT("Efficiency:"), wxDefaultPosition, wxDefaultSize, 0 );
	DOReport->Wrap( -1 );
	bSizer26->Add( DOReport, 0, wxALL, 5 );
	
	DOEfficiency = new wxGauge( m_panel9, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,25 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	bSizer26->Add( DOEfficiency, 0, wxALL|wxEXPAND, 5 );
	
	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer( 2, 2, 0, 25 );
	
	m_staticText25 = new wxStaticText( m_panel9, wxID_ANY, wxT("Running:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	gSizer6->Add( m_staticText25, 0, wxALL, 2 );
	
	DORunning = new wxStaticText( m_panel9, wxID_ANY, wxT("00:00s"), wxDefaultPosition, wxDefaultSize, 0 );
	DORunning->Wrap( -1 );
	gSizer6->Add( DORunning, 0, wxALIGN_RIGHT|wxALL, 2 );
	
	m_staticText27 = new wxStaticText( m_panel9, wxID_ANY, wxT("Remaining:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	gSizer6->Add( m_staticText27, 0, wxALL, 2 );
	
	DORemaining = new wxStaticText( m_panel9, wxID_ANY, wxT("00:00s"), wxDefaultPosition, wxDefaultSize, 0 );
	DORemaining->Wrap( -1 );
	gSizer6->Add( DORemaining, 0, wxALIGN_RIGHT|wxALL, 2 );
	
	m_staticText29 = new wxStaticText( m_panel9, wxID_ANY, wxT("Total:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	gSizer6->Add( m_staticText29, 0, wxALL, 2 );
	
	DOTotal = new wxStaticText( m_panel9, wxID_ANY, wxT("00:00s"), wxDefaultPosition, wxDefaultSize, 0 );
	DOTotal->Wrap( -1 );
	gSizer6->Add( DOTotal, 0, wxALIGN_RIGHT|wxALL, 1 );
	
	bSizer26->Add( gSizer6, 0, wxALIGN_CENTER, 5 );
	
	m_staticline7 = new wxStaticLine( m_panel9, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer26->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	DOPause = new wxButton( m_panel9, wxID_ANY, wxT("Pause"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( DOPause, 0, wxALL, 5 );
	
	DOAbort = new wxButton( m_panel9, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( DOAbort, 0, wxALL, 5 );
	
	bSizer26->Add( bSizer27, 0, wxALIGN_CENTER, 5 );
	
	m_panel9->SetSizer( bSizer26 );
	m_panel9->Layout();
	bSizer26->Fit( m_panel9 );
	bSizer25->Add( m_panel9, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer25 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( wxProgress::AbortProgress ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( wxProgress::IdleProgress ) );
	DOPause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::PauseProgress ), NULL, this );
	DOAbort->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::AbortProgress ), NULL, this );
}

wxProgress::~wxProgress()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( wxProgress::AbortProgress ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxProgress::IdleProgress ) );
	DOPause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::PauseProgress ), NULL, this );
	DOAbort->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::AbortProgress ), NULL, this );
	
}
