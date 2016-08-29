/**
 * Copyright (C) 2016, Wu Tao. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

/**
 * StatusUtils contains the macro wrappers used to implement exception-like
 * error report, which save the stack trace of error status. Users wrap up an
 * error status when throwing it(as a return value) with DTHROWS(), and
 * when the user catching the error wants to throw it to upper caller, they
 * should wrap it up with DTRACE.
 */

#if defined(__FUNCTION__) && !defined(__func__)
#define __func__ __FUNCTION__
#endif

#define DTRACE(s)                                                         \
  ((s) << "Traceback: " << __FILE__ << ":" << __LINE__ << " " << __func__ \
       << "\n")

#define DTHROW(s) \
  ((s) << " " << __FILE__ << ":" << __LINE__ << " " << __func__ << "\n")