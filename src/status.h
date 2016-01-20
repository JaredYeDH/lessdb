//
// Created by neverchanje on 8/24/15.
//

#pragma once

#include <string>

class Status {

  //copyable

 public:

  static Status OK() { return Status(Code::OK); };

  static Status Failed() { return Status(Code::kFailed); };

  std::string ToString() {
    switch (status_) {
      case Code::OK:
        return "OK";
      case Code::kFailed:
        return "Failed";
      default:
        return "Unknown";
    }
  }

  bool IsOK() const { return status_ == Code::OK; }

  bool IsFailed() const { return status_ == Code::kFailed; }

 private:

  Status() :
      status_(Code::OK) { };

  enum Code {
    OK = 0,
    kFailed = 1
  };

  explicit Status(Code status) :
      status_(status) {
  }

  Code status_;
};

