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

#define	_CRT_SECURE_NO_WARNINGS
#define	_CRT_NONSTDC_NO_DEPRECATE

#include <map>

using namespace std;

#include "../globals.h"
#include "../io/io.h"
#include "../io/texture-dds.h"

/* ---------------------------------------------------------------------------------------------- */

bool omsn, bmsn, cmsn; D3DFORMAT fmsn;
bool on,   bn,   cn;   D3DFORMAT fn;
bool ons,  bns,  cns;  D3DFORMAT fns;
bool oc,   bc,   cc;   D3DFORMAT fc;
bool oca,  bca,  cca;  D3DFORMAT fca;
bool og,   bg,   cg;   D3DFORMAT fg;
bool oga,  bga,  cga;  D3DFORMAT fga;

int lpw, lph;
int lcw, lch;
int lls;
int llc;
int llo;

/* ---------------------------------------------------------------------------------------------- */

bool compressimages = false;
bool leavehdrtexts = false;
bool normalmapts = false;
bool colormapgamma = true;
bool colormapalpha = false;
int normalsteepness = 1;
int ignoreborder = 0;

int fixedtexts = 0;
int modifiedtexts = 0;
int planartexts = 0;
int changedformats = 0;

#ifdef	TEXTUREOPT
const char *UsedforDDS(const char *path, int ch) {
  if (stristr(path, "\\menus"))
    return (!ch ? "User-interface element" : "Masking");
  if (stristr(path, "\\faces"))
    return (!ch ? "Aging-map for faces" : "None");
  if (stristr(path, "\\facemods"))
    return (!ch ? "Aging/tone-map for faces" : "None");

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
    return (!ch ? "Model-space normal-map" : "None");
  /* glow maps */
  if (issuf(path, "_g") ||
      issuf(path, "_glow") ||
      issuf(path, "_emit"))
    return (!ch ? "Glow-color" : "Glow-mask");
  /* hair gloss maps (l/rgb-only, greyscale) */
  if (issuf(path, "_hh"))
    return (!ch ? "Gloss-map for hair" : "None");
  /* hair detail maps (rgba) */
  if (issuf(path, "_hl"))
    return (!ch ? "Detail-map for hair" : "Opacity");
  /* metallic masks */
  if (issuf(path, "_m"))
    return (!ch ? "Reflectivity-map for light-sources (greyscale)" : "None");
  /* environment masks */
  if (issuf(path, "_em") ||
      issuf(path, "_envmap") ||
      issuf(path, "_envmapmask"))
    return (!ch ? "Reflectivity-map for environment-maps (greyscale)" : "None");
  /* environment maps */
  if (issuf(path, "_e"))
    return (!ch ? "Environment-map" : "None");
  /* shine maps */
  if (issuf(path, "_s"))
    return (!ch ? "Specularity-map for skin" : "None");
  /* skin maps */
  if (issuf(path, "_sk"))
    return (!ch ? "Tone-map for flesh/skin" : "None");
  /* backlist maps */
  if (issuf(path, "_b") ||
      issuf(path, "_bl"))
    return (!ch ? "Backlight-map for fur/ice" : "None");
  /* parallax maps */
  if (issuf(path, "_p"))
    return (!ch ? "Parallax-map (greyscale)" : "None");
  /* diffuse maps */
  if (issuf(path, "_d") ||
      issuf(path, "_diff"))
    return (!ch ? "Diffuse-color" : "Opacity");
  /* haze maps */
  if (issuf(path, "_h"))
    return (!ch ? "Haze-pattern" : "Unknown (please report)");
  /* ??? (Oblivion) */
  if (issuf(path, "_a"))
    return (!ch ? "Unknown (please report)" : "Unknown (please report)");

  return (!ch ? "Color-map" : "Opacity or parallax-shift");
}

/* ---------------------------------------------------------------------------------------------- */

map<string, unsigned int> wlist;

