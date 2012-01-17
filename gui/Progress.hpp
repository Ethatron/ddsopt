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

#ifndef DDSOPT_PROGRESS_HPP
#define DDSOPT_PROGRESS_HPP

#include "../DDSopt.h"

// ----------------------------------------------------------------------------
class DDSoptPrg; extern class DDSoptPrg *prg;
class DDSoptPrg : public wxProgress {

private:
  wxEventType evtProgress;
  int idProgress;

  class ProgressEvent: public wxCommandEvent {
  public:
    ProgressEvent(int id, const wxEventType& event_type) : wxCommandEvent(event_type, id) { memset(&state, 0, sizeof(state)); }
    ProgressEvent(const ProgressEvent& event) : wxCommandEvent(event) { memcpy(&state, &event.state, sizeof(state)); }

    wxEvent* Clone() const { return new ProgressEvent(*this); }

  public:
    struct {
      unsigned int mask;

      int taskR; int taskV;
      int effcR; int effcV;
      char subjS[256];
      char reptS[256];
    } state;

    void SetTaskRange(int r) { state.mask |= 1; state.taskR = r; }
    void SetTaskValue(int v) { state.mask |= 2; state.taskV = v; }
    void SetEffcRange(int r) { state.mask |= 4; state.effcR = r; }
    void SetEffcValue(int v) { state.mask |= 8; state.effcV = v; }
    void SetSubject(const char *s) { state.mask |= 16; strcpy(state.subjS, s); }
    void SetReport(const char *s) { state.mask |= 32; strcpy(state.reptS, s); }
  };

  typedef void (wxEvtHandler::*ProgressEventFunction)(ProgressEvent &);

  /* called from Progress-thread */
  void Progress(ProgressEvent &evt) {
    if (evt.state.mask &  1) DOTask->SetRange(evt.state.taskR);
    if (evt.state.mask &  2) DOTask->SetValue(evt.state.taskV);
    if (evt.state.mask &  4) DOEfficiency->SetRange(evt.state.effcR);
    if (evt.state.mask &  8) DOEfficiency->SetValue(evt.state.effcV);
    if (evt.state.mask & 16) DOSubject->SetLabel(evt.state.subjS);
    if (evt.state.mask & 32) DOReport->SetLabel(evt.state.reptS);
  }

  const char *lastpa;
  const char *lastpb;

public:
  /* all executed by Async-thread */
  void StartProgress(int rng) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetTaskRange(rng);

    wxPostEvent(this, event);
  }

  void InitProgress(const char *patterna, const char *patternb, int dne, int rng) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    char tmp[256];

    if (patterna) { sprintf(tmp, lastpa = patterna, "..."); event.SetSubject(tmp); }
    if (patternb) { sprintf(tmp, lastpb = patternb, "..."); event.SetReport(tmp); }

    event.SetTaskValue(dne);
    event.SetEffcRange(rng);
    event.SetEffcValue(rng);

    wxPostEvent(this, event);
  }

  int range1, value1;
  int range2, value2;

  void SetProgress(const char *str, int dne) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    char tmp[256];

    if (lastpa) { sprintf(tmp, lastpa, str  ); event.SetSubject(tmp); }
//  if (lastpb) { sprintf(tmp, lastpb, "..."); event.SetReport(tmp); }

    event.SetTaskValue(dne);

    wxPostEvent(this, event);
  }

  void SetProgress(size_t din, size_t dou) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    int rng = (int)(din ? ((unsigned __int64)dou * 0xFFFF) / din : 0);

    event.SetEffcRange(0xFFFF);
    event.SetEffcValue(rng);

    wxPostEvent(this, event);
  }

  void SetTopic(const char *topic) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetSubject(topic);

    wxPostEvent(this, event);
  }

  void SetReport(const char *status, size_t din, size_t dou, int dpl = -1) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    char tmp[256], tmpi[256], tmpo[256];

    /**/ if (din > 1000 * 1000 * 1000)
      sprintf(tmpi, "%d.%03d.%03d.%03d", (int)(((din / 1000) / 1000) / 1000), (int)(((din / 1000) / 1000) % 1000), (int)((din / 1000) % 1000), (int)(din % 1000));
    else if (din > 1000 * 1000)
      sprintf(tmpi, "%d.%03d.%03d", (int)((din / 1000) / 1000), (int)((din / 1000) % 1000), (int)(din % 1000));
    else if (din > 1000)
      sprintf(tmpi, "%d.%03d", (int)(din / 1000), (din % 1000));
    else
      sprintf(tmpi, "%d", (int)(din % 1000));

    /**/ if (dou > 1000 * 1000 * 1000)
      sprintf(tmpo, "%d.%03d.%03d.%03d", (int)(((dou / 1000) / 1000) / 1000), (int)(((dou / 1000) / 1000) % 1000), (int)((dou / 1000) % 1000), (int)(dou % 1000));
    else if (dou > 1000 * 1000)
      sprintf(tmpo, "%d.%03d.%03d", (int)((dou / 1000) / 1000), (int)((dou / 1000) % 1000), (int)(dou % 1000));
    else if (dou > 1000)
      sprintf(tmpo, "%d.%03d", (int)(dou / 1000), (dou % 1000));
    else
      sprintf(tmpo, "%d", (int)(dou % 1000));

    if (dpl < 0)
      sprintf(tmp, status, tmpi, tmpo);
    else
      sprintf(tmp, status, tmpi, tmpo, dpl);

    event.SetReport(tmp);

    wxPostEvent(this, event);
  }

  void PollProgress() {
    Wait();
  }

