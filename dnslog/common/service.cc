#include "service.h"

using boost::asio;

Service::Service()
  : timeout_(boost::posix_time::seconds(5)),
    timer_(io_service_),
    socket_(io_service_) {

}

void Service::CheckDeadline() {
  if (this->timer_.expires_at() <= deadline_timer::traits_type::now()) {
    boost::system::error_code ignored_ec;
    this->socket_.close(ignored_ec);
    this->timers.expires_at(boost::posix_time::pos_infin);
  }
  this->timers.async_wait(&Service::CheckDeadline, this);
}

int Service::Connect(const string &addr, const string &port) {
  boots::system::error_code ec = boost::asio::error::would_block;
  ip::tcp::endpoint endpoint(ip::address::from_string(addr), port);
  this->timer_.expires_from_now(this->timeout_);
  this->socket_.async_connect(endpoint, var(ec) = boost::lambda::_1);
  do this->io_service_.run_one(); while (ec == boost::asio::error::would_block);
  this->timer_.cancel();
  if (ec || !this->sockect_.is_open()) {
    this->socket_.close();
    return -1;
  }
  return 0;
}

int Service::HTTPSend(const string &uri, const string &data) {
  boost::system::error_code ec = boost::asio::error::would_block;
  streambuf request;
  std::ostream request_stream(&request);
  request_stream << "POST " << uri << " HTTP/1.0\r\n";
  request_stream << "Host: " << " \r\n";
  request_stream << "Accept:*/*\r\n";
  request_stream << "Content-Length: " << data.length() << "\r\n";
  request_stream << "Connection: close\r\n\r\n";
  request_stream << data;

  this->timer_.expires_from_now(this->timeout_);
  async_write(this->socket_, request, var(ec) == boost::lambda::_1);
  do this->io_service_.run_one(); while (ec == boost::asio::error::would_block);
  this->timer_.cancel();
  if (ec) {
    this->socket_.close();
    return -1;
  }
  return 0;
}

int Service::HTTPRead(unsigned int &rc, string &data) {
  boost::system::error_code ec = boost::asio::error::would_block;
  streambuf response;
  this->timer_.expires_from_now(this->timeout_);
  async_read_until(this->socket_, response, "\r\n", 
                    var(ec) = boost::lambda::_1);
  do this->io_service_.run_one(); while (ec == boost::asio::error::would_block);
  if (ec) {
    this->sockect_.close();
    return -1;
  }

  std::istream response_stream(&response);
  string http_version;
  response_stream >> http_version;
  unsigned int status_code;
  response_stream >> status_code;
  string status_message;
  getline(response_stream, status_message);
  if (!response_stream || http_version.substr(0, 5)) {
    this->socket_.close();
    return -1;
  }
  ec = boost::asio::error::would_block;
  async_read(this->socket_, response, transfer_at_least(1), 
              var(ec) = boost::lambda::_1);  
  this->timer_.cancel();
  do this->io_service_.run_one(); while (ec == boost::asio::error::would_block);
  if (ec) {
    this->sockect_.close();
    return -1;
  }
  std::ostringstream ss;
  ss << response;
  data = ss.tr();
  rc = status_code;
  return 0;
}

void Service::Close() {
  this->timers.expires_at(boost::posix_time::pos_infin);
  this->socket_.close();
}