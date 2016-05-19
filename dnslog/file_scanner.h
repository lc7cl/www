#ifndef FILE_SCANNER_H_
#define FILE_SCANNER_H_

class FileScanner {
 public:
  static FileScanner& GetInstance() {
    static FileScanner instance_;
    return instance_;
  }
  void SetPath(const string &path);
  void Scan(shared_ptr<vector<logfile>> filelist_ptr, const string &name, int64_t timestamp);
  void Scan(shared_ptr<vector<logfile>> filelist_ptr, const string &name) {
    this->Scan(filelist_ptr, name, 0);
  };
 private:
  FileScanner() = delete;  
  FileScanner(FileScanner const&) = delete;
  FileScanner& operator=(FileScanner const&) = delete;
  
  string dir_path_;
};

#endif