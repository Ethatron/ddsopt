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

#ifndef IO_H
#define IO_H

#define	_CRT_SECURE_NO_WARNINGS
#define	_CRT_NONSTDC_NO_DEPRECATE

#include <string>

using namespace std;

#include <sys/stat.h>
#include <sys/utime.h>
#include <sys/types.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "depext.h"

/* ---------------------------------------------------- */

#define IO_DIRECTORY  1
#define IO_FILE	      2
#define IO_PATTERN    3	/* TODO */
#define IO_ARCHIVE    IO_DIRECTORY

struct ioinfo {
  unsigned short io_type;
  size_t io_size;
  time_t io_time;
};

struct iodirent {
  int namelength;
  char name[1024];
};

struct iodir {
  DIR  *od;
  void *ad;

  struct iodirent oe;
};

struct iofile {
  FILE *of;
  void *af;
};

/* ---------------------------------------------------- */


void iorethrow(const char *what);
void ioblock();
void iorelease();
void ioinit();
void ioappend(HANDLE thread);
void ioflush();
void ioexit();
void iodispatch(DWORD (__stdcall * _dispatchBase)(LPVOID lp), LPVOID _dispatchStruct);

/* ------------------------------------------------------------ */

bool isext(const char *pathname, const char *ext);
char *getext(char *result, const char *pathname);
char *putext(char *result, const char *pathname, const char *ext);
bool isext(const string &pathname, const char *ext);

string &getext(string &result, const string &pathname);
string &putext(string &result, const string &pathname, const char *ext);

/* ------------------------------------------------------------ */

bool issuf(const char *pathname, const char *suffix);
char *setsuf(char *result, const char *pathname, const char *suffix);
char *putsuf(char *result, const char *pathname, const char *suffix);

/* ------------------------------------------------------------ */

int iostat(const char *pathname, struct ioinfo *info);
int iotime(const char *pathname, struct ioinfo *info);
int iotime(const char *takename, const char *pathname);
time_t iotime(const char *pathname);
size_t iosize(const char *pathname);

/* ------------------------------------------------------------ */

void iocp(const char *inname, const char *ouname);

/* ------------------------------------------------------------ */

int mkdir_rec(const char *pathname);
int iomkdir(const char *pathname);
int iormdir(const char *pathname);
struct iodir *ioopendir(const char *pathname);
struct iodirent *ioreaddir(struct iodir *dir);
void ioclosedir(struct iodir *dir);

/* ------------------------------------------------------------ */

struct iodir *ioopenpattern(const char *pathname);
struct iodirent *ioreadpattern(struct iodir *dir);
void ioclosepattern(struct iodir *dir);

/* ------------------------------------------------------------ */

struct iofile *ioopenfile(const char *pathname, const char *mode);
size_t ioreadfile(void *block, size_t size, struct iofile *file);
char iogetc(struct iofile *file);
size_t iowritefile(const void *block, size_t size, struct iofile *file);
int ioputs(const char *str, struct iofile *file);
void ioclosefile(struct iofile *file);
int ioeof(struct iofile *file);
int ioseek(struct iofile *file, long offset, int origin);
int ioseek(struct iofile *file, long long offset, int origin);
int iorewind(struct iofile *file);
int iostat(struct iofile *file, struct ioinfo *info);
int ioflush(struct iofile *file);
long int iotell(struct iofile *file);
int ioerror(struct iofile *file);
void ioclearerr(struct iofile *file);
void ioflushtofile(const void *block, size_t size, struct iofile *file);

/* ------------------------------------------------------------ */

#include <string>
#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>

using namespace std;

istream *ioopenistream(const char *pathname);
ostream *ioopenostream(const char *pathname);
void iocloseistream(istream *ist);
void iocloseostream(ostream *ost);
long iotellistream(istream *ist);
long iotellostream(ostream *ost);

#endif
