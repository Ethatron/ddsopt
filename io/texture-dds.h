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
 * The Original Code is the Oblivion Graphics Extender, short OBGE.
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

#include <d3d9.h>
#include <d3dx9.h>

extern IDirect3D9 *pD3D;
extern IDirect3DDevice9 *pD3DDevice;

struct DDS_PIXELFORMAT {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwFourCC;
  DWORD dwRGBBitCount;
  DWORD dwRBitMask;
  DWORD dwGBitMask;
  DWORD dwBBitMask;
  DWORD dwABitMask;
};

typedef struct {
  DWORD dwMagicNumber;

  DWORD dwSize;
  DWORD dwHeaderFlags;
  DWORD dwHeight;
  DWORD dwWidth;
  DWORD dwPitchOrLinearSize;
  DWORD dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
  DWORD dwMipMapCount;
  DWORD dwReserved1[11];
  DDS_PIXELFORMAT ddspf;
  DWORD dwSurfaceFlags;
  DWORD dwCubemapFlags;
  DWORD dwReserved2[3];
} DDS_HEADER;

#define D3DFMT_ATI1 (D3DFORMAT)MAKEFOURCC('A','T','I','1')
#define D3DFMT_ATI2 (D3DFORMAT)MAKEFOURCC('A','T','I','2')

bool TextureInit();
void TextureCleanup();
bool TextureDownMip(int w, int h, LPDIRECT3DTEXTURE9 *tex);
bool TextureConvert(D3DSURFACE_DESC &info, LPDIRECT3DTEXTURE9 *tex, bool black);

bool TextureQuantizeR5G5B5V1(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeR5G5B5A1(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeR5G5B5H1(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeR4G4B4V4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeR4G4B4A4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeR4G4B4H4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeR5G6B5(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeL4A4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeL4H4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertL4H4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureQuantizeX4Y4Z4V4(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureQuantize_X4Y4Z4V4(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureQuantizeX4Y4Z4D4(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureQuantize_X4Y4Z4D4(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureQuantizeX5Y6Z5(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureQuantizeX5Z6Y5(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureQuantize_X5Y6Z5(LPDIRECT3DTEXTURE9 *norm, int minlevel);

bool TextureConvertRGBV(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertRGBA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertRGBH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertRGB(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertLA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertLH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureConvertL(LPDIRECT3DTEXTURE9 *lumi, int minlevel, bool gamma);
bool TextureConvertA(LPDIRECT3DTEXTURE9 *alpha, int minlevel);
bool TextureConvertXYZV(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvert_XYZV(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvertXYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvert_XYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvertXY_Z(LPDIRECT3DTEXTURE9 *norm, LPDIRECT3DTEXTURE9 *z, int minlevel);
bool TextureConvertXYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvert_XYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvertXY(LPDIRECT3DTEXTURE9 *norm, int minlevel);

bool TextureCompressRGBA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureCompressRGBH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureCompressRGB(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureCompressLA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureCompressLH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureCompressL(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma);
bool TextureCompressA(LPDIRECT3DTEXTURE9 *alpha, int minlevel);
bool TextureCompressXYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompress_XYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompressXY_Z(LPDIRECT3DTEXTURE9 *norm, LPDIRECT3DTEXTURE9 *z, int minlevel);
bool TextureCompressXYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompress_XYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompressXY(LPDIRECT3DTEXTURE9 *norm, int minlevel);

bool TextureCompressPM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel, bool gamma);
bool TextureCompressQDM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel, bool gamma, bool LODed);

const char *findFormat(D3DFORMAT fmt);
short findFormatDepth(D3DFORMAT fmt);
char findFormatChannels(D3DFORMAT fmt);
bool findAlpha(D3DFORMAT fmt);
const char *findType(D3DRESOURCETYPE typ);
const char *findFileformat(D3DXIMAGE_FILEFORMAT fmt);
