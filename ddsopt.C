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

#include <string.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>

using namespace std;

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers

#ifdef _DEBUG
#include <fcntl.h>
#include <io.h>

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole() {
  int hConHandle;
  long lStdHandle;
  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  FILE *fp;

  // allocate a console for this app
  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

  coninfo.dwSize.Y = MAX_CONSOLE_LINES;

  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  // redirect unbuffered STDOUT to the console
  lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

  fp = _fdopen( hConHandle, "w" );
  *stdout = *fp;

  setvbuf( stdout, NULL, _IONBF, 0 );

  // redirect unbuffered STDIN to the console
  lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

  fp = _fdopen( hConHandle, "r" );
  *stdin = *fp;

  setvbuf( stdin, NULL, _IONBF, 0 );

  // redirect unbuffered STDERR to the console
  lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

  fp = _fdopen( hConHandle, "w" );
  *stderr = *fp;

  setvbuf( stderr, NULL, _IONBF, 0 );

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
  // point to console as well
  ios::sync_with_stdio();
}
#endif

BOOL CreateDirectoryRecursive(const char *pathname) {
  size_t len, l;
  char buf[1024];
  BOOL ret = FALSE;

  len = strlen(pathname);
  strcpy(buf, pathname);

  /* iterate down */
  while (!(ret = CreateDirectory(buf, NULL))) {
    char *sep = strrchr(buf, '\\');
    if (!sep)
      break;
    *sep = '\0';
  }

  /* back up again */
  while ((l = strlen(buf)) != len) {
    char *sep = buf + l;
    *sep = '\\';
    if (!(ret = CreateDirectory(buf, NULL)))
      break;
  }

  return ret;
}

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "DDSopt.h"

bool TextureInit();
void TextureCleanup();

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class DDSoptApp : public wxApp
{
  DDSoptGUI *frame;

public:
  // override base class virtuals
  // ----------------------------

  // this one is called on application startup and is a good place for the app
  // initialization (doing it here and not in the ctor allows to have an error
  // return: if OnInit() returns false, the application terminates)
  virtual bool OnInit();
  virtual int OnExit();
};

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. DDSoptApp and
// not wxApp)
IMPLEMENT_APP_NO_MAIN(DDSoptApp)

extern "C" int WINAPI WinMain(HINSTANCE hInstance,
			      HINSTANCE hPrevInstance,
			      wxCmdLineArgType lpCmdLine,
			      int nCmdShow)
{
  int ret = 0;

#ifdef _DEBUG
  RedirectIOToConsole();
#endif

  {
    // break the command line in words
    char *cmdLine = strdup(lpCmdLine), *cL, *cN;
    vector<char *> argv; int argc = 0; cL = cmdLine;

    // argv[] must begin with the command itself
    char PPath[256] = "";
    char CPath[256] = "";
    GetModuleFileName(0, PPath, sizeof(PPath) - 1);
    GetCurrentDirectory(sizeof(CPath) - 1, CPath);

    argv.push_back(PPath);

    do {
      if (cL[0] == ' ')
	cL += 1;
      if (cL[0] == '\0')
	break;

      if (cL[0] == '"') {
	cL[0] = '\0', cL += 1;
	if ((cN = strchr(cL + 0, '"')))
	  cN[0] = '\0', cN += 1;
      }
      else {
	if ((cN = strchr(cL + 0, ' ')))
	  cN[0] = '\0', cN += 1;
      }

      argv.push_back(cL);
    } while ((cL = cN));

    // argv[] must be NULL-terminated
    argc = argv.size();
    argv.push_back(NULL);

    if ((argc > 1) && parseCommandline(argc, &argv[0]))
      ret = main(argc, &argv[0]);
    else {
      ioinit();
      TextureInit();

      ret = wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

      TextureCleanup();
      ioexit();
    }

    free(cmdLine);
  }

  return ret;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool DDSoptApp::OnInit()
{
  // call the base class initialization method, currently it only parses a
  // few common command-line options but it could be do more in the future
  if ( !wxApp::OnInit() )
    return false;

  // create the main application window
  frame = new DDSoptGUI(_T("DDSopt"));

  // and show it (the frames, unlike simple controls, are not shown when
  // created initially)
  frame->SetIcon(wxICON(IDI_MAIN_ICON));
  frame->Show(true);
//frame->Refresh();
//frame->Update();
//frame->SetCommandLine(cmdIn, cmdOu);
  frame->DirectoryFromFiles(0);

  // success: wxApp::OnRun() will be called which will enter the main message
  // loop and the application will run. If we returned false here, the
  // application would exit immediately.
  return true;
}

int DDSoptApp::OnExit()
{
  return wxApp::OnExit();
}

/* --------------------------------------------------------------
 */

void InitProgress(const char *patterna, const char *patternb, int dne, int rng) {
  if (prg)
    prg->InitProgress(patterna, patternb, dne, rng);
}

void SetProgress(const char *str, int dne) {
  if (prg)
    prg->SetProgress(str, dne);
}

void SetProgress(size_t din, size_t dou) {
  if (prg)
    prg->SetProgress(din, dou);
}

void SetTopic(const char *topic) {
  if (prg)
    prg->SetTopic(topic);
}

void SetReport(const char *status, size_t din, size_t dou) {
  if (prg)
    prg->SetReport(status, din, dou);
}

void SetReport(const char *status, size_t din, size_t dou, int dpl) {
  if (prg)
    prg->SetReport(status, din, dou, dpl);
}

bool RequestFeedback(const char *question) {
  if (gui)
    return gui->RequestFeedback(question);
  return false;
}

DWORD __stdcall ProcessingStart(LPVOID lp) {
  if (gui)
    gui->ProcessingStart();
  return 0;
}
