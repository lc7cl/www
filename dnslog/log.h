#ifndef LOG_H_
#define LOG_H_

class loglog {
 public:
  static loglog GetLog() {
    static loglog log_;
    return log_;
  };
  
  int set_log(const string &file_path);
  
 private:
  loglog() = delete;
};

#endif