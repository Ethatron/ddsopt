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
  void DDSoptGUI::ResetPButtons() {
    /* enable button */
    DOSaveAs->Enable(
      (DOTexturePath->GetLabel() != "")
    );
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::PaintH(wxPaintEvent& event) {
    wxPaintDC dc(DOPreview);

    wxRect rg = DOPreview->GetUpdateRegion().GetBox();
    int dw = DOPreview->GetClientSize().GetWidth();
    int dh = DOPreview->GetClientSize().GetHeight();
    int sw = hdc.GetSize().GetWidth();
    int sh = hdc.GetSize().GetHeight();

#if 0
    if (hdc.IsOk())
      dc.Blit(
	0,
	0,
	DOPreview->GetClientSize().GetWidth(),
	DOPreview->GetClientSize().GetHeight(),
	&hdc,
	0,
	0,
	wxCOPY
      );
#elif 1
    if (hdc.IsOk())
      dc.StretchBlit(
	0,
	0,
	dw,
	dh,
	&hdc,
	0,
	0,
	sw,
	sh,
	wxCOPY
      );
#else
    if (hdc.IsOk())
    if ((rg.GetWidth() > 0) &&
    	(rg.GetHeight() > 0))
      dc.StretchBlit(
	rg.GetLeft(),
	rg.GetTop(),
	rg.GetWidth(),
	rg.GetHeight(),
	&hdc,
	(rg.GetLeft() * sw) / dw,
	(rg.GetTop() * sh) / dh,
	(rg.GetWidth() * sw) / dw,
	(rg.GetHeight() * sh) / dh,
	wxCOPY
      );
#endif
  }

  void DDSoptGUI::RedrawH() {
    wxString ph = DOTexturePath->GetLabel();
    if (ph.IsNull())
      return;

    /* look what we'v got */
    struct ioinfo info;
    if (iostat(ph.data(), &info))
      return;

    /**/ if (DOGame->FindItem(wxID_OBLIVON)->IsChecked())
      ;
    else if (DOGame->FindItem(wxID_SKYRIM)->IsChecked())
      ;

    /* ... */
    int width = rWidth->GetValueAsVariant().GetInteger();
    int height = rHeight->GetValueAsVariant().GetInteger();

    /* read in the file-contents */
    void *inmem = NULL;
    UINT insize =    0;
    struct iofile *file;
    if ((file = ioopenfile(ph.data(), "rb"))) {
      inmem = malloc(insize = (UINT)info.io_size);

      UINT rdsze = (UINT)
      ioreadfile(inmem, insize, file);
      ioclosefile(file);

      if (rdsze != insize) {
	free(inmem);
	return;
      }
    }
    else
      return;

    /* only if we got something */
    if (inmem && insize) {
      DOStatusBar->SetStatusText(wxT("Skimming texture ..."), 0);

      /* bad hack, so what? */
      if (((DWORD *)inmem)[0] == MAKEFOURCC('D', 'D', 'S', ' ') && ((DDS_HEADER *)inmem)->dwCubemapFlags) {
	LPDIRECT3DCUBETEXTURE9 base = NULL;
	HRESULT res;

	if ((res = D3DXCreateCubeTextureFromFileInMemoryEx(
	  pD3DDevice, inmem, insize,
	  max(width, height), 0/*D3DX_DEFAULT*/,
	  0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE/*D3DX_DEFAULT*/,
	  D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	  &base
	  )) == D3D_OK) {
	  int offsetx[] = {
	    2 * width,	// D3DCUBEMAP_FACE_POSITIVE_X     = 0,
	    0 * width,	// D3DCUBEMAP_FACE_NEGATIVE_X     = 1,
	    1 * width,	// D3DCUBEMAP_FACE_POSITIVE_Y     = 2,
	    3 * width,	// D3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
	    1 * width,	// D3DCUBEMAP_FACE_POSITIVE_Z     = 4,
	    1 * width,	// D3DCUBEMAP_FACE_NEGATIVE_Z     = 5,
	  };

	  int offsety[] = {
	    1 * height,	// D3DCUBEMAP_FACE_POSITIVE_X     = 0,
	    1 * height,	// D3DCUBEMAP_FACE_NEGATIVE_X     = 1,
	    1 * height,	// D3DCUBEMAP_FACE_POSITIVE_Y     = 2,
	    1 * height,	// D3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
	    0 * height,	// D3DCUBEMAP_FACE_POSITIVE_Z     = 4,
	    2 * height,	// D3DCUBEMAP_FACE_NEGATIVE_Z     = 5,
	  };

	  int transform[] = {
	    -2,	// D3DCUBEMAP_FACE_POSITIVE_X     = 0,
	    -3,	// D3DCUBEMAP_FACE_NEGATIVE_X     = 1,
	    -1,	// D3DCUBEMAP_FACE_POSITIVE_Y     = 2,
	    3,	// D3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
	    -1,	// D3DCUBEMAP_FACE_POSITIVE_Z     = 4,
	    3,	// D3DCUBEMAP_FACE_NEGATIVE_Z     = 5,
	  };

	  /**/ if (DOGame->FindItem(wxID_OBLIVON)->IsChecked())
	    ;
	  else if (DOGame->FindItem(wxID_SKYRIM)->IsChecked())
	    ;

	  unsigned char *rgb;
	  unsigned char *a;
	  wxImage *im = &hgt;
	  if (!im->IsOk() || (im->GetWidth() != (width * 4)) || (im->GetHeight() != (height * 3))) {
	    if (im->IsOk())
	      im->Destroy();

	    if (!im->Create((width * 4), (height * 3), false))
	      assert(NULL);
	  }

	  int w = im->GetWidth();
	  int h = im->GetHeight();
	  rgb = im->GetData();
	  if (im->HasAlpha())
	    a = im->GetAlpha();
	  memset(rgb, 0, w * h * 3);

	  /* ... */
	  if (DOPreviewSelector->GetSelection() == 0) {
	    D3DLOCKED_RECT texs;

	    for (int f = D3DCUBEMAP_FACE_POSITIVE_X; f <= D3DCUBEMAP_FACE_NEGATIVE_Z; f++) {
	      base->LockRect((D3DCUBEMAP_FACES)f, 0, &texs, NULL, 0);
	      ULONG *sTex = (ULONG *)texs.pBits;

	      if (1) {
		for (int y = 0; y < height; y++)
		for (int x = 0; x < width ; x++) {
		  ULONG t;

		  switch (transform[f]) {
		    case -3: t = sTex[(((width  - 1) - x) * height) + ((height - 1) - y)]; break;
		    case -2: t = sTex[(                x  * height) + (               y)]; break;
		    case -1: t = sTex[(((height - 1) - y) * width ) + (               x)]; break;
		    case  2: t = sTex[(                x  * height) + ((height - 1) - y)]; break;
		    case  1: t = sTex[(((width  - 1) - x) * height) + (               y)]; break;
		    case  3: t = sTex[(                y  * width ) + ((width  - 1) - x)]; break;
		    default: t = sTex[(                y  * width ) + (               x)]; break;
		  }

		  ULONG a = (t >> 24) & 0xFF; /*a*/
		  ULONG r = (t >> 16) & 0xFF; /*a*/
		  ULONG g = (t >>  8) & 0xFF; /*a*/
		  ULONG b = (t >>  0) & 0xFF; /*a*/

		  rgb[(((offsety[f] + y) * (width * 4)) + (offsetx[f] + x)) * 3 + 0] = min(255, r);
		  rgb[(((offsety[f] + y) * (width * 4)) + (offsetx[f] + x)) * 3 + 1] = min(255, g);
		  rgb[(((offsety[f] + y) * (width * 4)) + (offsetx[f] + x)) * 3 + 2] = min(255, b);
		}
	      }

	      base->UnlockRect((D3DCUBEMAP_FACES)f, 0);
	    }
	  }
	  else if (DOPreviewSelector->GetSelection() == 1) {
	    D3DLOCKED_RECT texs;

	    for (int f = D3DCUBEMAP_FACE_POSITIVE_X; f <= D3DCUBEMAP_FACE_NEGATIVE_Z; f++) {
	      base->LockRect((D3DCUBEMAP_FACES)f, 0, &texs, NULL, 0);
	      ULONG *sTex = (ULONG *)texs.pBits;

	      if (1) {
		for (int y = 0; y < height; y++)
		for (int x = 0; x < width ; x++) {
		  ULONG t;

		  switch (transform[f]) {
		    case -3: t = sTex[(((width  - 1) - x) * height) + ((height - 1) - y)]; break;
		    case -2: t = sTex[(                x  * height) + (               y)]; break;
		    case -1: t = sTex[(((height - 1) - y) * width ) + (               x)]; break;
		    case  2: t = sTex[(                x  * height) + ((height - 1) - y)]; break;
		    case  1: t = sTex[(((width  - 1) - x) * height) + (               y)]; break;
		    case  3: t = sTex[(                y  * width ) + ((width  - 1) - x)]; break;
		    default: t = sTex[(                y  * width ) + (               x)]; break;
		  }

		  ULONG a = (t >> 24) & 0xFF; /*a*/
		  ULONG r = (t >> 16) & 0xFF; /*a*/
		  ULONG g = (t >>  8) & 0xFF; /*a*/
		  ULONG b = (t >>  0) & 0xFF; /*a*/

		  rgb[(((offsety[f] + y) * (width * 4)) + (offsetx[f] + x)) * 3 + 0] = min(255, a);
		  rgb[(((offsety[f] + y) * (width * 4)) + (offsetx[f] + x)) * 3 + 1] = min(255, a);
		  rgb[(((offsety[f] + y) * (width * 4)) + (offsetx[f] + x)) * 3 + 2] = min(255, a);
		}
	      }

	      base->UnlockRect((D3DCUBEMAP_FACES)f, 0);
	    }
	  }
	}

	if (base)
	  base->Release();
      }
      else {
	LPDIRECT3DTEXTURE9 base = NULL;
	HRESULT res;

	if ((res = D3DXCreateTextureFromFileInMemoryEx(
	  pD3DDevice, inmem, insize,
	  width, height, 0/*D3DX_DEFAULT*/,
	  0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE/*D3DX_DEFAULT*/,
	  D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	  &base
	  )) == D3D_OK) {
	  /**/ if (DOGame->FindItem(wxID_OBLIVON)->IsChecked())
	    ;
	  else if (DOGame->FindItem(wxID_SKYRIM)->IsChecked())
	    ;

	  unsigned char *rgb;
	  unsigned char *a;
	  wxImage *im = &hgt;
	  if (!im->IsOk() || (im->GetWidth() != width) || (im->GetHeight() != height)) {
	    if (im->IsOk())
	      im->Destroy();

	    if (!im->Create(width, height, false))
	      assert(NULL);
	  }

	  int w = im->GetWidth();
	  int h = im->GetHeight();
	  rgb = im->GetData();
	  if (im->HasAlpha())
	    a = im->GetAlpha();

	  /* ... */
	  if (DOPreviewSelector->GetSelection() == 0) {
	    D3DLOCKED_RECT texs;

	    base->LockRect(0, &texs, NULL, 0);
	    ULONG *sTex = (ULONG *)texs.pBits;

	    if (issuf(ph.data(), "_n") ||
		issuf(ph.data(), "_fn") ||
		issuf(ph.data(), "_xyz") ||
		issuf(ph.data(), "_xyzd") ||
		issuf(ph.data(), "_msn")) {
	      for (int y = 0; y < height; y++)
	      for (int x = 0; x < width ; x++) {
		ULONG t = sTex[(y * width) + (x)];
		ULONG a = ((t >> 24) & 0xFF); /*a*/
		 LONG r = ((t >> 16) & 0xFF) - 0x80; /*a*/
		 LONG g = ((t >>  8) & 0xFF) - 0x80; /*a*/
		 LONG b = ((t >>  0) & 0xFF) - 0x80; /*a*/

		float len = sqrt((1.0f / 0x3F01) * (r * r) + (1.0f / 0x3F01) * (g * g) + (1.0f / 0x3F01) * (b * b));
		if (!len)
		  len = 0x01, b = 0x7F, g = 0x00, r = 0x00;
		else
		  len = 1.0f / len;

		r = (LONG)floor(len * r + 0.5);
		g = (LONG)floor(len * g + 0.5);
		b = (LONG)floor(len * b + 0.5);

		rgb[((y * width) + (x)) * 3 + 0] = min(255, r + 0x80);
		rgb[((y * width) + (x)) * 3 + 1] = min(255, g + 0x80);
		rgb[((y * width) + (x)) * 3 + 2] = min(255, b + 0x80);
	      }
	    }
	    else {
	      for (int y = 0; y < height; y++)
	      for (int x = 0; x < width ; x++) {
		ULONG t = sTex[(y * width) + (x)];
		ULONG a = (t >> 24) & 0xFF; /*a*/
		ULONG r = (t >> 16) & 0xFF; /*a*/
		ULONG g = (t >>  8) & 0xFF; /*a*/
		ULONG b = (t >>  0) & 0xFF; /*a*/

		rgb[((y * width) + (x)) * 3 + 0] = min(255, r);
		rgb[((y * width) + (x)) * 3 + 1] = min(255, g);
		rgb[((y * width) + (x)) * 3 + 2] = min(255, b);
	      }
	    }

	    base->UnlockRect(0);
	  }
	  else if (DOPreviewSelector->GetSelection() == 1) {
	    D3DLOCKED_RECT texs;

	    base->LockRect(0, &texs, NULL, 0);
	    ULONG *sTex = (ULONG *)texs.pBits;

	    if (1) {
	      for (int y = 0; y < height; y++)
	      for (int x = 0; x < width ; x++) {
		ULONG t = sTex[(y * width) + (x)];
		ULONG a = (t >> 24) & 0xFF; /*a*/
		ULONG r = (t >> 16) & 0xFF; /*a*/
		ULONG g = (t >>  8) & 0xFF; /*a*/
		ULONG b = (t >>  0) & 0xFF; /*a*/

		rgb[((y * width) + (x)) * 3 + 0] = min(255, a);
		rgb[((y * width) + (x)) * 3 + 1] = min(255, a);
		rgb[((y * width) + (x)) * 3 + 2] = min(255, a);
	      }

	      im->ConvertToGreyscale();
	    }

	    base->UnlockRect(0);
	  }
	}

	if (base)
	  base->Release();
      }

      hdc.SelectObject(wxBitmap(hgt));

//    this->Refresh();
      DOPreview->Update();
      DOPreview->Refresh();

      if (inmem)
	free(inmem);

      DOStatusBar->SetStatusText(wxT("Ready"), 0);
    }

    ResetPButtons();
  }

  void DDSoptGUI::ChangeSelectedPreview(wxString ph) {
    DOTexturePath->Hide();
    DOPreviewSelector->Hide();
    DOPreviewSelector->Clear();
    DOPreview->Hide();
    DOTextureFirst->Show();
    DOTextureFirst->GetParent()->Layout();
    DOTextureInfos->Clear();
    DOSaveFormat->Disable();
    DOSaveAs->Disable();

    D3DXIMAGE_INFO nfo;
    rWidth = rHeight = NULL;

    if (ph.IsNull())
      return;

    /* does it exist? */
    /* look what we'v got */
    struct ioinfo info;
    if (iostat(ph.data(), &info))
      return;

    /* read in the file-contents */
    void *inmem = NULL;
    UINT insize =    0;
    struct iofile *file;
    if ((file = ioopenfile(ph.data(), "rb"))) {
      inmem = malloc(insize = (UINT)info.io_size);

      UINT rdsze = (UINT)
      ioreadfile(inmem, insize, file);
      ioclosefile(file);

      if (rdsze != insize) {
	free(inmem);
	return;
      }
    }
    else
      return;

    HRESULT res = D3DXGetImageInfoFromFileInMemory(inmem, insize, &nfo);
    free(inmem);
    if (res != D3D_OK)
      return;
/*
    if (((DDS_HEADER *)inmem)->dwCubemapFlags)
    if (nfo.ResourceType)
      nfo.Width /= 6;
*/

    DOTexturePath->SetLabel(ph);
    DOTexturePath->Show();
    DOPreview->Show();
    DOTextureFirst->Hide();
    DOTextureFirst->GetParent()->Layout();

    /**/ if (DOGame->FindItem(wxID_OBLIVON)->IsChecked())
      ;
    else if (DOGame->FindItem(wxID_SKYRIM)->IsChecked())
      ;
 
    {
      DOStatusBar->SetStatusText(wxT("Analyzing texture ..."), 0);
      DOStatusBar->SetStatusText(wxT("Ready"), 0);
    }

    wxPGProperty *p;
    DOTextureInfos->AppendCategory("Infos");
    DOTextureInfos->Append(p = wxStringProperty("Type", wxPG_LABEL, findType(nfo.ResourceType))); p->SetFlag(wxPG_PROP_READONLY);
    DOTextureInfos->Append(p = wxStringProperty("Primary use", wxPG_LABEL, UsedforDDS(ph.data(), 0))); p->SetFlag(wxPG_PROP_READONLY);;
    if (findAlpha(nfo.Format))
    DOTextureInfos->Append(p = wxStringProperty("Alpha use", wxPG_LABEL, UsedforDDS(ph.data(), 1))); p->SetFlag(wxPG_PROP_READONLY);
    wxPGId dimProp =
    DOTextureInfos->Append(wxParentProperty("Dimensions", wxPG_LABEL));
    DOTextureInfos->AppendIn(dimProp, p = wxIntProperty("Width", wxPG_LABEL, nfo.Width)); rWidth = p; p->SetFlag(wxPG_PROP_READONLY);
    DOTextureInfos->AppendIn(dimProp, p = wxIntProperty("Height", wxPG_LABEL, nfo.Height)); rHeight = p; p->SetFlag(wxPG_PROP_READONLY);
    DOTextureInfos->AppendIn(dimProp, p = wxIntProperty("Mips", wxPG_LABEL, nfo.MipLevels)); p->SetFlag(wxPG_PROP_READONLY);
    wxPGId fmtProp =
    DOTextureInfos->Append(wxParentProperty("Format", wxPG_LABEL));
    DOTextureInfos->AppendIn(fmtProp, p = wxStringProperty("Type", wxPG_LABEL, findFileformat(nfo.ImageFileFormat))); p->SetFlag(wxPG_PROP_READONLY);
    DOTextureInfos->AppendIn(fmtProp, p = wxStringProperty("Layout", wxPG_LABEL, findFormat(nfo.Format))); p->SetFlag(wxPG_PROP_READONLY);
    DOTextureInfos->ExpandAll();
    DOTextureInfos->SetSize(wxSize(-1, 210));
    DOTextureInfos->SetMinSize(wxSize(-1, 210));

    DOPreviewSelector->Append("Primary");
    if (findAlpha(nfo.Format)) {
      DOPreviewSelector->Append("Alpha");
      DOPreviewSelector->Show(); }
    DOPreviewSelector->SetSelection(0);

    DOSaveAs->Enable();
    DOSaveFormat->Clear();
    if (issuf(ph.data(), "_n") ||
	issuf(ph.data(), "_fn") ||
	issuf(ph.data(), "_xyz") ||
	issuf(ph.data(), "_xyzd") ||
	issuf(ph.data(), "_msn")) {
      if (findAlpha(nfo.Format)) {
	DOSaveFormat->Append("DDS - A8R8G8B8", (void *)0);
	DOSaveFormat->Append("DDS - A4R4G4B4", (void *)2);
      }

      DOSaveFormat->Append("DDS - X8R8G8B8", (void *)8);
      DOSaveFormat->Append("DDS - X4R4G4B4", (void *)10);
      DOSaveFormat->Append("DDS - R8G8B8", (void *)5);	// unsupported, hu?
      DOSaveFormat->Append("DDS - R5G6B5", (void *)6);
    }
    else {
      if (findAlpha(nfo.Format)) {
	DOSaveFormat->Append("DDS - A8R8G8B8", (void *)0);
	DOSaveFormat->Append("DDS - A1R5G5B5", (void *)1);
	DOSaveFormat->Append("DDS - A4R4G4B4", (void *)2);
	DOSaveFormat->Append("DDS - A8L8", (void *)3);
	DOSaveFormat->Append("DDS - A4L4", (void *)4);
      }

      DOSaveFormat->Append("DDS - X8R8G8B8", (void *)8);
      DOSaveFormat->Append("DDS - X1R5G5B5", (void *)9);
      DOSaveFormat->Append("DDS - X4R4G4B4", (void *)10);
      DOSaveFormat->Append("DDS - R8G8B8", (void *)5);	// unsupported, hu?
      DOSaveFormat->Append("DDS - R5G6B5", (void *)6);
      DOSaveFormat->Append("DDS - L8", (void *)7);
    }

    if (findAlpha(nfo.Format))  {
      DOSaveFormat->Append("DDS - Optimal DXT (with alpha)", (void *)-2);
      DOSaveFormat->Append("DDS - Optimal DXT (strip alpha)", (void *)-3);
    }
    else {
      DOSaveFormat->Append("DDS - Optimal DXT", (void *)-3);
    }

    DOSaveFormat->Append("PNG", (void *)-4);
    DOSaveFormat->Enable();
    DOSaveFormat->SetSelection(0);

//  SetHeightfield(&hgt, _rt);
    RedrawH();

    DOTextureFirst->GetParent()->Layout();

    /* next tab */
    DOToolSwitch->SetSelection(1);
  }

  void DDSoptGUI::ChangeSelectedPreview(wxCommandEvent& event) {
    int which = event.GetSelection();
//  wxOwnerDrawn it = DOArchiveList->GetItem(which);
    iomap::iterator sel = ldirectory[which];
    wxString path = DOInText->GetValue();
    path.Append(sel->first);

    ChangeSelectedPreview(path);
  }

  void DDSoptGUI::ChangePreview(wxCommandEvent& event) {
    RedrawH();
  }

  /* ---------------------------------------------------------------------------- */
  void DDSoptGUI::SaveAsStart(wxCommandEvent& event) {
    DOStatusBar->SetStatusText(wxT("Saving ..."), 0);
    wxBusyCursor wait;

    int type = (int)DOSaveFormat->GetClientData(DOSaveFormat->GetSelection());

    wxFileDialog dlg(
      this,
      _T("Save texture"),
      wxT(""),
      wxEmptyString,
      (type == -4 ? _T("PNG (*.png)|*.png") : _T("DDS (*.dds)|*.dds")),
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

    if (dlg.ShowModal() != wxID_OK)
      return;

    // get filename
    wxString m_filename = dlg.GetFilename();
    wxString m_filepath = dlg.GetPath();

    if (1) {
      wxString ph = DOTexturePath->GetLabel();
      if (ph.IsNull())
	return;

      /* look what we'v got */
      struct ioinfo info;
      if (iostat(ph.data(), &info))
	return;

      /* read in the file-contents */
      void *inmem = NULL;
      UINT insize =    0;
      struct iofile *file;
      if ((file = ioopenfile(ph.data(), "rb"))) {
	inmem = malloc(insize = (UINT)info.io_size);

	UINT rdsze = (UINT)
	ioreadfile(inmem, insize, file);
	ioclosefile(file);

	if (rdsze != insize) {
	  free(inmem);
	  return;
	}
      }
      else
	return;

      /* only if we got something */
      if (inmem && insize) {
	LPDIRECT3DTEXTURE9 base = NULL;
	HRESULT res;

	normalmapts     = DOBTextures->FindChildItem(wxID_NMTS, NULL)->IsChecked();
	normalsteepness = DOBTextures->FindChildItem(wxID_NMSP, NULL)->IsChecked() ? 2 : 1;
	colormapgamma   = DOBTextures->FindChildItem(wxID_CMGM, NULL)->IsChecked();

	/* read the ini in */
	WhitelistDDS();

	if ((res = D3DXCreateTextureFromFileInMemoryEx(
	  pD3DDevice, inmem, insize,
	  D3DX_DEFAULT, D3DX_DEFAULT, 0/*D3DX_DEFAULT*/,
	  0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
	  D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	  &base
	)) == D3D_OK) {
	  bool success = false;

	  /* look if we have an individual overwrite here */
	  unsigned int cfg = WhitelistedDDS(ph.data());

	  if (issuf(ph.data(), "_n") ||
	      issuf(ph.data(), "_nor") ||
	      issuf(ph.data(), "_fn") ||
	      issuf(ph.data(), "_xyz") ||
	      issuf(ph.data(), "_xyzd") ||
	      issuf(ph.data(), "_msn") || (cfg & WL_NORMALT) || (cfg & WL_NORMALM)) {
	    /* only known object-space normal-maps */
	    bool ts = normalmapts;
	    if (stristr(ph.data(), "landscapelod\\") || stristr(ph.data(), "terrain\\"))
	      ts = false;
	    if (issuf(ph.data(), "_msn"))
	      ts = false;
	    if (cfg & WL_NORMALT)
	      ts = normalmapts;

	    switch (type) {
	      case  0: 
		if (ts) success = TextureConvertXYZD (&base, 0);
		else    success = TextureConvert_XYZD(&base, 0); break;
	      case  1: break;
	      case  2: 
		if (ts) success = TextureQuantizeX4Y4Z4D4 (&base, 0);
		else    success = TextureQuantize_X4Y4Z4D4(&base, 0); break;
	      case  3: break;
	      case  4: break;
	      case  5: 
		if (ts) success = TextureConvertXYZ  (&base, 0);
		else    success = TextureConvert_XYZ (&base, 0); break;
	      case  6: 
		if (ts) success = TextureQuantizeX5Y6Z5   (&base, 0);
		else    success = TextureQuantize_X5Y6Z5  (&base, 0); break;
	      case  7: break;
	      case  8: 
		if (ts) success = TextureConvertXYZV (&base, 0);
		else    success = TextureConvert_XYZV(&base, 0); break;
	      case  9: break;
	      case  10: 
		if (ts) success = TextureQuantizeX4Y4Z4V4 (&base, 0);
		else    success = TextureQuantize_X4Y4Z4V4(&base, 0); break;
	      case -2: 
		if (ts) success = TextureCompressXYZD (&base, 0);
		else    success = TextureCompress_XYZD(&base, 0); break;
	      case -3: 
		if (ts) success = TextureCompressXYZ  (&base, 0);
		else    success = TextureCompress_XYZ (&base, 0); break;
	      case -4: 
		if (ts) success = true;
		else    success = true; break;
	    }
	  }
	  else {
	    bool ga = !stristr(ph.data(), "faces\\") && !stristr(ph.data(), "facemods\\");
	    bool al =  stristr(ph.data(), "\\menus") || DOGame->FindChildItem(wxID_SKYRIM , NULL)->IsChecked();

	    switch (type) {
	      case  0: 
		if (al) success = TextureConvertRGBA (&base, 0, ga);
		else    success = TextureConvertRGBH (&base, 0, ga); break;
	      case  1: 
		if (al) success = TextureQuantizeR5G5B5A1 (&base, 0, ga);
		else    success = TextureQuantizeR5G5B5H1 (&base, 0, ga); break;
	      case  2: 
		if (al) success = TextureQuantizeR4G4B4A4 (&base, 0, ga);
		else    success = TextureQuantizeR4G4B4H4 (&base, 0, ga); break;
	      case  3: 
		if (al) success = TextureConvertLA (&base, 0, ga);
		else    success = TextureConvertLH (&base, 0, ga); break;
	      case  4: 
		if (al) success = TextureQuantizeL4A4 (&base, 0, ga);
		else    success = TextureQuantizeL4H4 (&base, 0, ga); break;
	      case  5: 
		if (al) success = TextureConvertRGB  (&base, 0, ga);
		else    success = TextureConvertRGB  (&base, 0, ga); break;
	      case  6: 
		if (al) success = TextureQuantizeR5G6B5   (&base, 0, ga);
		else    success = TextureQuantizeR5G6B5   (&base, 0, ga); break;
	      case  7: 
		if (al) success = TextureConvertL (&base, 0, ga);
		else    success = TextureConvertL (&base, 0, ga); break;
	      case  8: 
		if (al) success = TextureConvertRGBV (&base, 0, ga);
		else    success = TextureConvertRGBV (&base, 0, ga); break;
	      case  9: 
		if (al) success = TextureQuantizeR5G5B5V1 (&base, 0, ga);
		else    success = TextureQuantizeR5G5B5V1 (&base, 0, ga); break;
	      case  10: 
		if (al) success = TextureQuantizeR4G4B4V4 (&base, 0, ga);
		else    success = TextureQuantizeR4G4B4V4 (&base, 0, ga); break;
	      case -2: 
		if (al) success = TextureCompressRGBA (&base, 0, ga);
		else    success = TextureCompressRGBH (&base, 0, ga); break;
	      case -3: 
		if (al) success = TextureCompressRGB  (&base, 0, ga);
		else    success = TextureCompressRGB  (&base, 0, ga); break;
	      case -4: 
		if (al) success = true;
		else    success = true; break;
	    }
	  }

	  if (success) {
	    /* check for resolution-overflow and extract lower mip-levels */
	    if (1) {
	      D3DSURFACE_DESC scheck;
	      bool limit; int lw, lh;
	      int limitw, limith;

	      int pdn = DOSaveRes->GetSelection(); lw = 8192, lh = 8192;
	      if (pdn >= 17)
		lw = lh = -(pdn - 17);
	      else
		while (pdn-- > 0) {
		  lh >>= 1;
		  if (pdn-- > 0) {
		    lw >>= 1;
		  }}

	      base->GetLevelDesc(0, &scheck);

	      /* orientation doesn't matter */
	      limit = !(((scheck.Width <= lw) && (scheck.Height <= lh)) ||
			((scheck.Width <= lh) && (scheck.Height <= lw)));

	      /**/ if (scheck.Width > scheck.Height) {
		limitw = max(lw, lh);
		limith = min(lw, lh);
	      }
	      else if (scheck.Width < scheck.Height) {
		limitw = min(lw, lh);
		limith = max(lw, lh);
	      }
	      else {
		limitw = min(lw, lh);
		limith = min(lw, lh);
	      }

	      /* these are percentages */
	      if (limitw <= 0) limit = true, limitw = scheck.Width  >> (-limitw);
	      if (limith <= 0) limit = true, limith = scheck.Height >> (-limith);

	      /* not necessary if no change */
	      if (limit && (limitw != scheck.Width) && (limith != scheck.Height)) {
		/* we try, but we don't fail on this one */
		if (!TextureDownMip(limitw, limith, &base)) {
		  wxMessageDialog d(this, "Failed to downscale the texture.\nTexture will be saved unscaled", "DDSopt error");
		  d.ShowModal();
		}
	      }
	    }

	    if (type == -4)
	      res = D3DXSaveTextureToFile(m_filepath.data(), D3DXIFF_PNG, base, NULL);
	    else
	      res = D3DXSaveTextureToFile(m_filepath.data(), D3DXIFF_DDS, base, NULL);
	  }
	  else {
	    wxMessageDialog d(this, "Failed to convert texture.", "DDSopt error");
	    d.ShowModal();
	  }

	  base->Release();
	}

	free(inmem);
      }
    }

    DOStatusBar->SetStatusText(wxT("Ready"), 0);
  }
