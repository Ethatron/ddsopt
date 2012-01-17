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

#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include <map>

using namespace std;

extern vector<string> notes;
extern vector<string> errrs;
extern vector<string> zerobytes;
extern map<string, string> duplicates;
extern map<string, int> texmode;

/* ---------------------------------------------------------------------------------------------- */

#include <stdlib.h>

extern size_t processedinbytes;
extern size_t processedoubytes;
extern size_t compressedinbytes;
extern size_t compresseddtbytes;
extern size_t compressedoubytes;
extern size_t convertedinbytes;
extern size_t convertedoubytes;
extern ptrdiff_t deltaiobytes;
extern int virtualbsafiles;
extern int virtualbsabytes;

/* ---------------------------------------------------------------------------------------------- */

#include <d3d9.h>
#include <d3dx9.h>

extern bool omsn, bmsn, cmsn; extern D3DFORMAT fmsn;
extern bool on,   bn,   cn;   extern D3DFORMAT fn;
extern bool ons,  bns,  cns;  extern D3DFORMAT fns;
extern bool oc,   bc,   cc;   extern D3DFORMAT fc;
extern bool oca,  bca,  cca;  extern D3DFORMAT fca;
extern bool og,   bg,   cg;   extern D3DFORMAT fg;
extern bool oga,  bga,  cga;  extern D3DFORMAT fga;

extern int lpw, lph;
extern int lcw, lch;
extern int lls;
extern int llc;
extern int llo;

/* ---------------------------------------------------------------------------------------------- */

extern bool compressimages;
extern bool leavehdrtexts;
extern bool normalmapts;
extern bool colormapgamma;
extern bool colormapalpha;
extern int normalsteepness;
extern int ignoreborder;

extern int fixedtexts;
extern int modifiedtexts;
extern int planartexts;
extern int changedformats;

const char *UsedforDDS(const char *path, int ch);
bool ProcessDDS(const char *inname, const char *ouname, const char *rep);

/* ---------------------------------------------------------------------------------------------- */

#define WL_NORMALT	(1UL << 0)  // consider it's a tangent-space normal
#define WL_NORMALM	(1UL << 1)  // consider it's a model-space normal
#define WL_COLOR	(1UL << 2)  // consider it's a color-map
#define WL_GREY		(1UL << 3)  // consider it's a greyscale-map
#define WL_TRANSP	(1UL << 4)  // consider alpha is transparency
#define WL_CUSTOM	(1UL << 5)  // consider alpha is not alpha
#define WL_COMPRESS	(1UL << 6)  // compress to DXT
#define WL_BITFIELD	(1UL << 7)  // compress to 565/4444 etc.
#define WL_LEAVE	(1UL << 8)  // compress to 565/4444 etc.

void WhitelistDDS(const char *ini = NULL);
unsigned int WhitelistedDDS(const char *inname);

#endif
