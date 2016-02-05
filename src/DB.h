//
// Created by neverchanje on 8/24/15.
//

#pragma once

#include <string>

#include "Disallowcopying.h"


class Status;
class DBImpl;

class DB {
  __DISALLOW_COPYING__(DB);
 public:


 private:
  std::unique_ptr<DBImpl> pImpl_;
};


