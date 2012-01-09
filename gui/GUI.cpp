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

class DDSoptGUI *gui;

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::ResetHButtons() {
    /* enable button */
    DOProcess->Enable(
      (DOInText ->GetValue() != "") &&
      (DOOutText->GetValue() != "")
    );
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::ChangeLeaveHDR(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Textures", "Leave HDR", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeNormalTS(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Textures", "Normal TS", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeNormalSteep(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Textures", "Normal Steep", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeColorGamma(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Textures", "Color Gamma", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeDoCompressImages(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Textures", "Compress", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeDoOptimizeImages(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Textures", "Optimize", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::ChangeSkipExisting(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Skip", "Existing", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);

    /* refresh file-list */
    ChangeTreeItem(currentpath.data());
  }

  void DDSoptGUI::ChangeSkipNewer(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Skip", "Newer", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);

    /* refresh file-list */
    ChangeTreeItem(currentpath.data());
  }

  void DDSoptGUI::ChangeSkipHidden(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Skip", "Hidden", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);

    /* refresh file-list */
    ChangeTreeItem(currentpath.data());
  }

  void DDSoptGUI::ChangeSkipHash(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Skip", "Hash", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeSkipBroken(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Skip", "Broken", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeUnselectExtras(wxCommandEvent& event) {
    RegSetKeyValue(Settings, "Unselect", "Extras", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  void DDSoptGUI::ChangeLogFile(wxCommandEvent& event) {
    RegSetKeyValue(Settings, NULL, "Logging", RRF_RT_REG_SZ, event.IsChecked() ? "1" : "0", 2);
  }

  /* ---------------------------------------------------------------------------- */
  bool DDSoptGUI::RequestFeedback(const char *question) {
    wxMessageDialog d(prg ? (wxWindow *)prg : (wxWindow *)this, question, "DDSopt", wxOK | wxCANCEL | wxCENTRE);
    int ret = d.ShowModal();
    if (ret == wxID_CANCEL)
      return false;
    return true;
  }

  /* ---------------------------------------------------------------------------- */
  DDSoptGUI::DDSoptGUI(const wxString& title)
    : wxDDSopt(NULL, wxID_ANY, title) {
    gui = this;
    warmup = true;

    Settings = 0;
    /**/ if (RegOpenKeyEx  (HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\DDSopt", 0, KEY_READ | KEY_WRITE | KEY_WOW64_32KEY, &Settings) == ERROR_SUCCESS) {
    }
    else if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\DDSopt", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_32KEY, NULL, &Settings, NULL) == ERROR_SUCCESS) {
    }

    char TS[1024]; DWORD TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Filter", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOFilter->SetValue(TS); TSL = 1023;

    /* . -> \.
     * * -> .*
     */
    wxString spat = DOFilter->GetValue();
    wxString wildcard = spat;
    wildcard.Replace(".", "\\.", true);
    wildcard.Replace("*", ".*", true);

    if (!filter.Compile(wildcard))
      filter.Compile("");

    TS[0] = 0; RegGetValue(Settings, "Textures", "Leave HDR", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOBTextures->FindChildItem(wxID_NHDR, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Textures", "Normal TS", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOBTextures->FindChildItem(wxID_NMTS, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Textures", "Normal Steep", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOBTextures->FindChildItem(wxID_NMSP, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Textures", "Color Gamma", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOBTextures->FindChildItem(wxID_CMGM, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Textures", "Compress", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOBTextures->FindChildItem(wxID_DOCT, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Textures", "Optimize", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOBTextures->FindChildItem(wxID_DOOT, NULL)->Check(TS[0] == '1'); TSL = 1023;

    TS[0] = 0; RegGetValue(Settings, "Skip", "Existing", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_SKIPE, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Skip", "Newer", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_SKIPN, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Skip", "Hidden", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_SKIPH, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Skip", "Hash", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_SKIPC, NULL)->Check(TS[0] == '1'); TSL = 1023; skiphashcheck = (TS[0] == '1');
    TS[0] = 0; RegGetValue(Settings, "Skip", "Broken", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_SKIPB, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Unselect", "Extras", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_SKIPX, NULL)->Check(TS[0] == '1'); TSL = 1023; dropextras = (TS[0] == '1');
    TS[0] = 0; RegGetValue(Settings, NULL, "Logging", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOSettings->FindChildItem(wxID_LOGF, NULL)->Check(TS[0] == '1'); TSL = 1023;

    TS[0] = 0; RegGetValue(Settings, NULL, "Show Recursive", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DORecursive->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Game", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      case '0': DOGame->FindChildItem(wxID_AUTO   , NULL)->Check(true); break;
      case '1': DOGame->FindChildItem(wxID_OBLIVON, NULL)->Check(true); break;
      case '3': DOGame->FindChildItem(wxID_SKYRIM , NULL)->Check(true); break;
    } TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Compression", "Force", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOCompression->FindChildItem(wxID_FORCE, NULL)->Check(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, "Compression", "Level", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      case '0': DOCompression->FindChildItem(wxID_DOZ0, NULL)->Check(true); break;
      case '1': DOCompression->FindChildItem(wxID_DOZ1, NULL)->Check(true); break;
      case '2': DOCompression->FindChildItem(wxID_DOZ2, NULL)->Check(true); break;
      case '3': DOCompression->FindChildItem(wxID_DOZ3, NULL)->Check(true); break;
      case '4': DOCompression->FindChildItem(wxID_DOZ4, NULL)->Check(true); break;
      case '5': DOCompression->FindChildItem(wxID_DOZ5, NULL)->Check(true); break;
      case '6': DOCompression->FindChildItem(wxID_DOZ6, NULL)->Check(true); break;
      case '7': DOCompression->FindChildItem(wxID_DOZ7, NULL)->Check(true); break;
      case '8': DOCompression->FindChildItem(wxID_DOZ8, NULL)->Check(true); break;
      case '9': DOCompression->FindChildItem(wxID_DOZ9, NULL)->Check(true); TSL = 1023;
	TS[0] = 0; RegGetValue(Settings, "Compression", "Search", RRF_RT_REG_SZ, NULL, TS, &TSL);
	if (TS[0]) DOCompression->FindChildItem(wxID_DOZ10T, NULL)->Check(TS[0] == '1'); TSL = 1023;
	TS[0] = 0; RegGetValue(Settings, "Compression", "Quick", RRF_RT_REG_SZ, NULL, TS, &TSL);
	if (TS[0]) DOCompression->FindChildItem(wxID_DOZ10Q, NULL)->Check(TS[0] == '1'); TSL = 1023;
	break;
    } TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Input Location", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOInText->SetValue(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Output Location", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) DOOutText->SetValue(TS); TSL = 1023;

    warmup = false;

//  BrowseIn(DOInText->GetValue());
//  BrowseOut(DOOutText->GetValue());

    ResetHButtons();
  }

  DDSoptGUI::~DDSoptGUI() {
    if (Settings)
      RegCloseKey(Settings);
  }
