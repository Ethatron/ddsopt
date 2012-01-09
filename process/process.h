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

extern bool compressimages;
extern bool leavehdrtexts;
extern bool normalmapts;
extern bool colormapgamma;
extern int normalsteepness;

extern int fixedtexts;
extern int modifiedtexts;
extern int planartexts;
extern int changedformats;

const char *UsedforDDS(const char *path, int ch);
bool ProcessDDS(const char *inname, const char *ouname, const char *rep);
