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
      if (((DDS_HEADER *)inmem)->dwCubemapFlags) {
	LPDIRECT3DCUBETEXTURE9 base = NULL;
	HRESULT res;

	if ((res = D3DXCreateCubeTextureFromFileInMemoryEx(
	  pD3DDevice, inmem, insize,
	  D3DX_DEFAULT, 0/*D3DX_DEFAULT*/,
	  0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
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
	  D3DX_DEFAULT, D3DX_DEFAULT, 0/*D3DX_DEFAULT*/,
	  0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
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

	    if (1) {
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
  }

  void DDSoptGUI::ChangeSelectedPreview(wxString ph) {
    DOTexturePath->Hide();
    DOPreviewSelector->Hide();
    DOPreviewSelector->Clear();
    DOPreview->Hide();
    DOTextureFirst->Show();
    DOTextureFirst->GetParent()->Layout();
    DOTextureInfos->Clear();

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
    if (res != D3D_OK)
      return;
/*
    if (((DDS_HEADER *)inmem)->dwCubemapFlags)
    if (nfo.ResourceType)
      nfo.Width /= 6;
*/
    free(inmem);

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
    DOTextureInfos->Append(p = wxStringProperty("Primary use", wxPG_LABEL, UsedforDDS(ph.data(), 0))); p->SetFlag(wxPG_PROP_READONLY);
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
