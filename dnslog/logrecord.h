#ifndef LOGRECORD_H_
#define LOGRECORD_H_

class LogRecord {
public:
  LogRecord(const string &str) : data_(str), timestamp_(0) {};

private:
  string data_;
  ptime timestamp_;
};

#endif