private:
  bool paused, quit;
  time_t tinit, tlast, tnow;

public:
  /* all executed by Progress-thread */
  virtual void PauseProgress(wxCommandEvent& event) {
    if (!paused) {
      Block();

      paused = true;
      DOPause->SetLabel("Unpause");
    }
    else {
      Unblock();

      paused = false;
      DOPause->SetLabel("Pause");
    }
  }

  virtual void AbortProgress(wxCloseEvent& event) {
    if (event.CanVeto())
      event.Veto();

    Abort();
  }

  virtual void AbortProgress(wxCommandEvent& event) {
    Abort();
  }

  virtual void IdleProgress(wxIdleEvent& event) {
    tnow = time(NULL);

    long tpass = (long)(tnow - tinit);
    if (tpass != tlast) {
      tlast = tpass;

      float f1 = (float)(DOTask->GetValue()) / (DOTask->GetRange());
      int tfinal = (int)floor((float)tpass / f1);
      int trem = tfinal - tlast;

      char buf[256];

      sprintf(buf, "%02d:%02ds", tpass / 60, tpass % 60);
      DORunning->SetLabel(buf);

      if (tfinal != 0x80000000) {
	sprintf(buf, "%02d:%02ds", trem / 60, trem % 60);
	DORemaining->SetLabel(buf);
	sprintf(buf, "%02d:%02ds", tfinal / 60, tfinal % 60);
	DOTotal->SetLabel(buf);
      }
      else {
	DORemaining->SetLabel("00:00s");
	DOTotal->SetLabel(buf);
      }
    }

//  Sleep(500);
//  event.RequestMore();
  }

private:
  HANDLE evt, end;
  HANDLE async;

  /* called from Progress-thread */
  void Abort() {
    quit = true;
    SetEvent(evt);
  }

  /* called from Async-thread */
  void Wait() {
    WaitForSingleObject(evt, INFINITE);

    /* signal abortion (inside Async-thread) */
    if (quit)
      throw runtime_error("ExitThread");
  }

  void Block() {
    ResetEvent(evt);
  }

  void Unblock() {
    SetEvent(evt);
  }

public:
  wxEventType evtLeave;
  int idLeave;
  int retCode;

  class LeaveEvent: public wxCommandEvent {
public:
    LeaveEvent(int id, const wxEventType& event_type) : wxCommandEvent(event_type, id) {}
    LeaveEvent(const LeaveEvent& event) : wxCommandEvent(event) {}

    wxEvent* Clone() const { return new LeaveEvent(*this); }
  };

  typedef void (wxEvtHandler::*LeaveEventFunction)(LeaveEvent &);

  /* called from outside-thread */
  int Enter(LPTHREAD_START_ROUTINE routine) {
    if ((async = CreateThread(NULL, 0, routine, this, 0, NULL)) == INVALID_HANDLE_VALUE)
      return 0;

    /* rebase the created thread, and possibly spawned OpenMP ones (via process) */
    SetThreadPriority(async, THREAD_PRIORITY_BELOW_NORMAL);
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);

    return ShowModal();
  }

  /* called from Async-thread */
  void Leave(int rc) {
    retCode = rc;

    LeaveEvent event(idLeave, evtLeave);
    wxPostEvent(this, event);

    /* wait for the progress-dialog to recognize our message */
    WaitForSingleObject(end, INFINITE);
  }

  /* called from Progress-thread */
  void Leave(LeaveEvent &) {
    /* signal that we recognize the message */
    SetEvent(end);

    WaitForSingleObject(async, INFINITE);
    CloseHandle(async);

    EndModal(retCode);
  }

  DDSoptPrg::DDSoptPrg(wxWindow *parent) : wxProgress(parent) {
    evtLeave = wxNewEventType();
    idLeave = wxNewId();

    /* Connect to event handler that will make us close */
    Connect(wxID_ANY, evtLeave,
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)wxStaticCastEvent(LeaveEventFunction, &DDSoptPrg::Leave),
      NULL,
      this);

    evtProgress = wxNewEventType();
    idProgress = wxNewId();

    /* Connect to event handler that will make us close */
    Connect(wxID_ANY, evtProgress,
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)wxStaticCastEvent(ProgressEventFunction, &DDSoptPrg::Progress),
      NULL,
      this);

    tinit = time(NULL);
    paused = false;
    quit = false;
    prg = this;

    evt = CreateEvent(
      NULL,		  // default security attributes
      TRUE,		  // manual reset
      TRUE,		  // initially set
      NULL		  // unnamed mutex
    );

    end = CreateEvent(
      NULL,		  // default security attributes
      TRUE,		  // manual reset
      FALSE,		  // initially not set
      NULL		  // unnamed mutex
    );

    SetSize(600, 265);
  }

  DDSoptPrg::~DDSoptPrg() {
    CloseHandle(evt);
    CloseHandle(end);
  }
};

#endif
