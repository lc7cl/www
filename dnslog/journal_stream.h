#ifndef JOURNAL_STREAM_H_
#define JOURNAL_STREAM_H_

#include <queue>
#include "common/journal.h"

enum {
  STREAM_STATE_OK,
  STREAM_STATE_ERROR,
  STREAM_STATE_EOF,
  STREAM_STATE_NONE_JOURNAL,
};

class JournalStream {
 public:
  JournalStream(const string &name);
  int Read(JournalRecord &record);
  
 private:
  void ParseLine(const string& line, vector<string>& vStr);
  int GetJournal(Journal &jnl);
  
  string stream_name_;
  std::queue<Journal> jnl_list_;
  string buf_;
  ifstream in_;
  bool is_first_line_;
  Journal jnl_;
  vector<string> cache_;
};

shared_ptr<JournalStream> GetStream(const string &name);
void DeleteStream(const string &name);
shared_ptr<JournalStream> CreateStream(const string &name);

#endif