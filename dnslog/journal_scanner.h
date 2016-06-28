#ifndef JOURNAL_SCANNER_H_
#define JOURNAL_SCANNER_H_

#include "journal.h"

class JournalScanner {
 public:
  static JournalScanner& GetInstance() {
    static JournalScanner instance_;
    instance_.dir_path_ = config::default_journal_path;
    return instance_;
  }
  void SetPath(const string &path);
  void Scan(shared_ptr<vector<Journal>> filelist_ptr, const string &name, int64_t timestamp);
  void Scan(shared_ptr<vector<Journal>> filelist_ptr, const string &name) {
    this->Scan(filelist_ptr, name, 0);
  };
 private:
  JournalScanner() = delete;  
  JournalScanner(JournalScanner const&) = delete;
  JournalScanner& operator=(JournalScanner const&) = delete;
  
  string dir_path_;
};

#endif