#ifndef LOG_H_
#define LOG_H_

class Log {
 public:
  static Log& GetLog() {
    static Log log_;
    return log_;
  };

  Log& operator<<(const string &str) {
    out_ << str;
    return this;
  };
  
  int set_log(const string &file_path) {
    if (out_.is_open()) {
      out_.close();
    }
    out_.open(file_path);
    if (!out_.is_open())
      return -1;
    return 0;
  };
  
 private:
  Log() = delete;
  std::ofstream out_;
};

#endif