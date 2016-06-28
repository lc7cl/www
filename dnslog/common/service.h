#ifndef SERVICE_H_
#define SERVICE_H_

class Service {
 public:
  int Connect();
  int Send();
  int Read();
  int Close();

 private:
  io_service io_service_;
};

#endif