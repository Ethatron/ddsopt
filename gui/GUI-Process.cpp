/* Version: MPL 1.1/LGPL 3.0
 *
 * "The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is DDSopt.
 *
 * The Initial Developer of the Original Code is
 * Ethatron <niels@paradice-insight.us>. Portions created by The Initial
 * Developer are Copyright (C) 2011 The Initial Developer.
 * All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Library General Public License Version 3 license (the
 * "LGPL License"), in which case the provisions of LGPL License are
 * applicable instead of those above. If you wish to allow use of your
 * version of this file only under the terms of the LGPL License and not
 * to allow others to use your version of this file under the MPL,
 * indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL License.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under either the MPL or the LGPL License."
 */

#include "GUI.hpp"
#include "../globals.h"
#include "../io/bsa.h"
#include "../io/texture-dds.h"
#include "../process/process.h"

//class DDSoptGUI *gui;

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::ResetCFileList() {
    iomap::iterator walk = fdirectory.begin();
    while (walk != fdirectory.end()) {
      walk->second.selected = true;
      walk++;
    }

    ResetHButtons();
  }

  void DDSoptGUI::ClearCFileList() {
    iomap::iterator walk = fdirectory.begin();
    while (walk != fdirectory.end()) {
      walk->second.selected = false;
      walk++;
    }

    ResetHButtons();
  }

  void DDSoptGUI::LoadCFileList() {
#if 0
    int num = OSFileList->GetCount();
    for (int n = 0; n < num; n++) {
      const char *p = OSFileList->GetItem(n)->GetName().data();
      char s[32] = ""; DWORD sl = 32;

      RegGetValue(Settings, "Plugins", p, RRF_RT_REG_SZ, NULL, s, &sl);

      if (s[0] != '\0')
	OSFileList->Check(n, s[0] == '1');
    }
#endif
  }

  void DDSoptGUI::SaveCFileList() {
#if 0
    int num = OSFileList->GetCount();
    for (int n = 0; n < num; n++) {
      const char *p = OSFileList->GetItem(n)->GetName().data();
      const char *s = "0";
      if (OSFileList->IsChecked(n))
	s = "1";

      RegSetKeyValue(Settings, "Plugins", p, RRF_RT_REG_SZ, s, (DWORD)strlen(s) + 1);
    }
#endif
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::FilterDirectory(wxTreeItemId root, const wxString& sFilterFor) {
    wxTreeItemIdValue cookie;
    wxTreeItemId item = DOArchiveTree->GetFirstChild(root, cookie);
    wxTreeItemId child;

    while (item.IsOk()) {
      wxString sData = DOArchiveTree->GetItemText(item);
      if (sFilterFor.CompareTo(sData) == 0) {
	;
      }

      if (DOArchiveTree->ItemHasChildren(item)) {
	FilterDirectory(item, sFilterFor);
      }

      item = DOArchiveTree->GetNextChild(root, cookie);
    }
  }

  void DDSoptGUI::RefreshDirectory(wxTreeItemId root, int lvl) {
    wxTreeItemIdValue cookie;
    wxTreeItemId item = DOArchiveTree->GetFirstChild(root, cookie);
    wxTreeItemId child;

    while (item.IsOk()) {
//    wxString sData = DOArchiveTree->GetItemText(item);
      if (lvl == 0) {
	DOArchiveTree->Expand(item);
//	DOArchiveTree->SelectItem(item);
      }

      if (DOArchiveTree->ItemHasChildren(item)) {
	RefreshDirectory(item, lvl + 1);
      }

      item = DOArchiveTree->GetNextChild(root, cookie);
    }
  }

  wxTreeItemId DDSoptGUI::FindItem(wxTreeItemId root, wxString name) {
    wxTreeItemIdValue cookie;
    wxTreeItemId item = DOArchiveTree->GetFirstChild(root, cookie);
    wxTreeItemId child;

    while (item.IsOk()) {
      wxString sData = DOArchiveTree->GetItemText(item);
      if (!sData.CmpNoCase(name))
	return item;

      item = DOArchiveTree->GetNextChild(root, cookie);
    }

    return item;
  }

  wxTreeItemId DDSoptGUI::ScanEntry(const wxTreeItemId &parent, const char *name, const char *dle) {
    ioTreeItemData *dt = new ioTreeItemData(string(name));

    wxTreeItemId entry = FindItem(parent, dle);
    if (!entry.IsOk())
      entry = DOArchiveTree->AppendItem(parent, dle, -1, -1, (wxTreeItemData *)dt);

    return entry;
  }

#define OEX 1
#define IEX 2
#define CNT 4
  int DDSoptGUI::Scan(const char *name, const char *locl, const wxTreeItemId &parent, int lvl, bool o) {
    size_t ilen = (name ? strlen(name) : 0);
    size_t llen = (locl ? strlen(locl) : 0);
    int msk = 0;

    struct ioinfo info;
    if (!iostat(name, &info)) {
      /* input: directory */
      if (info.io_type & IO_DIRECTORY) {
	const char *dle;
	char tmp[256];
	class iost *stat = &ddirectory[locl];

	/* strip trailing slash */
	if (!(dle = strrchr(name, '/')))
	  if (!(dle = strrchr(name, '\\')))
	    dle = name - 1;

	dle += 1;

	/* no BSAs inside BSAs! */
	if (!o && oarchive && isext(locl, "bsa"))
	  return msk;

	sprintf(tmp, "Skimming %s \"%s\" ...", o ? "output" : "input", dle);
	DOStatusBar->SetStatusText(wxT(tmp), 0);

	/* the in and output directories are parallel-roots */
	wxTreeItemId entry = (!lvl ? parent : ScanEntry(parent, locl, dle));

	/* walk */
	struct iodir *dir;
	if ((dir = ioopendir(name))) {
	  struct iodirent *et;

	  while ((et = ioreaddir(dir))) {
	    if (!strcmp(et->name, ".") ||
		!strcmp(et->name, ".."))
	      continue;

	    char *nname = (char *)malloc(ilen + 1 + et->namelength + 1);
	    char *lname = (char *)malloc(llen + 1 + et->namelength + 1);

	    strcpy(nname, name);
	    strcpy(lname, locl);

	    strcat(nname, "\\");
	    strcat(lname, "\\");

	    strcat(nname, et->name);
	    strcat(lname, et->name);

	    strlwr(lname);

	    /* recurse */
	    int lmsk = Scan(nname, lname, entry, lvl + 1, o);

	    /* break down the field */
	    msk = ((lmsk & ~3) + (msk & ~3)) | (lmsk & 3) | (msk & 3);

	    free(nname);
	    free(lname);
	  }

	  ioclosedir(dir);

	  /* register */
	  if (o) stat->omsk = msk;
	  else   stat->imsk = msk;

	  /* combine */
	  msk = stat->omsk | stat->imsk;

	  /* assign the color */
	  wxColour col(0, 0, 0, 255);
	  if (!(msk & IEX))
	    col = wxColour(140, 140, 140, 255);
	  else if (!(msk & OEX))
	    col = wxColour(0, 60, 0, 255);

	  DOArchiveTree->SetItemTextColour(entry, col);
//	  DOArchiveTree->SetItemText(entry, col);
	}
	else {
	  sprintf(rerror, "Can't open the directory \"%s\"\n", name);
	  throw runtime_error(rerror);
	}
      }
      /* input: file */
      else {
	const char *fle;
	bool dealloc = false;
	bool docopy = true;
	bool iszero = !info.io_size;

	/* strip trailing slash */
	if (!(fle = strrchr(name, '/')))
	  if (!(fle = strrchr(name, '\\')))
	    fle = name - 1;

	fle += 1;
	if (stristr(fle, "thumbs.db"))
	  return 0;

	bool firsttime = (fdirectory.count(locl) == 0);
	class ioio *nfo = &fdirectory[locl];
	if (dropextras) {
	  /* none of this stuff inside BSAs! */
	  if (!o && oarchive && firsttime) {
	    if (isext(fle, "esp")) nfo->selected = false;
	    if (isext(fle, "esm")) nfo->selected = false;
	    if (isext(fle, "pdf")) nfo->selected = false;
	    if (isext(fle, "psd")) nfo->selected = false;
	  }

//	  if (!docopy)
//	    fprintf(stderr, "filtered \"%s\"\n", fle);
	}

//	nfo->selected = newio;
	nfo->skip = docopy;
	if (o) {
	  nfo->oex = true;
	  nfo->ou = info;

	  oactives++;
	}
	else {
	  nfo->iex = true;
	  nfo->in = info;

	  iactives += (nfo->selected ? 1 : 0);
	}

	msk |= (nfo->oex ? OEX : 0) | (nfo->iex ? IEX : 0);
	msk += CNT;
      }
    }

    return msk;
  }

  void DDSoptGUI::DirectoryFromFiles(int io) {
    if (warmup)
      return;

    skipexisting  = DOSettings->FindChildItem(wxID_SKIPE, NULL)->IsChecked();
    skipnewer     = DOSettings->FindChildItem(wxID_SKIPN, NULL)->IsChecked();
    skiphashcheck = DOSettings->FindChildItem(wxID_SKIPC, NULL)->IsChecked();
    skipbroken    = DOSettings->FindChildItem(wxID_SKIPB, NULL)->IsChecked();
    processhidden = DOSettings->FindChildItem(wxID_SKIPH, NULL)->IsChecked();

    wxBusyCursor busy;
    wxTreeItemId root;

    iarchive = isarchive(DOInText ->GetValue().data());
    oarchive = isarchive(DOOutText->GetValue().data());

    if (!io) {
      iactives =
      oactives = 0;

      ddirectory.clear();
      fdirectory.clear();
    }
    else if (io == 1) {
      iactives = 0;

      {
	iomap newmap;
	iomap::iterator walk = fdirectory.begin();
	while (walk != fdirectory.end()) {
	  if (walk->second.oex) {
	    ioio *m = &newmap[walk->first];

	    m->skip     = walk->second.skip;
	    m->selected = walk->second.selected;

	    m->oex = walk->second.oex;
	    m->ou  = walk->second.ou;
	  }

	  walk++;
	}

	fdirectory = newmap;
      }

      {
	stmap newmap;
	stmap::iterator walk = ddirectory.begin();
	while (walk != ddirectory.end()) {
	  if (walk->second.omsk & OEX) {
	    iost *m = &newmap[walk->first];

	    m->omsk = walk->second.omsk;
	  }

	  walk++;
	}

	ddirectory = newmap;
      }
    }
    else if (io == 2) {
      oactives = 0;

      {
	iomap newmap;
	iomap::iterator walk = fdirectory.begin();
	while (walk != fdirectory.end()) {
	  if (walk->second.iex) {
	    ioio *m = &newmap[walk->first];

	    m->skip     = walk->second.skip;
	    m->selected = walk->second.selected;

	    m->iex = walk->second.iex;
	    m->in  = walk->second.in;
	  }

	  walk++;
	}

	fdirectory = newmap;
      }

      {
	stmap newmap;
	stmap::iterator walk = ddirectory.begin();
	while (walk != ddirectory.end()) {
	  if (walk->second.imsk & IEX) {
	    iost *m = &newmap[walk->first];

	    m->imsk = walk->second.imsk;
	  }

	  walk++;
	}

	ddirectory = newmap;
      }
    }

    DOArchiveList->Clear();
    DOArchiveTree->DeleteAllItems(); root =
    DOArchiveTree->AddRoot("\\");

    try {
//    if (!io || (io == 1)) {
	DOStatusBar->SetStatusText(wxT("Skimming input ..."), 0);
	Scan(DOInText->GetValue().data(), "", root, 0, false);
//    }

//    if (!io || (io == 2)) {
	DOStatusBar->SetStatusText(wxT("Skimming output ..."), 0);
	Scan(DOOutText->GetValue().data(), "", root, 0, true);
//    }
    }
    catch(exception &e) {
      if (strcmp(e.what(), "ExitThread")) {
	wxMessageDialog d(this, e.what(), "DDSopt error");
	d.ShowModal();
      }
    }

    RefreshDirectory(root);

    /* preselect root (will trigger ChangedItem()) */
    DOArchiveTree->Expand(root);
    DOArchiveTree->SelectItem(root, true);

    DOStatusBar->SetStatusText(wxT("Ready"), 0);
  }

  void DDSoptGUI::ChangeFilter(wxCommandEvent& event) {
//  filter = event.GetString();
  }

  void DDSoptGUI::ApplyFilter(wxCommandEvent& event) {
    /* . -> \.
     * * -> .*
     */
    wxString spat = DOFilter->GetValue();
    wxString wildcard = spat;
    wildcard.Replace(".", "\\.", true);
    wildcard.Replace("*", ".*", true);

    if (!filter.Compile(wildcard))
      filter.Compile("");
    else
      RegSetKeyValue(Settings, NULL, "Filter", RRF_RT_REG_SZ, spat.data(), (DWORD)spat.length() + 1);

    /* refresh file-list */
    ChangeTreeItem(currentpath.data());
  }

  void DDSoptGUI::ResetFileList(wxCommandEvent& event) {
//  ResetCFileList();

    /* refresh file-list */
    ChangeTreeItem(currentpath.data(), 1);
  }

  void DDSoptGUI::ClearFileList(wxCommandEvent& event) {
//  ClearCFileList();

    /* refresh file-list */
    ChangeTreeItem(currentpath.data(), 2);
  }

  void DDSoptGUI::LoadFileList(wxCommandEvent& event) {
    LoadCFileList();
  }

  void DDSoptGUI::SaveFileList(wxCommandEvent& event) {
    SaveCFileList();
  }

  void DDSoptGUI::ChangeRecursion(wxCommandEvent& event) {
    RegSetKeyValue(Settings, NULL, "Show Recursive", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);

    /* refresh file-list */
    ChangeTreeItem(currentpath.data());
  }

  void DDSoptGUI::ChangeTreeItem(wxTreeEvent& event) {
    wxTreeItemId item = event.GetItem();
    ioTreeItemData *iod = (ioTreeItemData *)DOArchiveTree->GetItemData(item);
    currentpath = (iod ? iod->fullpath : "");

    /* refresh file-list */
    ChangeTreeItem(currentpath.data());
  }

  void DDSoptGUI::ChangeTreeItem(const char *basedir, int op) {
    wxBusyCursor wait;
//  DOStatusBar->SetStatusText(wxT("Running ..."), 0);

    if (op != 0) {
      size_t n = ldirectory.size();
      while (n-- > 0) {
	iomap::iterator walk = ldirectory[n];

	/* apply operation locally */
	bool selected =
	  op == 2 ? false :
	  op == 1 ? true  : walk->second.selected;

	/**/ if ( selected && !walk->second.selected)
	  iactives++;
	else if (!selected &&  walk->second.selected)
	  iactives--;

	walk->second.selected = selected;

	/* change the item */
	DOArchiveList->Check(n, walk->second.selected && walk->second.iex);
      }

      return;
    }

    wxFont fnt = DOArchiveList->GetFont();
    const wxNativeFontInfo *fni = fnt.GetNativeFontInfo();
    wxNativeFontInfo sni = *fni; sni.lf.lfStrikeOut = TRUE;
    wxFont str = fnt; str.SetNativeFontInfo(sni);

    skipexisting  = DOSettings->FindChildItem(wxID_SKIPE, NULL)->IsChecked();
    skipnewer     = DOSettings->FindChildItem(wxID_SKIPN, NULL)->IsChecked();
    skiphashcheck = DOSettings->FindChildItem(wxID_SKIPC, NULL)->IsChecked();
    skipbroken    = DOSettings->FindChildItem(wxID_SKIPB, NULL)->IsChecked();
    processhidden = DOSettings->FindChildItem(wxID_SKIPH, NULL)->IsChecked();

    DOArchiveList->Clear();
/*  If we don't want root selectable do this: if (!basedir[0]) return; */

    DOStatusBar->SetStatusText(wxT("Refreshing file-list ..."), 0);
    ldirectory.clear();
    iomap::iterator walk = fdirectory.begin();
    while (walk != fdirectory.end()) {
      const char *fname = walk->first.data();

      if ((fname == stristr(fname, basedir))) {
	const char *fbase = fname + strlen(basedir) + 1;
	/* prevent matching similarly named subdirectories */
	if (fbase[-1] == '\\')

	/* recursive or direct file */
	if ((DORecursive->GetValue() || !strchr(fbase, '\\')) && filter.Matches(fbase)) {
	  /* apply operation locally */
	  bool selected =
	    op == 2 ? false :
	    op == 1 ? true  : walk->second.selected;

	  /**/ if ( selected && !walk->second.selected)
	    iactives++;
	  else if (!selected &&  walk->second.selected)
	    iactives--;

	  walk->second.selected = selected;

	  /* add the item */
	  int n =
	  DOArchiveList->Append(fbase);
	  DOArchiveList->Check(n, walk->second.selected && walk->second.iex);
	  wxOwnerDrawn *id =
	  DOArchiveList->GetItem(n);

	  /* client-data substitute */
	  if (n >= ldirectory.size())
	    ldirectory.resize(n + 1);
	  ldirectory[n] = walk;

	  /* mark destination-only */
	  id->SetCheckable(walk->second.iex);
	  wxColour col(0, 0, 0, 255);
	  if (!walk->second.iex)
	    col = wxColour(140, 140, 140, 255);
	  else if (!walk->second.oex)
	    col = wxColour(0, 60, 0, 255);

	  /* mark no-overwrite */
	  if (walk->second.iex && walk->second.oex) {
	    if (skipexisting)
	      id->SetFont(str);
	    else if (skipnewer && (walk->second.in.io_time <= walk->second.ou.io_time))
	      id->SetFont(str);
	  }

	  id->SetTextColour(col);
	}
      }

      walk++;
    }

    DOStatusBar->SetStatusText(wxT("Ready"), 0);
  }

  void DDSoptGUI::ChangeSelectedFiles(wxCommandEvent& event) {
    int n = event.GetSelection();
    iomap::iterator f = ldirectory[n];
    /* event.IsChecked() doesn't work here */
    bool selected = DOArchiveList->IsChecked(n);

    /**/ if ( selected && !f->second.selected)
      iactives++;
    else if (!selected &&  f->second.selected)
      iactives--;

    f->second.selected = selected;
  }

  void DDSoptGUI::ActivateTreeItem(wxTreeEvent& event) {
    wxTreeItemId item = event.GetItem();
    ioTreeItemData *iod = (ioTreeItemData *)DOArchiveTree->GetItemData(item);

    if (iod) {
      wxString ph = IPath;
      ph.Append(iod->fullpath);

      DOInText->SetValue(ph);
      BrowseIn(ph);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::TypedIn(wxCommandEvent& event) { return;
    wxString ph = event.GetString();
//  DOInText->SetValue(ph);

    if (ph.IsNull())
      return;
    /* does it exist? */
    if (iosize(ph.data()) == -1)
      return;

    if (stricmp(IPath, ph.data()))
      BrowseIn(ph);
  }

  void DDSoptGUI::TypedInDone(wxFocusEvent& event) {
    wxString ph = DOInText->GetValue();

    if (ph.IsNull())
      return;
    /* does it exist? */
    if (iosize(ph.data()) == -1)
      return;

    if (stricmp(IPath, ph.data()))
      BrowseIn(ph);
  }

  void DDSoptGUI::BrowseIn(wxCommandEvent& event) {
    wxBusyCursor wait;
    HRESULT hr = AskInput(IPath);
    if (!SUCCEEDED(hr))
      return;

    wxString ph = selected_string;
    DOInText->SetValue(ph);

    if (!ph.IsNull()) {
    }

    BrowseIn(ph);
  }

  void DDSoptGUI::BrowseIn(wxString ph) {
    DOProcess->Enable(FALSE);

    strcpy(IPath, ph.data());

//  ResetCFileList();
    ResetHButtons();

    RegSetKeyValue(Settings, NULL, "Input Location", RRF_RT_REG_SZ, IPath, (DWORD)strlen(IPath) + 1);

    if (ph.IsNull())
      return;

    DirectoryFromFiles(1);
    ResetHButtons();
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::TypedOut(wxCommandEvent& event) { return;
    wxString ph = event.GetString();
    //  DOOutText->SetValue(ph);

    if (ph.IsNull())
      return;

    if (stricmp(OPath, ph.data()))
      BrowseOut(ph);
  }

  void DDSoptGUI::TypedOutDone(wxFocusEvent& event) {
    wxString ph = DOOutText->GetValue();
    DOOutCText->SetValue(ph);

    if (ph.IsNull())
      return;

    if (stricmp(OPath, ph.data()))
      BrowseOut(ph);
  }

  void DDSoptGUI::BrowseOut(wxCommandEvent& event) {
    wxBusyCursor wait;
    SetOutput(DOInText->GetValue().data());
    HRESULT hr = AskOutput(OPath);
    if (!SUCCEEDED(hr))
      return;

    wxString ph = selected_string;
    DOOutText->SetValue(ph);
    DOOutCText->SetValue(ph);

    if (!ph.IsNull()) {
    }

    BrowseOut(ph);
  }

  void DDSoptGUI::TypedOutC(wxCommandEvent& event) {
    TypedOut(event);
  }

  void DDSoptGUI::TypedOutCDone(wxFocusEvent& event) {
    wxString ph = DOOutCText->GetValue();
    DOOutText->SetValue(ph);

    if (ph.IsNull())
      return;

    if (stricmp(OPath, ph.data()))
      BrowseOut(ph);
  }

  void DDSoptGUI::BrowseOutC(wxCommandEvent& event) {
    BrowseOut(event);
  }

  void DDSoptGUI::BrowseOut(wxString ph) {
    DOProcess->Enable(FALSE);
    DOProcessC->Enable(FALSE);

    strcpy(OPath, ph.data());

//  ResetCFileList();
    ResetHButtons();

    RegSetKeyValue(Settings, NULL, "Output Location", RRF_RT_REG_SZ, OPath, (DWORD)strlen(OPath) + 1);

    if (ph.IsNull())
      return;
    /* does it exist?
    if (iosize(ph.data()) == -1)
      return; */

    DirectoryFromFiles(2);
    ResetHButtons();
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::ProcessingStart(wxCommandEvent& event) {
    DOStatusBar->SetStatusText(wxT("Running ..."), 0);
    wxBusyCursor wait;
    prog = new DDSoptPrg(this);
    int ret = prog->Enter(::ProcessingStart);
    delete prog;

    /* wait for the asynchronous processes ... */
    ioflush();
    free_arc();

    DOStatusBar->SetStatusText(wxT("Ready"), 0);
    prog = NULL;
    if (ret != 666)
      return;

    /* reflect the changes made to the output */
    DirectoryFromFiles(2);
  }

  void DDSoptGUI::Process(const char *inname, const char *ouname, const char *lcname) {
    size_t ilen = (inname ? strlen(inname) : 0);
    size_t olen = (ouname ? strlen(ouname) : ilen);
    size_t llen = (lcname ? strlen(lcname) : 0);

    bool rm = false;
    struct ioinfo iinfo, oinfo;
    if (!iostat(inname, &iinfo)) {
      /* input: directory */
      if (iinfo.io_type & IO_DIRECTORY) {
	/* don't create output in simulation-mode */
	if (ouname && !simulation && iostat(ouname, &oinfo)) {
	  rm = true;

	  if (iomkdir(ouname)) {
	    sprintf(rerror, "Can't create the directory \"%s\"\n", ouname);
	    throw runtime_error(rerror);
	  }

	  if (iostat(ouname, &oinfo)) {
	    sprintf(rerror, "Can't find the directory \"%s\"\n", ouname);
	    throw runtime_error(rerror);
	  }
	}

	if ((oinfo.io_type & IO_DIRECTORY) || simulation) {
	  struct iodir *dir;

	  /* no BSAs inside BSAs! */
	  if (oarchive && isext(lcname, "bsa"))
	    return;

	  if ((dir = ioopendir(inname))) {
	    struct iodirent *et;

	    while ((et = ioreaddir(dir))) {
	      if (!strcmp(et->name, ".") ||
		  !strcmp(et->name, ".."))
		continue;

	      char *ninname = (char *)malloc(ilen + 1 + et->namelength + 1);
	      char *nouname = (char *)malloc(olen + 1 + et->namelength + 1);
	      char *nlcname = (char *)malloc(llen + 1 + et->namelength + 1);

	      strcpy(ninname, inname);
	      strcpy(nouname, ouname ? ouname : inname);
	      strcpy(nlcname, lcname);

	      strcat(ninname, "\\");
	      strcat(nouname, "\\");
	      strcat(nlcname, "\\");

	      strcat(ninname, et->name);
	      strcat(nouname, et->name);
	      strcat(nlcname, et->name);

	      strlwr(nlcname);

	      Process(ninname, nouname, nlcname);

	      free(ninname);
	      free(nouname);
	      free(nlcname);
	    }

	    ioclosedir(dir);

	    /* don't create output in simulation-mode */
	    if (ouname && !simulation && (dir = ioopendir(ouname))) {
	      struct iodirent *et;
	      int num = 0;

	      while ((et = ioreaddir(dir))) {
		if (!strcmp(et->name, ".") ||
		    !strcmp(et->name, ".."))
		  continue;

		/* ioopen/fopen is synchronous, only close is
		 * asynchronous, which means even non-yet-written
		 * files exist in the folder and we don't need to wait for
		 * completion to check if a folder is empty/non-empty
		 *
		 * no lock/synchronization necessary
		 */
		num++;
	      }

	      ioclosedir(dir);

	      if (!num && rm)
		iormdir(ouname);
	    }
	  }
	  else {
	    sprintf(rerror, "Can't open the directory \"%s\"\n", inname);
	    throw runtime_error(rerror);
	  }
	}
	else {
	  sprintf(rerror, "Destination \"%s\" isn't a directory\n", ouname);
	  throw runtime_error(rerror);
	}
      }
      /* input: file */
      else {
	const char *fle;
	struct iodir *dir;
	bool dealloc = false;
	bool docopy = true;
	bool iszero = !iinfo.io_size;

	/* strip trailing slash */
	if (!(fle = strrchr(inname, '/')))
	  if (!(fle = strrchr(inname, '\\')))
	    fle = inname - 1;

	fle += 1;
	if (stristr(fle, "thumbs.db"))
	  return;

	/* skip unselected ones (iactive is altered by the selection) */
	if (!fdirectory[lcname].selected)
	  return;

	if (!docopy)
	  prog->SetProgress(lcname, iprogres++);
	else
	  prog->SetProgress(lcname, iprogres++);

	/* if there is no destination, clone the source */
	if (ouname && !iostat(ouname, &oinfo)) {
	  if (oinfo.io_type & IO_DIRECTORY) {
	    char *nouname = (char *)malloc(olen + 1 + strlen(fle) + 1);

	    /* if we push a single specific file into a BSA
	     * we have to open/close it to flush the BSA to disk
	     */
	    dir = ioopendir(ouname);

	    strcpy(nouname, ouname);
	    strcat(nouname, "\\");
	    strcat(nouname, fle);

	    ouname = nouname;
	    dealloc = true;
	  }
	  else if (skipexisting)
	    return;
	  else if (skipnewer)
	    if (iinfo.io_time <= oinfo.io_time)
	      return;
	}

        /* global statistics (even for multiple files in one pass) */
        deltaiobytes = 0;

#ifdef	NDEBUG
//        __try {
        try {
#endif
#ifdef	TEXTUREOPT
	  if (isext(inname, "dds") ||
	    ((isext(inname, "png") ||
	      isext(inname, "bmp") ||
	      isext(inname, "dib") ||
	      isext(inname, "tga") ||
	      isext(inname, "ppm") ||
	      isext(inname, "pfm") ||
	      isext(inname, "hdr")) && (compressimages /*|| skipimages*/))) {
	    /*if (skipimages)
	      docopy = false;
	    else*/ if (!skipprocessing && stristr(inname, "textures\\")) {
	      fprintf(stderr, "processing \"%s\"\n", fle);

	      /* TODO: nowait asynchronous */
	      while (1) {
	        docopy = true;
	        try {
	          docopy = !ProcessDDS(inname, ouname, fle);
	          /* okay, done with */
	        }
	        catch(exception &e) {
		  if (strcmp(e.what(), "ExitThread")) {
		    errprintf(stdout, e.what());
		    if (skipbroken)
		      break;

		    char buf[256]; sprintf(buf, "Failed to process file \"%s\". Retry?", lcname);
		    wxMessageDialog d(this, buf, "DDSopt error", wxYES_NO | wxCANCEL | wxCENTRE);
		    int res = d.ShowModal();
		    if (res == wxID_YES)
		      continue;
		    if (res == wxID_NO)
		      break;
		  }

		  throw runtime_error("ExitThread");
	        }

	        break;
	      };
	    }
	  }
#endif
#ifdef	NDEBUG
        }
        catch(exception &e) {
	  errprintf(stderr, "error processing \"%s\": %s\n", fle, e.what());
	  errprintf(stdout, "error processing \"%s\": %s\n", fle, e.what());
	  brokenfiles++;
        }
//        }
//        __except (EXCEPTION_EXECUTE_HANDLER) {
//	  errprintf(stderr, "Fatal abortion while reading the NIF\n");
//	  errprintf(stdout, "Fatal abortion while reading the NIF\n");
//        }
#endif

        /* passthrough-mode, don't create output in simulation-mode */
        if (docopy && passthrough && ouname && !simulation) {
	  /* action required */
	  if (stricmp(inname, ouname)) {
	    fprintf(stderr, "copying \"%s\"\n", fle);

	    /* TODO: nowait asynchronous */
	    while (1) {
	      try {
		iocp(inname, ouname);

		/* uh, MicroSoft, no signed "ssize_t"? */
		processedinbytes += iinfo.io_size;
	      }
	      catch(exception &e) {
		if (strcmp(e.what(), "ExitThread")) {
		  errprintf(stdout, e.what());
		  if (skipbroken)
		    break;

		  char buf[256]; sprintf(buf, "Failed to copy file \"%s\". Retry?", lcname);
		  wxMessageDialog d(this, buf, "DDSopt error", wxYES_NO | wxCANCEL | wxCENTRE);
		  int res = d.ShowModal();
		  if (res == wxID_YES)
		    continue;
		  if (res == wxID_NO)
		    break;
		}

		throw runtime_error("ExitThread");
	      }

	      break;
	    };
	  }

	  if (iszero)
	    zerobytes.push_back(inname);
        }
        else {
	  if (deltaiobytes)
	    nfoprintf(stdout, "delta:            %d bytes %s\n", (deltaiobytes > 0) ? deltaiobytes : -deltaiobytes, (deltaiobytes > 0) ? "less" : "more");
/*	  else
	    nfoprintf(stdout, "delta:            unchanged\n");	*/
        }

	/* progress */
	prog->SetReport("Efficiency: %s to %s bytes",
	  processedinbytes,
	  processedinbytes - compresseddtbytes - virtualbsabytes - (convertedinbytes - convertedoubytes)
	);
	prog->SetProgress(
	  processedinbytes,
	  processedinbytes - compresseddtbytes - virtualbsabytes - (convertedinbytes - convertedoubytes)
	);

	if (dealloc) {
	  ioclosedir(dir);
	  free((void *)ouname);
	}
      }
    }
  }

  void DDSoptGUI::ProcessingStart() {
    skipprocessing = DOIgnore->FindChildItem(wxID_SKIPP, NULL)->IsChecked();
    passthrough    = DOIgnore->FindChildItem(wxID_PASST, NULL)->IsChecked();

    skipexisting  = DOSettings->FindChildItem(wxID_SKIPE, NULL)->IsChecked();
    skipnewer     = DOSettings->FindChildItem(wxID_SKIPN, NULL)->IsChecked();
    skiphashcheck = DOSettings->FindChildItem(wxID_SKIPC, NULL)->IsChecked();
    skipbroken    = DOSettings->FindChildItem(wxID_SKIPB, NULL)->IsChecked();
    processhidden = DOSettings->FindChildItem(wxID_SKIPH, NULL)->IsChecked();
    dropextras    = DOSettings->FindChildItem(wxID_SKIPX, NULL)->IsChecked();
    verbose = true;

    bmsn = false; omsn = cmsn = DOCompressMSN->GetValue();
    bn   = false; on   = cn   = DOCompressN->GetValue();
    bns  = false; ons  = cns  = DOCompressNS->GetValue();
    bc   = false; oc   = cc   = DOCompressC->GetValue();
    bca  = false; oca  = cca  = DOCompressCA->GetValue();
    bg   = false; og   = cg   = DOCompressG->GetValue();
    bga  = false; oga  = cga  = DOCompressGA->GetValue();

    switch(DOCompressMSNFormat->GetSelection()) {
      case  0: fmsn = D3DFMT_R8G8B8; cmsn = false; break;
      case  1: fmsn = D3DFMT_R5G6B5; bmsn = true; break;
      default: fmsn = D3DFMT_UNKNOWN; break; }
    switch(DOCompressNFormat->GetSelection()) {
      case  0: fn = D3DFMT_R8G8B8; cn = false; break;
      case  1: fn = D3DFMT_R5G6B5; bn = true; break;
      default: fn = D3DFMT_UNKNOWN; break; }
    switch(DOCompressNSFormat->GetSelection()) {
      case  0: fns = D3DFMT_A8R8G8B8; cns = false; break;
      case  1: fns = D3DFMT_A4R4G4B4; bns = true; break;
      default: fns = D3DFMT_UNKNOWN; break; }
    switch(DOCompressCFormat->GetSelection()) {
      case  0: fc = D3DFMT_R8G8B8; cc = false; break;
      case  1: fc = D3DFMT_R5G6B5; bc = true; break;
      default: fc = D3DFMT_UNKNOWN; break; }
    switch(DOCompressCAFormat->GetSelection()) {
      case  0: fca = D3DFMT_A8R8G8B8; cca = false; break;
      case  1: fca = D3DFMT_A4R4G4B4; bca = true; break;
      default: fca = D3DFMT_UNKNOWN; break; }
    switch(DOCompressGFormat->GetSelection()) {
      case  0: fg = D3DFMT_L8; cg = false; break;
      default: fg = D3DFMT_UNKNOWN; break; }
    switch(DOCompressGAFormat->GetSelection()) {
      case  0: fga = D3DFMT_A8L8; cga = false; break;
      case  1: fga = D3DFMT_A4L4; bga = true; break;
      default: fga = D3DFMT_UNKNOWN; break; }

    int pdn = DOLimitResPlain->GetSelection(); lpw = 8192, lph = 8192;
    if (pdn >= 17)
      lpw = lph = -(pdn - 17);
    else
      while (pdn-- > 0) {
	lph >>= 1;
	if (pdn-- > 0) {
	  lpw >>= 1;
	}}

    int cdn = DOLimitResCompressed->GetSelection(); lcw = 8192, lch = 8192;
    if (pdn >= 17)
      lcw = lch = -(cdn - 17);
    else
      while (cdn-- > 0) {
	lch >>= 1;
	if (cdn-- > 0) {
	  lcw >>= 1;
	}}

    static const int szs[] = {
      32*1024, 24*1024, 20*1024, 16*1024, 12*1024, 10*1024,
      8*1024, 6*1024, 4*1024, 3*1024, 2*1024, 1.5*1024, 1.25*1024,
      1*1024, 800, 700, 600, 500
    };

    lls = szs[DOLimitSzeLand ->GetSelection()];
    llc = szs[DOLimitSzeChar ->GetSelection()];
    llo = szs[DOLimitSzeOther->GetSelection()];

    gameversion = -1;
    /**/ if (DOGame->FindChildItem(wxID_OBLIVON, NULL)->IsChecked())
      gameversion = OB_BSAHEADER_VERSION, RegSetKeyValue(Settings, NULL, "Game", RRF_RT_REG_SZ, "1", 2);
    else if (DOGame->FindChildItem(wxID_SKYRIM , NULL)->IsChecked())
      gameversion = SK_BSAHEADER_VERSION, RegSetKeyValue(Settings, NULL, "Game", RRF_RT_REG_SZ, "3", 2);
    else
					  RegSetKeyValue(Settings, NULL, "Game", RRF_RT_REG_SZ, "0", 2);

    leavehdrtexts   = DOBTextures->FindChildItem(wxID_NHDR, NULL)->IsChecked();
    normalmapts     = DOBTextures->FindChildItem(wxID_NMTS, NULL)->IsChecked();
    normalsteepness = DOBTextures->FindChildItem(wxID_NMSP, NULL)->IsChecked() ? 2 : 1;
    colormapgamma   = DOBTextures->FindChildItem(wxID_CMGM, NULL)->IsChecked();
    compressimages  = DOBTextures->FindChildItem(wxID_DOCT, NULL)->IsChecked();
    optimizetexts   = DOBTextures->FindChildItem(wxID_DOOT, NULL)->IsChecked();
    colormapalpha   =   DOGame->FindChildItem(wxID_SKYRIM , NULL)->IsChecked();

    ignoreborder = 0;
    /**/ if (DOBorderIgnore->FindChildItem(wxID_BRD0, NULL)->IsChecked())
      ignoreborder = 0;
    else if (DOBorderIgnore->FindChildItem(wxID_BRD1, NULL)->IsChecked())
      ignoreborder = 1;
    else if (DOBorderIgnore->FindChildItem(wxID_BRD2, NULL)->IsChecked())
      ignoreborder = 2;
    else if (DOBorderIgnore->FindChildItem(wxID_BRD4, NULL)->IsChecked())
      ignoreborder = 4;
    else if (DOBorderIgnore->FindChildItem(wxID_BRD8, NULL)->IsChecked())
      ignoreborder = 8;

    srchbestbsa = false;
    thresholdbsa = true;
    /**/ if (DOCompression->FindChildItem(wxID_DOZ0, NULL)->IsChecked())
      compresslevel = 0;
    else if (DOCompression->FindChildItem(wxID_DOZ1, NULL)->IsChecked())
      compresslevel = 1;
    else if (DOCompression->FindChildItem(wxID_DOZ2, NULL)->IsChecked())
      compresslevel = 2;
    else if (DOCompression->FindChildItem(wxID_DOZ3, NULL)->IsChecked())
      compresslevel = 3;
    else if (DOCompression->FindChildItem(wxID_DOZ4, NULL)->IsChecked())
      compresslevel = 4;
    else if (DOCompression->FindChildItem(wxID_DOZ5, NULL)->IsChecked())
      compresslevel = 5;
    else if (DOCompression->FindChildItem(wxID_DOZ6, NULL)->IsChecked())
      compresslevel = 6;
    else if (DOCompression->FindChildItem(wxID_DOZ7, NULL)->IsChecked())
      compresslevel = 7;
    else if (DOCompression->FindChildItem(wxID_DOZ8, NULL)->IsChecked())
      compresslevel = 8;
    else if (DOCompression->FindChildItem(wxID_DOZ9, NULL)->IsChecked())
      compresslevel = 9;
    else if (DOCompression->FindChildItem(wxID_DOZ10Q, NULL)->IsChecked())
      compresslevel = 9, srchbestbsa = true, optimizequick = true;
    else if (DOCompression->FindChildItem(wxID_DOZ10T, NULL)->IsChecked())
      compresslevel = 9, srchbestbsa = true, optimizequick = false;
    /**/ if (DOCompression->FindChildItem(wxID_FORCE , NULL)->IsChecked())
      thresholdbsa = false;

    char cl[] = "0"; cl[0] = '0' + compresslevel;
    RegSetKeyValue(Settings, "Compression", "Level", RRF_RT_REG_SZ, cl, 2);
    RegSetKeyValue(Settings, "Compression", "Search", RRF_RT_REG_SZ, srchbestbsa ? "1" : "0", 2);
    RegSetKeyValue(Settings, "Compression", "Quick", RRF_RT_REG_SZ, optimizequick ? "1" : "0", 2);
    RegSetKeyValue(Settings, "Compression", "Force", RRF_RT_REG_SZ, thresholdbsa ? "0" : "1", 2);

    virtualbsabytes = 0;
    virtualbsafiles = 0;

    compressedinbytes = 0;
    compresseddtbytes = 0;
    compressedoubytes = 0;

    convertedinbytes = 0;
    convertedoubytes = 0;

    processedinbytes = 0;
    processedoubytes = 0;

    modifiedtexts  = 0;
    planartexts    = 0;
    changedformats = 0;

    brokenfiles    = 0;
    fixedfiles     = 0;
    processedfiles = 0;

    /* count again */
    iactives = 0;
    iomap::iterator walk = fdirectory.begin();
    while (walk != fdirectory.end()) {
      if (walk->second.iex)
	iactives += walk->second.selected ? 1 : 0;

      walk++;
    }

    /* initialize progress-bar(s) */
    prog->StartProgress(iactives + 1); iprogres = 0;
    prog->InitProgress("Processing file \"%s\":", "Efficiency:", 0, 1);

    /* if we want a log, open it */
    duplicates.clear();
    if (DOSettings->FindChildItem(wxID_LOGF, NULL)->IsChecked()) {
      wxString ph = DOOutText->GetValue();
      wxFileName log(ph); log.ClearExt(); log.SetExt("log");
      logfile = fopen(log.GetFullPath().data(), (skipexisting || skipnewer ? "ab" : "wb"));
    }

    /* read the ini in */
    WhitelistDDS();

    try {
/*
      if (!skipprocessing) {
	if ((TextureInit() != true))
	  throw runtime_error("Failed to initialize DirectX!");
      }
*/
      Process(DOInText->GetValue().data(), DOOutText->GetValue().data(), "");
/*
      if (!skipprocessing) {
	if (!skipprocessing)
	  TextureCleanup();
      }
*/
    }
    catch(exception &e) {
      if (strcmp(e.what(), "ExitThread")) {
	errprintf(stdout, e.what());

	wxMessageDialog d(prog, e.what(), "DDSopt error");
	d.ShowModal();
      }
      else {
	errprintf(stdout, "Canceled ...");
      }

      /* if we wanted a log, close it */
      if (logfile)
	fclose(logfile);

      prog->Leave(0);
      return;
    }

    /* if we wanted a log, close it */
    if (logfile) {
      summary(logfile, DOOutText->GetValue().data(), iactives);
      fclose(logfile);
    }

    prog->Leave(666);
  }
