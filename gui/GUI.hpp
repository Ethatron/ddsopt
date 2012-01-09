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

#ifndef DDSOPT_GUI_HPP
#define DDSOPT_GUI_HPP

#include "../DDSopt.h"
#include "../io/io.h"

DWORD __stdcall ProcessingStart(LPVOID lp);

// ----------------------------------------------------------------------------
class ioio;
class iost;
typedef	map<string, ioio> iomap;
typedef	map<string, iost> stmap;

class ioio {
public:
  ioio() { selected = true; skip = iex = oex = false; }

  bool skip, selected;

  struct ioinfo in; bool iex;
  struct ioinfo ou; bool oex;

#if 0
  static bool compare(
    const iomap::iterator d1,
    const iomap::iterator d2
  ) {
    return (d1->second.in.io_time < d2->second.in.io_time);
  }
#endif
};

class iost {
public:
  iost() { imsk = omsk /*= icnt = ocnt*/ = 0; }

  int imsk; //int icnt;
  int omsk; //int ocnt;

#if 0
  static bool compare(
    const iomap::iterator d1,
    const iomap::iterator d2
    ) {
      return (d1->second.in.io_time < d2->second.in.io_time);
  }
#endif
};

class ioTreeItemData : public wxTreeItemData {
public:
  ioTreeItemData(string &fp) : wxTreeItemData() {fullpath = fp;}
  string fullpath;
};

// ----------------------------------------------------------------------------
class DDSoptGUI; extern class DDSoptGUI *gui;
class DDSoptGUI : public wxDDSopt
{
  friend class DDSoptApp;

public:
  int iarchive, oarchive;
  int iactives, oactives;
  int iprogres, oprogres;
  iomap fdirectory;
  stmap ddirectory;
  vector<iomap::iterator> ldirectory;
  wxRegEx filter;
  string currentpath;
  bool warmup;

private:
  // Installed Path
  char IPath[1024];
  char OPath[1024];
  HKEY Settings;
  DDSoptPrg *prog;

  /* ---------------------------------------------------------------------------- */
  void ResetHButtons();

  /* ---------------------------------------------------------------------------- */
  void ChangeLeaveHDR(wxCommandEvent& event);
  void ChangeNormalTS(wxCommandEvent& event);
  void ChangeNormalSteep(wxCommandEvent& event);
  void ChangeColorGamma(wxCommandEvent& event);
  void ChangeDoCompressImages(wxCommandEvent& event);
  void ChangeDoOptimizeImages(wxCommandEvent& event);

  /* ---------------------------------------------------------------------------- */
  void ChangeSkipExisting(wxCommandEvent& event);
  void ChangeSkipNewer(wxCommandEvent& event);
  void ChangeSkipHidden(wxCommandEvent& event);
  void ChangeSkipHash(wxCommandEvent& event);
  void ChangeSkipBroken(wxCommandEvent& event);
  void ChangeUnselectExtras(wxCommandEvent& event);
  void ChangeLogFile(wxCommandEvent& event);

  /* ---------------------------------------------------------------------------- */
  void ResetCFileList();
  void ClearCFileList();
  void LoadCFileList();
  void SaveCFileList();

  /* ---------------------------------------------------------------------------- */
  void FilterDirectory(wxTreeItemId root, const wxString& sFilterFor);
  void RefreshDirectory(wxTreeItemId root, int lvl = 0);
  wxTreeItemId FindItem(wxTreeItemId root, wxString name);
  wxTreeItemId ScanEntry(const wxTreeItemId &parent, const char *name, const char *dle);

  int Scan(const char *name, const char *locl, const wxTreeItemId &parent, int lvl, bool o);

  void DirectoryFromFiles(int io = 0);

  void ChangeFilter(wxCommandEvent& event);
  void ApplyFilter(wxCommandEvent& event);

  void ResetFileList(wxCommandEvent& event);
  void ClearFileList(wxCommandEvent& event);
  void LoadFileList(wxCommandEvent& event);
  void SaveFileList(wxCommandEvent& event);

  void ChangeRecursion(wxCommandEvent& event);
  void ChangeTreeItem(wxTreeEvent& event);
  void ChangeTreeItem(const char *basedir, int op = 0);
  void ChangeSelectedFiles(wxCommandEvent& event);
  void ChangeSelectedPreview(wxCommandEvent& event);
  void ChangeSelectedPreview(wxString ph);

  /* ---------------------------------------------------------------------------- */
  void TypedIn(wxCommandEvent& event);
  void BrowseIn(wxCommandEvent& event);
  void BrowseIn(wxString ph);

  /* ---------------------------------------------------------------------------- */
  void TypedOut(wxCommandEvent& event);
  void BrowseOut(wxCommandEvent& event);
  void BrowseOut(wxString ph);

  /* ---------------------------------------------------------------------------- */
  void ProcessingStart(wxCommandEvent& event);
public:
  void Process(const char *inname, const char *ouname, const char *lcname);
  void ProcessingStart();
private:

  /* **************************************************************************** */
  wxImage hgt;
  wxMemoryDC hdc;

  void PaintH(wxPaintEvent& event);
  void RedrawH();

  wxPGProperty *rWidth;
  wxPGProperty *rHeight;

  void ChangePreview(wxCommandEvent& event);

public:
  bool RequestFeedback(const char *question);

public:
  DDSoptGUI::DDSoptGUI(const wxString& title);
  DDSoptGUI::~DDSoptGUI();
};

#endif
