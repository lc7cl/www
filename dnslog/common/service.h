#ifndef SERVICE_H_
#define SERVICE_H_

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

class Service {
 public:
  Service();
  int Connect(const string &addr, const string &port);
  int HTTPSend(const string &uri, const string &data);
  int HTTPRead();
  void Close();

 private:
  boost::asio::io_service io_service_;
  boots::asio::deadline_timer timer_;
  boost::asio::ip::tcp::socket socket_;
  boost::posix_time::time_duration timeout_;

  void CheckDeadline();
};

#endif