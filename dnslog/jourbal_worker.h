#ifndef JOURNAL_WORKER_H_
#define JOURNAL_WORKER_H_

class JournalWorker {
 public:
  void Work();
  void BindStream(vector<string> namelist);
  
 private:
  vector<shared_ptr<LogStream>> streamlist_;
};

#endif