void WhitelistDDS(char *mem, unsigned int base) {
  char *cfg, *fnm;

  while (*mem) {
    int next = strlen(mem) + 1;

    /* skip comments */
    if ((*mem != ';') && (*mem != '#')) {
      unsigned int wl = base;

      while ((cfg = strchr(mem, '/')))
	*cfg = '\\';
      if ((cfg = strchr(fnm = mem, '='))) {
	*cfg++ = '\0';

	/**/ if (!stricmp(cfg, "DXT"))
	  wl |= WL_COMPRESS;
	else if (!stricmp(cfg, "BIT"))
	  wl |= WL_BITFIELD;
	else if (!stricmp(cfg, "RAW"))
	  wl |= WL_LEAVE;
      }

      /* move to after last "texture"-fragment */
      char *iptr;
      while ((iptr = stristr(fnm, "textures\\")))
	fnm = iptr + 9;

      /* mark the entry */
      wlist[fnm] |= wl;
    }

    /* next null-terminated string */
    mem += next;
  }
}

void WhitelistDDS(const char *ini) {
  char *mem = (char *)malloc(1024 * 1024);

  /* others are additive */
  if (!ini) {
    wlist.clear();

    WhitelistDDS("./DDSopt.ini");
    /**/ if (gameversion == OB_BSAHEADER_VERSION)
      WhitelistDDS("./DDSopt-Oblivion.ini");
    else if (gameversion == SK_BSAHEADER_VERSION)
      WhitelistDDS("./DDSopt-Skyrim.ini");

    return;
  }

  DWORD
  res = GetPrivateProfileSection("NormalsTangentSpace", mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, WL_NORMALT);
  res = GetPrivateProfileSection("NormalsModelSpace"  , mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, WL_NORMALM);
  res = GetPrivateProfileSection("DiffuseColor"       , mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, WL_COLOR);
  res = GetPrivateProfileSection("DiffuseGrey"        , mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, WL_GREY);
  res = GetPrivateProfileSection("AlphaOpacity"       , mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, WL_TRANSP);
  res = GetPrivateProfileSection("AlphaCustom"        , mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, WL_CUSTOM);
  res = GetPrivateProfileSection("Automatic"          , mem, 1024 * 1024, ini); if (res > 0) WhitelistDDS(mem, 0UL);

  free(mem);
}

