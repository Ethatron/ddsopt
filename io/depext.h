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
 * The Original Code is BSAopt.
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

// ######################## "libdirent\dirent.h"

#ifndef DIRENT_H
#define DIRENT_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dirent {
  unsigned char d_namlen;
  char d_name[256];
};

/* Definition of DIR requires many other headers; not included here to
   avoid namespace pollution. */
typedef struct {
  int num_read;
  char *name;

  WIN32_FIND_DATA ff;
  HANDLE ffhandle;

  struct dirent de;
} DIR;

/*
int closedir(DIR *dirp);
DIR *opendir(const char *_dirname);
struct dirent *readdir(DIR *_dirp);
void rewinddir(DIR *_dirp);

void seekdir(DIR *_dir, long _loc);
long telldir(DIR *_dir);
*/

#ifdef __cplusplus
}
#endif

#endif /* !DIRENT_H */
