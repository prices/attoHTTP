# attoHTTP
A very small embedded HTTP 1.0 Server

## Introduction

This web server is meant to have a very small footprint.  It is designed to go into
32bit ARM Cortex MCUs, however it is written as much as possible to be cross platform.
It will compile and run on 32 and 64bit x86 to perform the testing.

## Requirements

 - GNU Make
 - gcc
 - doxygen to make the documentation

### Notes on GCC
gcc may not be required.  I use the compiler flags '-Wall -Werror -pedantic -std=c11'.
This should make it so that this is ANSI standard C11 that I am writing and it
*should* compile in any ANSI standard C11 compiler.  While I do not test other compilers,
I am happy to accept pull requests to make the code compile on other compilers,
provided it doesn't break compiling on gcc.

## Make Targets

 - `test` : Compiles and runs the tests
 - `junit` : Compiles and creates JUnit compatible XML files
 - `doc` : Uses doxygen to build the source documentation
 - `clean` : Removes all temporary files in all directories, including the documentation

## Testing

This software is unit tested using the FCTX Unit testing framework.  This means that
the software must compile and run on x86 machines, at least.

Anything in the software that can be tested, should be tested.

## Documentation

This project uses doxygen to create documentation.  All functions and files in the src/
directory should be documented.

## Pull Requests

Pull requests are welcomed, as long as they are properly tested and documented.

## attoHTTP License

 The MIT License (MIT)

 Copyright (c) 2015 Scott Price

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

## FCTX (test/fct.h) License

 FCTX (Fast C Test) Unit Testing Framework

 Copyright (c) 2008, Ian Blumel (ian.blumel@gmail.com)
 All rights reserved.

 This license is based on the BSD License.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

    * Neither the name of, Ian Blumel, nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