unsigned int WhitelistedDDS(const char *inname) {
  if (wlist.size() > 0) {
    /* move to after last "texture"-fragment */
    const char *iptr;
    while ((iptr = stristr(inname, "textures\\")))
      inname = iptr + 9;

    /* check and return */
    if (wlist.count(inname) > 0)
      return wlist[inname];
  }

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

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
	bool uncompressed = false, unsupported = false;
	bool istransp = false;
	bool cangamma = !stristr(inname, "faces\\") && !stristr(inname, "facemods\\");
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
	if ((based.Format == D3DFMT_X8R8G8B8) ||
	    (based.Format == D3DFMT_X1R5G5B5) ||
	    (based.Format == D3DFMT_X4R4G4B4) ||
	    (based.Format == D3DFMT_X8B8G8R8) ||
	    (based.Format == D3DFMT_X8L8V8U8))
	  channels = 3;

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

	/* take a look if it already has been optimized */
	if ((((DDS_HEADER *)inmem)->dwReserved1[0] == MAKEFOURCC('D', 'O', '0', '1')) ||
	    (((DDS_HEADER *)inmem)->dwReserved1[0] == MAKEFOURCC('N', 'O', '0', '1'))) {
	  /* no need to remip/optimize, but's also cool */
	  remip   = false;
	  success = true;
	}

	if (remip) {
	  bool unknown = false;
	  bool dooptcomp = compressimages || !uncompressed;

	  /* it's too late already, and we don't upgrade/elevate */
	  bool cns  = ::cns  || !uncompressed;
	  bool cn   = ::cn   || !uncompressed;
	  bool cmsn = ::cmsn || !uncompressed;
	  bool cca  = ::cca  || !uncompressed;
	  bool cc   = ::cc   || !uncompressed;
	  bool cga  = ::cga  || !uncompressed;
	  bool cg   = ::cg   || !uncompressed;

	  /* it's too late already, and we don't upgrade/elevate */
	  bool bns  = ::bns  &&  uncompressed;
	  bool bn   = ::bn   &&  uncompressed;
	  bool bmsn = ::bmsn &&  uncompressed;
	  bool bca  = ::bca  &&  uncompressed;
	  bool bc   = ::bc   &&  uncompressed;
	  bool bga  = ::bga  &&  uncompressed;
	  bool bg   = ::bg   &&  uncompressed;

	  nfoprintf(stdout, "processing:\n");

	  /* look if we have an individual overwrite here */
	  unsigned int cfg = WhitelistedDDS(inname);

	  if ((cfg & WL_COMPRESS)                 ) cns = cn = cmsn = cca = cc = cga = cg = true;
	  if ((cfg & WL_BITFIELD) &&  uncompressed) bns = bn = bmsn = bca = bc = bga = bg = true;
	  if ((cfg & WL_LEAVE   ) &&  uncompressed) cns = cn = cmsn = cca = cc = cga = cg = false;
	  if ((cfg & WL_LEAVE   ) &&  uncompressed) bns = bn = bmsn = bca = bc = bga = bg = false;

	  if (dooptcomp) {
	    /* normal maps */
	    if (issuf(inname, "_n") ||
		issuf(inname, "_nor") ||
		issuf(inname, "_fn") ||
		issuf(inname, "_xyz") ||
		issuf(inname, "_xyzd") || (cfg & WL_NORMALT)) {
	      /* only known object-space normal-maps */
	      bool ts = normalmapts;
	      if (stristr(inname, "landscapelod\\") || stristr(inname, "terrain\\"))
		ts = false;
	      if (relevel > 0)
		ts = false;

	      /**/ if (cns && issuf(inname, "_xyzd")) {
		if (ts) success = (!bns ? TextureCompressXYZD (&base, relevel) : TextureQuantizeX4Y4Z4D4 (&base, relevel));
		else    success = (!bns ? TextureCompress_XYZD(&base, relevel) : TextureQuantize_X4Y4Z4D4(&base, relevel));
	      }
	      else if (cn  && issuf(inname, "_xyz")) {
		if (ts) success = (!bn  ? TextureCompressXYZ  (&base, relevel) : TextureQuantizeX5Y6Z5  (&base, relevel));
		else    success = (!bn  ? TextureCompress_XYZ (&base, relevel) : TextureQuantize_X5Y6Z5 (&base, relevel));
	      }
	      else if (cns && (channels == 4)) {
		if (ts) success = (!bns ? TextureCompressXYZD (&base, relevel) : TextureQuantizeX4Y4Z4D4 (&base, relevel));
		else    success = (!bns ? TextureCompress_XYZD(&base, relevel) : TextureQuantize_X4Y4Z4D4(&base, relevel));
	      }
	      else if (cn  && (channels == 3)) {
		if (ts) success = (!bn  ? TextureCompressXYZ  (&base, relevel) : TextureQuantizeX5Y6Z5  (&base, relevel));
		else    success = (!bn  ? TextureCompress_XYZ (&base, relevel) : TextureQuantize_X5Y6Z5 (&base, relevel));
	      }
	      else if (cn  && (channels == 2)) {
		if (ts) success = (!bn  ? TextureCompressXY   (&base, relevel) : false);
		else    success = (!bn  ? TextureCompressXY   (&base, relevel) : false);
	      }
/*	      else if (cn  && (channels == 1)) {
		if (ts) success = (!bn  ? TextureCompressZ    (&base, relevel) : false);
		else    success = (!bn  ? TextureCompressZ    (&base, relevel) : false);
	      } */
	      else
		unknown = true;

	      if (issuf(inname, "_xyz") ||
		  issuf(inname, "_xyzd"))
		newsuf = "_n";
	    }
	    /* model-space normal maps */
	    else if (issuf(inname, "_msn") ||
		     issuf(inname, "_uvw") || (cfg & WL_NORMALM)) {
	      /**/ if (cmsn && issuf(inname, "_uvw"))
		success = (!bmsn ? TextureCompress_XYZ (&base, relevel) : TextureQuantize_X5Y6Z5 (&base, relevel));
	      else if (cmsn && (channels == 4))
		success = (!bmsn ? TextureCompress_XYZ (&base, relevel) : TextureQuantize_X5Y6Z5 (&base, relevel));
	      else if (cmsn && (channels == 3))
		success = (!bmsn ? TextureCompress_XYZ (&base, relevel) : TextureQuantize_X5Y6Z5 (&base, relevel));
	      else if (cmsn && (channels == 2))
		success = (!bmsn ? TextureCompressXY   (&base, relevel) : false);
/*	      else if (cmsn && (channels == 1))
		success = (!bmsn ? TextureCompressZ    (&base, relevel) : false);
	      } */
	      else
		unknown = true;

	      if (issuf(inname, "_uvw"))
		newsuf = "_msn";
	    }
	    /* glow maps (l/rgb-only) */
	    else if (issuf(inname, "_g") ||
		     issuf(inname, "_glow") ||
		     issuf(inname, "_emit")) {
	      /* notable exception: ./textures/clutter/falmerglow01_g.dds */
	      /* better exception: ./textures/clutter/woodfires/woodfires01_g.dds */
	      /**/ if (cca && (channels == 4))
		success = (!bca ? TextureCompressRGBH(&base, relevel, false) : TextureQuantizeR4G4B4H4(&base, relevel, false));
	      else if (cc  && (channels == 3))
		success = (!bc  ? TextureCompressRGB (&base, relevel, false) : TextureQuantizeR5G6B5  (&base, relevel, false));
	      else if (cga && (channels == 2))
	        success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4    (&base, relevel, false));
	      else if (cg  && (channels == 1))
	        success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else
		unknown = true;
	    }
	    /* backlight maps (l/rgb-only) */
	    else if (issuf(inname, "_b") ||
		     issuf(inname, "_bl")) {
	      /**/ if (cc && (channels == 4))
		success = (!bc ? TextureCompressRGB (&base, relevel, false) : TextureQuantizeR5G6B5 (&base, relevel, false));
	      else if (cc && (channels == 3))
		success = (!bc ? TextureCompressRGB (&base, relevel, false) : TextureQuantizeR5G6B5 (&base, relevel, false));
	      else if (cg && (channels == 2))
	        success = (!bg ? TextureCompressL   (&base, relevel, false) : false);
	      else if (cg && (channels == 1))
	        success = (!bg ? TextureCompressL   (&base, relevel, false) : false);
	      else
		unknown = true;
	    }
	    /* skin-tone maps (rgb) */
	    else if (issuf(inname, "_sk")) {
	      /**/ if (cc && (channels == 4))
		success = (!bc ? TextureCompressRGB (&base, relevel, false) : TextureQuantizeR5G6B5 (&base, relevel, false));
	      else if (cc && (channels == 3))
		success = (!bc ? TextureCompressRGB (&base, relevel, false) : TextureQuantizeR5G6B5 (&base, relevel, false));
	      else if (cg && (channels == 2))
		success = (!bg ? TextureCompressL   (&base, relevel, false) : false);
	      else if (cg && (channels == 1))
		success = (!bg ? TextureCompressL   (&base, relevel, false) : false);
	      else
		unknown = true;
	    }
	    /* shine maps (grey) */
	    else if (issuf(inname, "_s")) {
	      /* only with alpha: ./textures/actors/character/manekin/manakinbodym_s.dds */
	      /* only with alpha: ./textures/actors/character/manekin/manakinhandm_s.dds */
	      /**/ if (cga && (channels == 4))
		success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4  (&base, relevel, false));
	      else if (cg  && (channels == 3))
		success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else if (cga && (channels == 2))
		success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4  (&base, relevel, false));
	      else if (cg  && (channels == 1))
		success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else
		unknown = true;
	    }
	    /* environment masks (grey) */
	    else if (issuf(inname, "_em") ||
		     issuf(inname, "_envmask") ||
		     issuf(inname, "_envmapmask")) {
	      /* only with alpha: ./textures/armor/iron/shield_em.dds */
	      /**/ if (cga && (channels == 4))
		success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4  (&base, relevel, false));
	      else if (cg  && (channels == 3))
		success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else if (cga && (channels == 2))
		success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4  (&base, relevel, false));
	      else if (cg  && (channels == 1))
		success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else
		unknown = true;
	    }
	    /* light masks (grey) */
	    else if (issuf(inname, "_m")) {
	      /* only with alpha: ./textures/armor/iron/shield_em.dds */
	      /**/ if (cga && (channels == 4))
		success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4  (&base, relevel, false));
	      else if (cg  && (channels == 3))
		success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else if (cga && (channels == 2))
		success = (!bga ? TextureCompressLH  (&base, relevel, false) : TextureQuantizeL4H4  (&base, relevel, false));
	      else if (cg  && (channels == 1))
		success = (!bg  ? TextureCompressL   (&base, relevel, false) : false);
	      else
		unknown = true;
	    }
	    /* hair gloss maps (l/rgb-only, greyscale) */
	    else if (issuf(inname, "_hh")) {
	      bool ga = cangamma;

  	      /**/ if (cc && (channels == 4))
  		success = (!bc ? TextureCompressRGB (&base, relevel, ga) : TextureQuantizeR5G6B5 (&base, relevel, ga));
  	      else if (cc && (channels == 3))
		success = (!bc ? TextureCompressRGB (&base, relevel, ga) : TextureQuantizeR5G6B5 (&base, relevel, ga));
	      else if (cg && (channels == 2))
		success = (!bg ? TextureCompressL   (&base, relevel, ga) : false);
	      else if (cg && (channels == 1))
		success = (!bg ? TextureCompressL   (&base, relevel, ga) : false);
	      else
		unknown = true;
	    }
	    /* hair detail maps (rgba) */
	    else if (issuf(inname, "_hl")) {
	      bool ga = cangamma;

	      /**/ if (cca && (channels == 4))
		success = (!bca ? TextureCompressRGBA(&base, relevel, ga) : TextureQuantizeR4G4B4A4(&base, relevel, ga));
	      else if (cc  && (channels == 3))
		success = (!bc  ? TextureCompressRGB (&base, relevel, ga) : TextureQuantizeR5G6B5  (&base, relevel, ga));
	      else if (cga && (channels == 2))
		success = (!bga ? TextureCompressLA  (&base, relevel, ga) : TextureQuantizeL4A4    (&base, relevel, ga));
	      else if (cg  && (channels == 1))
		success = (!bg  ? TextureCompressL   (&base, relevel, ga) : false);
	      else
		unknown = true;
	    }
	    /* color maps */
	    else if (issuf(inname, "_rgb") ||
		     issuf(inname, "_rgba")) {
	      bool ga = cangamma;

	      /**/ if (cca && issuf(inname, "_rgba"))
		success = (!bca ? TextureCompressRGBA(&base, relevel, ga) : TextureQuantizeR4G4B4A4(&base, relevel, ga));
	      else if (cc  && issuf(inname, "_rgb"))
		success = (!bc  ? TextureCompressRGB (&base, relevel, ga) : TextureQuantizeR5G6B5  (&base, relevel, ga));
	      else
		unknown = true;

	      newsuf = "";
	    }
	    /* color maps */
	    else {
	      bool ga = cangamma;

	      /**/ if (cca && (channels == 4) && (istransp || colormapalpha))
		success = (!bca ? TextureCompressRGBA(&base, relevel, ga) : TextureQuantizeR4G4B4A4(&base, relevel, ga));
	      else if (cca && (channels == 4))
		success = (!bca ? TextureCompressRGBH(&base, relevel, ga) : TextureQuantizeR4G4B4H4(&base, relevel, ga));
	      else if (cc  && (channels == 3))
		success = (!bc  ? TextureCompressRGB (&base, relevel, ga) : TextureQuantizeR5G6B5  (&base, relevel, ga));
	      else if (cga && (channels == 2) && (istransp || colormapalpha))
		success = (!bga ? TextureCompressLA  (&base, relevel, ga) : TextureQuantizeL4A4    (&base, relevel, ga));
	      else if (cga && (channels == 2))
		success = (!bga ? TextureCompressLH  (&base, relevel, ga) : TextureQuantizeL4H4    (&base, relevel, ga));
	      else if (cg  && (channels == 1))
	        success = (!bg  ? TextureCompressL   (&base, relevel, ga) : false);
	      else
		unknown = true;
	    }

	    if (success)
	      modifiedtexts++;
	  }

	  if (!dooptcomp || (!success && unknown)) {
	    /* normal maps */
	    if (issuf(inname, "_n") ||
		issuf(inname, "_nor") ||
		issuf(inname, "_fn") ||
		issuf(inname, "_xyz") ||
		issuf(inname, "_xyzd") || (cfg & WL_NORMALT)) {
	      /* only known object-space normal-maps */
	      bool ts = normalmapts;
	      if (stristr(inname, "landscapelod\\") || stristr(inname, "terrain\\"))
		ts = false;
	      if (relevel > 0)
		ts = false;

	      /**/ if (ons && issuf(inname, "_xyzd")) {
		if (ts) success = TextureConvertXYZD (&base, relevel);
		else    success = TextureConvert_XYZD(&base, relevel);
	      }
	      else if (on  && issuf(inname, "_xyz")) {
		if (ts) success = TextureConvertXYZ  (&base, relevel);
		else    success = TextureConvert_XYZ (&base, relevel);
	      }
	      else if (ons && (channels == 4)) {
		if (ts) success = TextureConvertXYZD (&base, relevel);
		else    success = TextureConvert_XYZD(&base, relevel);
	      }
	      else if (on  && (channels == 3)) {
		if (ts) success = TextureConvertXYZ  (&base, relevel);
		else    success = TextureConvert_XYZ (&base, relevel);
	      }
	      else if (on  && (channels == 2)) {
		if (ts) success = TextureConvertXY   (&base, relevel);
		else    success = TextureConvertXY   (&base, relevel);
	      }
/*	      else if (on  && (channels == 1)) {
		if (ts) success = TextureConvertZ    (&base, relevel);
		else    success = TextureConvertZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      if (issuf(inname, "_xyz") ||
		  issuf(inname, "_xyzd"))
		newsuf = "_n";
	    }
	    /* model-space normal maps */
	    else if (issuf(inname, "_msn") ||
		     issuf(inname, "_uvw") || (cfg & WL_NORMALM)) {
	      /**/ if (omsn && issuf(inname, "_uvw"))
		success = TextureConvert_XYZ (&base, relevel);
	      else if (omsn && (channels == 4))
		success = TextureConvert_XYZ (&base, relevel);
	      else if (omsn && (channels == 3))
		success = TextureConvert_XYZ (&base, relevel);
	      else if (omsn && (channels == 2))
		success = TextureConvertXY   (&base, relevel);
/*	      else if (omsn && (channels == 1))
		success = TextureConvertZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      if (issuf(inname, "_uvw"))
		newsuf = "_msn";
	    }
	    /* glow maps (l/rgb-only) */
	    else if (issuf(inname, "_g") ||
		     issuf(inname, "_glow") ||
		     issuf(inname, "_emit")) {
	      /* notable exception: ./textures/clutter/falmerglow01_g.dds */
	      /* better exception: ./textures/clutter/woodfires/woodfires01_g.dds */
	      /**/ if (oca && (channels == 4))
		success = TextureConvertRGBH(&base, relevel, false);
	      else if (oc  && (channels == 3))
		success = TextureConvertRGB (&base, relevel, false);
	      else if (oga && (channels == 2))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 1))
		success = TextureConvertL   (&base, relevel, false);
	      else
		unknown = true;
	    }
	    /* backlight maps (l/rgb-only) */
	    else if (issuf(inname, "_b") ||
		     issuf(inname, "_bl")) {
	      /**/ if (oc && (channels == 4))
		success = TextureConvertRGB (&base, relevel, false);
	      else if (oc && (channels == 3))
		success = TextureConvertRGB (&base, relevel, false);
	      else if (og && (channels == 2))
		success = TextureConvertL   (&base, relevel, false);
	      else if (og && (channels == 1))
		success = TextureConvertL   (&base, relevel, false);
	      else
		unknown = true;
	    }
	    /* skin-tone maps (rgb) */
	    else if (issuf(inname, "_sk")) {
	      /**/ if (oc && (channels == 4))
		success = TextureConvertRGB (&base, relevel, false);
	      else if (oc && (channels == 3))
		success = TextureConvertRGB (&base, relevel, false);
	      else if (og && (channels == 2))
		success = TextureConvertL   (&base, relevel, false);
	      else if (og && (channels == 1))
		success = TextureConvertL   (&base, relevel, false);
	      else
		unknown = true;
	    }
	    /* shine maps (grey) */
	    else if (issuf(inname, "_s")) {
	      /* only with alpha: ./textures/actors/character/manekin/manakinbodym_s.dds */
	      /* only with alpha: ./textures/actors/character/manekin/manakinhandm_s.dds */
	      /**/ if (oga && (channels == 4))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 3))
		success = TextureConvertL   (&base, relevel, false);
	      else if (oga && (channels == 2))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 1))
		success = TextureConvertL   (&base, relevel, false);
	      else
		unknown = true;
	    }
	    /* environment masks (grey) */
	    else if (issuf(inname, "_em") ||
		     issuf(inname, "_envmask") ||
		     issuf(inname, "_envmapmask")) {
	      /* only with alpha: ./textures/armor/iron/shield_em.dds */
	      /**/ if (oga && (channels == 4))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 3))
		success = TextureConvertL   (&base, relevel, false);
	      else if (oga && (channels == 2))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 1))
		success = TextureConvertL   (&base, relevel, false);
	      else
		unknown = true;
	    }
	    /* light masks (grey) */
	    else if (issuf(inname, "_m")) {
	      /* only with alpha: ./textures/armor/iron/shield_em.dds */
	      /**/ if (oga && (channels == 4))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 3))
		success = TextureConvertL   (&base, relevel, false);
	      else if (oga && (channels == 2))
		success = TextureConvertLH  (&base, relevel, false);
	      else if (og  && (channels == 1))
		success = TextureConvertL   (&base, relevel, false);
	      else
		unknown = true;
	    }
	    /* hair gloss maps (l/rgb-only, greyscale) */
	    else if (issuf(inname, "_hh")) {
	      bool ga = cangamma;

	      /**/ if (oc && (channels == 4))
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (oc && (channels == 3))
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (og && (channels == 2))
		success = TextureConvertL   (&base, relevel, ga);
	      else if (og && (channels == 1))
		success = TextureConvertL   (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* hair detail maps (rgba) */
	    else if (issuf(inname, "_hl")) {
	      bool ga = cangamma;

	      /**/ if (oca && (channels == 4))
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (oc  && (channels == 3))
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (oga && (channels == 2))
		success = TextureConvertLA  (&base, relevel, ga);
	      else if (og  && (channels == 1))
		success = TextureConvertL   (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* color maps */
	    else if (issuf(inname, "_rgb") ||
		     issuf(inname, "_rgba")) {
	      bool ga = cangamma;

	      /**/ if (oca && issuf(inname, "_rgba"))
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (oc  && issuf(inname, "_rgb"))
		success = TextureConvertRGB (&base, relevel, ga);
	      else
		unknown = true;

	      newsuf = "";
	    }
	    /* color maps */
	    else {
	      bool ga = cangamma;

	      /**/ if (oca && (channels == 4) && (istransp || colormapalpha))
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (oca && (channels == 4))
		success = TextureConvertRGBH(&base, relevel, ga);
	      else if (oc  && (channels == 3))
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (oga && (channels == 2) && (istransp || colormapalpha))
		success = TextureConvertLA  (&base, relevel, ga);
	      else if (oga && (channels == 2))
		success = TextureConvertLH  (&base, relevel, ga);
	      else if (og  && (channels == 1))
		success = TextureConvertL   (&base, relevel, ga);
	      else
		unknown = true;
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

	/* check for resolution-overflow and extract lower mip-levels */
	if (1) {
	  D3DSURFACE_DESC scheck;
	  bool limit; int lw, lh;
	  int limitw, limith;

	  base->GetLevelDesc(0, &scheck);

	  if ((scheck.Format != D3DFMT_DXT1) &&
	      (scheck.Format != D3DFMT_DXT2) &&
	      (scheck.Format != D3DFMT_DXT3) &&
	      (scheck.Format != D3DFMT_DXT4) &&
	      (scheck.Format != D3DFMT_DXT5) &&
	      (scheck.Format != D3DFMT_ATI1) &&
	      (scheck.Format != D3DFMT_ATI2))
	    lw = lpw, lh = lph;
	  else
	    lw = lcw, lh = lch;

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
	      errprintf(stderr, "couldn't downscale the DDS\n");
	      success = success;
	    }
	    else {
	      addnote(" Resolution has been surpassed and a lower mip-chain selected.\n");
	      success = true;
	    }
	  }
	}

	/* "success" tells us if the contents have been changed (and we need to write something) */
	if (success && (res = D3DXSaveTextureToFileInMemory(&oubuf, D3DXIFF_DDS, base, NULL)) == D3D_OK) {
	  D3DFORMAT oldFormat = based.Format;
	  const int oldWidth  = based.Width;
	  const int oldHeight = based.Height;

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

	  /**/ if (levels > levelc)
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
	  if ((((DDS_HEADER *)inmem)->dwReserved1[0] == MAKEFOURCC('D', 'O', '0', '1')) ||
	      (((DDS_HEADER *)inmem)->dwReserved1[0] == MAKEFOURCC('N', 'O', '0', '1')))
	    addnote(" Texture was optimized.\n");

	  if (oldFormat != based.Format)
	    changedformats++;
	  if ((based.Width == 1) && (based.Height == 1))
	    planartexts++;

	  /* print out format-changes */
	  if (oldFormat != based.Format) {
	    nfoprintf(stdout, " Format           : %s to %s\n", findFormat(oldFormat), findFormat(based.Format)); }
	  else {
	    nfoprintf(stdout, " Format           : %s\n", findFormat(oldFormat)); }

	  /* print out resolution-changes */
	  if ((oldWidth != based.Width) || (oldHeight != based.Height)) {
	    nfoprintf(stdout, " Dimensions       : %dx%d to %dx%d - %d to %d levels\n", oldWidth, oldHeight, based.Width, based.Height, levelc, levels); }
	  else if (levelc != levels) {
	    nfoprintf(stdout, " Dimensions       : %dx%d - %d to %d levels\n", based.Width, based.Height, levelc, levels); }
	  else {
	    nfoprintf(stdout, " Dimensions       : %dx%d - %d levels\n", based.Width, based.Height, levels); }

	  /* print out pending notes */
	  if (notes.size() > 0) {
	    nfoprintf(stdout, "\nnotes:\n");
	    for (size_t n = 0; n < notes.size(); n++)
	      nfoprintf(stdout, notes[n].data());
	  }

	  oumem  = oubuf->GetBufferPointer();
	  ousize = oubuf->GetBufferSize();

	  /* mark the file to have been optimized */
	  ((DDS_HEADER *)oumem)->dwReserved1[0] = MAKEFOURCC('D', 'O', '0', '1');
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
