#ifndef FILE_SCANNER_H_
#define FILE_SCANNER_H_

class FileScanner {
 public:
  FileScanner(const string &path);
  void Scan(shared_ptr<vector<logfile>> filelist_ptr, int64_t timestamp);
  void Scan(shared_ptr<vector<logfile>> filelist_ptr) {
    this->Scan(filelist_ptr, 0);
  };
 private:
  string dir_path_;
};

#endif