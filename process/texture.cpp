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

#include "../globals.h"
#include "../io/io.h"
#include "../io/texture-dds.h"

/* ---------------------------------------------------------------------------------------------- */

bool compressimages = false;
bool leavehdrtexts = false;
bool normalmapts = false;
bool colormapgamma = true;
int normalsteepness = 1;

int fixedtexts = 0;
int modifiedtexts = 0;
int planartexts = 0;
int changedformats = 0;

#ifdef	TEXTUREOPT
const char *UsedforDDS(const char *path, int ch) {
  if (stristr(path, "\\menus"))
    return (!ch ? "User-interface element" : "Masking");
  /* normal maps */
  if (issuf(path, "_n") ||
      issuf(path, "_fn") ||
      issuf(path, "_xyz") ||
      issuf(path, "_xyzd")) {
    if (stristr(path, "landscapelod") ||
	stristr(path, "terrain"))
      return (!ch ? "World-space normal-map for terrain" : "None");

    return (!ch ? "Tangent-space normal-map" : "Specularity-map");
  }
  /* normal maps */
  if (issuf(path, "_msn"))
    return (!ch ? "Model-space normal-map" : "Unknown (please report)");
  /* glow maps */
  if (issuf(path, "_g") ||
      issuf(path, "_glow") ||
      issuf(path, "_emit"))
    return (!ch ? "Glow-map" : "None");
  /* hair gloss maps (l/rgb-only, greyscale) */
  if (issuf(path, "_hh"))
    return (!ch ? "Gloss-map for hair" : "None");
  /* hair detail maps (rgba) */
  if (issuf(path, "_hl")) 
    return (!ch ? "Detail-map for hair" : "Opacity");
  /* metallic maps */
  if (issuf(path, "_m"))
    return (!ch ? "Reflectivity-map for light-sources" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_em") ||
      issuf(path, "_envmap"))
    return (!ch ? "Reflectivity-map for environment-maps" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_e"))
    return (!ch ? "Environment-map" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_b") ||
      issuf(path, "_bl"))
    return (!ch ? "Backlight-map" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_s"))
    return (!ch ? "Specularity-map for skins" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_sk"))
    return (!ch ? "Tone-map for skins" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_p"))
    return (!ch ? "Parallax-map" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_d"))
    return (!ch ? "Diffuse-map" : "Unknown (please report)");
  /* environment maps */
  if (issuf(path, "_h"))
    return (!ch ? "Unknown (please report)" : "Unknown (please report)");

  return (!ch ? "Color-map" : "Opacity or parallax-shift");
}

bool ProcessDDS(const char *inname, const char *ouname, const char *rep) {
    /* read the DDS */
    void *inmem = NULL, *oumem = NULL;
    UINT insize =    0, ousize =    0;
    LPD3DXBUFFER oubuf = NULL;
    char buf[1024], *newsuf = NULL;

    /* look what we'v got */
    struct ioinfo info;
    if (iostat(inname, &info)) {
      errprintf(stderr, "can't read the DDS\n");
      return false;
    }

    /* read in the file-contents */
    struct iofile *file;
    if ((file = ioopenfile(inname, "rb"))) {
      inmem = malloc(insize = (UINT)info.io_size);

      UINT rdsze = (UINT)
      ioreadfile(inmem, insize, file);
      ioclosefile(file);

      if (rdsze != insize) {
	free(inmem);
	errprintf(stderr, "can't read the DDS\n");
	return false;
      }
    }
    else {
      errprintf(stderr, "can't read the DDS\n");
      return false;
    }

    /* only if we got something */
    if (inmem && insize) {
      LPDIRECT3DTEXTURE9 base = NULL;
      HRESULT res;

      /* extract levels the hard way (if there is 0 in the file, make it 1) */
      int mipl = 1;
      if (((DDS_HEADER *)inmem)->dwMagicNumber == MAKEFOURCC('D', 'D', 'S', ' '))
	mipl = ((DDS_HEADER *)inmem)->dwMipMapCount;
      if (!mipl)
	mipl = 1;

      /* with depth: cube or volume */
      if (((DDS_HEADER *)inmem)->dwHeaderFlags & 0x00800000U)
	;//addnote(" Texture has unsupported format (cubic or volumetric).\n");
      else if ((res = D3DXCreateTextureFromFileInMemoryEx(
	pD3DDevice, inmem, insize,
	D3DX_DEFAULT, D3DX_DEFAULT, mipl/*D3DX_DEFAULT*/,
	0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
	D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	&base
      )) == D3D_OK) {

//    if ((res = D3DXCreateTextureFromFileInMemory(
//	pD3DDevice, inmem, insize, &base
//    )) == D3D_OK) {

	barprintf(stdout, '#', "%s", rep);
	if (verbose)
	  nfoprintf(stdout, "%s\n", inname);

	/* the lowest mip-level contains a row or a column of 4x4 blocks
	 * we won't generate mip-levels for mips smaller than the BTC-area
	 */
	D3DSURFACE_DESC based;
	base->GetLevelDesc(0, &based);
	int levelc = base->GetLevelCount();
	short depth = findFormatDepth(based.Format);
	char channels = findFormatChannels(based.Format);
	const char *from = findFormat(based.Format);
	bool uncompressed = false, unsupported = false;
	bool istransp = false;
	bool cangamma = !stristr(inname, "faces\\");
	if (!cangamma)
	  addnote(" Turned gamma off for face-modifier texture.\n");

	/* exclude menus (and recognize they are alphas) */
	int levels = 1;
	if (stristr(inname, "\\menus"))
	  istransp = true;
	else {
	  int ww = based.Width;
	  int hh = based.Height;
	  while ((ww > 1) && (hh > 1)) {
	    ww = (ww + 1) >> 1;
	    hh = (hh + 1) >> 1;

	    levels++;
	  }
	}

	if ((based.Format != D3DFMT_DXT1) &&
	    (based.Format != D3DFMT_DXT2) &&
	    (based.Format != D3DFMT_DXT3) &&
	    (based.Format != D3DFMT_DXT4) &&
	    (based.Format != D3DFMT_DXT5) &&
	    (based.Format != D3DFMT_ATI1) &&
	    (based.Format != D3DFMT_ATI2))
	  uncompressed = true;
	if ((based.Format == D3DFMT_V8U8) ||
	    (based.Format == D3DFMT_Q8W8V8U8) ||
	    (based.Format == D3DFMT_V16U16)  ||
	    (based.Format == D3DFMT_Q16W16V16U16) ||
	    (based.Format == D3DFMT_CxV8U8))
	  unsupported = true;

	/**/ if (base->GetType() == D3DRTYPE_VOLUMETEXTURE)
	  unsupported = true, levels = levelc;
	else if (base->GetType() == D3DRTYPE_CUBETEXTURE)
	  unsupported = true, levels = levelc;
	else if (base->GetType() != D3DRTYPE_TEXTURE)
	  unsupported = true;

	/* file-size sanity check */
	if (unsupported)
	  addnote(" Texture has unsupported format.\n");
	else if (levelc > 1) {
	  unsigned int bsize = 0;
	  switch (based.Format) {
	    case D3DFMT_DXT5:
	    case D3DFMT_DXT4:
	    case D3DFMT_DXT3:
	    case D3DFMT_DXT2:
	    case D3DFMT_ATI2:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 3) / 4) *
			 ((((based.Height + (1 << l) - 1) >> l) + 3) / 4) * 16;
	      break;
	    case D3DFMT_DXT1:
	    case D3DFMT_ATI1:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 3) / 4) *
			 ((((based.Height + (1 << l) - 1) >> l) + 3) / 4) * 8;
	      break;
	    case D3DFMT_A4L4:
	    case D3DFMT_A4R4G4B4:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 0) / 1) *
			 ((((based.Height + (1 << l) - 1) >> l) + 0) / 1) * channels >> 1;
	      break;
	    default:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 0) / 1) *
			 ((((based.Height + (1 << l) - 1) >> l) + 0) / 1) * channels;
	      break;
	  }

	  if (bsize > insize)
	    addnote(" Truncated texture found, fixed!\n"), fixedtexts++;
	}

	/* choose what to do */
	int relevel = 0;
	if (levels > levelc)
	  relevel = levelc + 1;
	else if (levels < levelc)
	  relevel = 0 - levels;
	else if (levels == 1)
	  relevel = 0 - levels;

	/* remip it just all again */
	bool remip = false;
	if (optimizetexts || (compressimages && uncompressed))
	  remip = true, relevel = min(0, relevel);
	if (leavehdrtexts && ((depth < -8) || (depth > 8)))
	  remip = false;
	if (unsupported)
	  remip = false;

	/* go ahead, compress */
	bool success = false;
	if (remip) {
	  bool unknown = false;
	  bool dooptcomp = compressimages || !uncompressed;

	  nfoprintf(stdout, "processing:\n");

	  if (dooptcomp) {
	    /* normal maps */
	    if (issuf(inname, "_n") ||
		issuf(inname, "_fn") ||
		issuf(inname, "_xyz") ||
		issuf(inname, "_xyzd")) {
	      /* only known object-space normal-maps */
	      bool ts = normalmapts;
	      if (stristr(inname, "landscapelod\\") || stristr(inname, "terrain\\"))
		ts = false;
	      if (relevel > 0)
		ts = false;

	      /**/ if (issuf(inname, "_xyzd")) {
		if (ts) success = TextureCompressXYZD (&base, relevel);
		else    success = TextureCompress_XYZD(&base, relevel);
	      }
	      else if (issuf(inname, "_xyz")) {
		if (ts) success = TextureCompressXYZ  (&base, relevel);
		else    success = TextureCompress_XYZ (&base, relevel);
	      }
	      else if (channels == 4) {
		if (ts) success = TextureCompressXYZD (&base, relevel);
		else    success = TextureCompress_XYZD(&base, relevel);
	      }
	      else if (channels == 3) {
		if (ts) success = TextureCompressXYZ  (&base, relevel);
		else    success = TextureCompress_XYZ (&base, relevel);
	      }
	      else if (channels == 2) {
		if (ts) success = TextureCompressXY   (&base, relevel);
		else    success = TextureCompressXY   (&base, relevel);
	      }
/*	      else if (channels == 1) {
		if (ts) success = TextureCompressZ    (&base, relevel);
		else    success = TextureCompressZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      newsuf = "_n";
	      if (issuf(inname, "_fn"))
		newsuf = "_fn";
	    }
	    /* model-space normal maps */
	    else if (issuf(inname, "_msn") ||
		     issuf(inname, "_uvw") ||
		     issuf(inname, "_uvwd")) {
	      /**/ if (issuf(inname, "_uvwd"))
		success = TextureCompress_XYZD(&base, relevel);
	      else if (issuf(inname, "_uvw"))
		success = TextureCompress_XYZ (&base, relevel);
	      else if (channels == 4)
		success = TextureCompress_XYZD(&base, relevel);
	      else if (channels == 3)
		success = TextureCompress_XYZ (&base, relevel);
	      else if (channels == 2)
		success = TextureCompressXY   (&base, relevel);
/*	      else if (channels == 1)
		success = TextureCompressZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      newsuf = "_msn";
	    }
	    /* glow maps (l/rgb-only) */
	    else if (issuf(inname, "_g") ||
		     issuf(inname, "_glow") ||
		     issuf(inname, "_emit")) {
	      /**/ if (channels == 4)
		success = TextureCompressRGBH(&base, relevel, false);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, false);
//	      else if (channels == 2)
//	        success = TextureCompressLA  (&base, relevel);
//	      else if (channels == 1)
//	        success = TextureCompressL   (&base, relevel
	      else
		unknown = true;
	    }
	    /* backlight maps (l/rgb-only) */
	    else if (issuf(inname, "_b") ||
		     issuf(inname, "_bl")) {
	      /**/ if (channels == 4)
		success = TextureCompressRGB (&base, relevel, false);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, false);
//	      else if (channels == 2)
//	        success = TextureCompressLA  (&base, relevel);
//	      else if (channels == 1)
//	        success = TextureCompressL   (&base, relevel
		else
		  unknown = true;
	    }
	    /* hair gloss maps (l/rgb-only, greyscale) */
	    else if (issuf(inname, "_hh")) {
	      bool ga = cangamma;

  	      /**/ if (channels == 4)
  		success = TextureCompressRGB (&base, relevel, ga);
  	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* hair detail maps (rgba) */
	    else if (issuf(inname, "_hl")) {
	      bool ga = cangamma;

	      /**/ if (channels == 4)
		success = TextureCompressRGBA(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* color maps */
	    else if (issuf(inname, "_rgb") ||
		     issuf(inname, "_rgba")) {
	      bool ga = cangamma;

	      /**/ if (issuf(inname, "_rgba"))
		success = TextureCompressRGBA(&base, relevel, ga);
	      else if (issuf(inname, "_rgb"))
		success = TextureCompressRGB (&base, relevel, ga);
	      else
		unknown = true;

	      newsuf = "";
	    }
	    /* color maps */
	    else {
	      bool ga = cangamma;

	      /**/ if (channels == 4 && istransp)
		success = TextureCompressRGBA(&base, relevel, ga);
	      else if (channels == 4)
		success = TextureCompressRGBH(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, ga);
//	      else if (channels == 2)
//	        success = TextureCompressLA  (&base, relevel);
//	      else if (channels == 1)
//	        success = TextureCompressL   (&base, relevel);
	      else
		unknown = true;
	    }

	    if (success)
	      modifiedtexts++;
	  }

	  if (!dooptcomp || (!success && unknown)) {
	    /* normal maps */
	    if (issuf(inname, "_n") ||
		issuf(inname, "_fn") ||
		issuf(inname, "_xyz") ||
		issuf(inname, "_xyzd")) {
	      /* only known object-space normal-maps */
	      bool ts = normalmapts;
	      if (stristr(inname, "landscapelod\\") || stristr(inname, "terrain\\"))
		ts = false;
	      if (relevel > 0)
		ts = false;

	      /**/ if (issuf(inname, "_xyzd")) {
		if (ts) success = TextureConvertXYZD (&base, relevel);
		else    success = TextureConvert_XYZD(&base, relevel);
	      }
	      else if (issuf(inname, "_xyz")) {
		if (ts) success = TextureConvertXYZ  (&base, relevel);
		else    success = TextureConvert_XYZ (&base, relevel);
	      }
	      else if (channels == 4) {
		if (ts) success = TextureConvertXYZD (&base, relevel);
		else    success = TextureConvert_XYZD(&base, relevel);
	      }
	      else if (channels == 3) {
		if (ts) success = TextureConvertXYZ  (&base, relevel);
		else    success = TextureConvert_XYZ (&base, relevel);
	      }
	      else if (channels == 2) {
		if (ts) success = TextureConvertXY   (&base, relevel);
		else    success = TextureConvertXY   (&base, relevel);
	      }
/*	      else if (channels == 1) {
		if (ts) success = TextureConvertZ    (&base, relevel);
		else    success = TextureConvertZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      newsuf = "_n";
	      if (issuf(inname, "_fn"))
		newsuf = "_fn";
	    }
	    /* glow maps (l/rgb-only) */
	    else if (issuf(inname, "_g")) {
	      /**/ if (channels == 4)
		success = TextureConvertRGBH(&base, relevel, false);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, false);
	      else if (channels == 2)
		success = TextureConvertLA  (&base, relevel);
	      else if (channels == 1)
		success = TextureConvertL   (&base, relevel);
	      else
		unknown = true;
	    }
	    /* hair gloss maps (l/rgb-only, greyscale) */
	    else if (issuf(inname, "_hh")) {
	      bool ga = cangamma;

	      /**/ if (channels == 4)
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (channels == 2)
		success = TextureConvertLA  (&base, relevel);
	      else if (channels == 1)
		success = TextureConvertL   (&base, relevel);
	      else
		unknown = true;
	    }
	    /* hair detail maps (rgba) */
	    else if (issuf(inname, "_hl")) {
	      bool ga = cangamma;

	      /**/ if (channels == 4)
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* color maps */
	    else if (issuf(inname, "_rgb") ||
		     issuf(inname, "_rgba")) {
	      bool ga = cangamma;

	      /**/ if (issuf(inname, "_rgba"))
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (issuf(inname, "_rgb"))
		success = TextureConvertRGB (&base, relevel, ga);
	      else
		unknown = true;

	      newsuf = "";
	    }
	    /* color maps */
	    else {
	      bool ga = cangamma;

	      /**/ if (channels == 4 && istransp)
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (channels == 4)
		success = TextureConvertRGBH(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (channels == 2)
		success = TextureConvertLA  (&base, relevel);
	      else if (channels == 1)
		success = TextureConvertL   (&base, relevel);
	    }

	    if (success)
	      modifiedtexts++;
	  }

	  if (!success && !unknown)
	    errprintf(stderr, "failed to regenerate DDS\n");
	}
	/* do the mip-levels */
	else if (levels != levelc) {
	  LPDIRECT3DTEXTURE9 replct;
	  LPDIRECT3DSURFACE9 stex, srep;
	  DWORD filter = D3DX_FILTER_BOX;
	  HRESULT res;

	  nfoprintf(stdout, "processing:\n");

	  /* normal maps */
	  if (issuf(inname, "_n") || issuf(inname, "_fn"))
	    ;
	  /* glow maps */
	  else if (issuf(inname, "_g"))
	    ;
	  /* color maps */
	  else if (cangamma)
	    filter |= D3DX_FILTER_DITHER | D3DX_FILTER_SRGB;
	  else
	    filter |= D3DX_FILTER_DITHER;

	  if ((res = pD3DDevice->CreateTexture(based.Width, based.Height, levels, 0, based.Format, D3DPOOL_MANAGED, &replct, NULL)) == D3D_OK) {
	    base->GetSurfaceLevel(0, &stex);
	    replct->GetSurfaceLevel(0, &srep);

	    res = D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, filter, 0);

	    stex->Release();
	    srep->Release();

	    if (res == D3D_OK) {
	      base->Release();
	      base = replct;

	      res = D3DXFilterTexture(base, NULL, 0, filter);
	    }
	    else {
	      replct->Release();
	      replct = base;
	    }
	  }

	  if (!(success = (res == D3D_OK)))
	    errprintf(stderr, "failed to regenerate DDS\n");

	  if (success)
	    modifiedtexts++;
	}

	if (success && (res = D3DXSaveTextureToFileInMemory(&oubuf, D3DXIFF_DDS, base, NULL)) == D3D_OK) {
	  D3DFORMAT oldFormat = based.Format;

	  /* read changed information in */
	  base->GetLevelDesc(0, &based);

	  /* recalculate levels after conversion */
	  int levels = 1; {
	    int ww = based.Width;
	    int hh = based.Height;
	    while ((ww > 1) && (hh > 1)) {
	      ww = (ww + 1) >> 1;
	      hh = (hh + 1) >> 1;

	      levels++;
	    }
	  }

	  if (levels > levelc)
	    addnote(" Missing %d mip-level(s) complemented.\n", levels - levelc);
	  else if (levels < levelc)
	    addnote(" Excess of %d mip-level(s) removed.\n", levelc - levels);

	  if (((based.Format != D3DFMT_DXT1) &&
	       (based.Format != D3DFMT_DXT2) &&
	       (based.Format != D3DFMT_DXT3) &&
	       (based.Format != D3DFMT_DXT4) &&
	       (based.Format != D3DFMT_DXT5) &&
	       (based.Format != D3DFMT_ATI1) &&
	       (based.Format != D3DFMT_ATI2)) && !uncompressed)
	    addnote(" Texture was compressed.\n");
	  if (((based.Format == D3DFMT_DXT1) ||
	       (based.Format == D3DFMT_DXT2) ||
	       (based.Format == D3DFMT_DXT3) ||
	       (based.Format == D3DFMT_DXT4) ||
	       (based.Format == D3DFMT_DXT5) ||
	       (based.Format == D3DFMT_ATI1) ||
	       (based.Format == D3DFMT_ATI2)) && uncompressed)
	    addnote(" Texture was uncompressed.\n");

	  if (oldFormat != based.Format)
	    changedformats++;
	  if ((based.Width == 1) && (based.Height == 1))
	    planartexts++;

	  nfoprintf(stdout, " Format           : %s to %s\n", from, findFormat(based.Format));
	  nfoprintf(stdout, " Dimensions       : %dx%d - %d to %d levels\n", based.Width, based.Height, levelc, levels);

	  if (notes.size() > 0) {
	    nfoprintf(stdout, "\nnotes:\n");
	    for (size_t n = 0; n < notes.size(); n++)
	      nfoprintf(stdout, notes[n].data());
	  }

	  oumem  = oubuf->GetBufferPointer();
	  ousize = oubuf->GetBufferSize();

	  /* mark the file to have been optimized */
	  ((DDS_HEADER *)oumem)->dwReserved1[0] = MAKEFOURCC('N', 'O', '0', '1');
	}

	notes.clear();
	base->Release();
      }
      else
	errprintf(stderr, "can't read the DDS\n");
    }

    /* dummy content (copy over) */
    bool success = true;
    if (!oumem)
      oumem = inmem, ousize = insize;

    if (oumem) {
      /* take the ouname if we have it */
      if (ouname)
	strcpy(buf, ouname);
      else
	strcpy(buf, inname);

      /* we do never overwrite files! append suffix */
      putext(buf, buf, "dds");
      if (newsuf)
	setsuf(buf, buf, newsuf);
      if (!stricmp(inname, buf))
	putsuf(buf, buf, "_opt");
      assert(isext(buf, "dds"));

      /* put out the file-contents */
      if (!simulation) {
	struct iofile *file;

	if ((file = ioopenfile(buf, "wb")))
	  ioflushtofile(oumem, ousize, file);
	else {
	  errprintf(stderr, "can't write the DDS\n"); success = false; }
      }

      /* uh, MicroSoft, no signed "ssize_t"? */
      if (insize)
	processedinbytes += insize, convertedinbytes += insize, deltaiobytes += insize;
      if (ousize)
	processedoubytes += ousize, convertedoubytes += ousize, deltaiobytes -= ousize;
   }

   if (inmem) free(inmem);
   if (oubuf) oubuf->Release();

   return success;
}
#endif
