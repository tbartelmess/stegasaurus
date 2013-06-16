# stegasaurus

Purposely misspelled. Maybe as a hint?


## Setup

You will need to install `giflib5` in order to compile this project.
The recommended way of installing `giflib5` is with Homebrew on OSX,
or using the appropriate system package manager on Linux systems.

You will also need a working version of Ruby to use the build tools
for this project.


## Usage

Once everything is installed, you can build and run stegasaurus:

    rake build
    ./stegasaurus --message "My secret message" <input.gif >output.gif

And thus you have your message encoded into your image. Code for
retreiving the message from the images has purposely not been
published at this time.


## Limitations

The scheme that I created for encoding messages will not work if the
image uses less than 8 bits per pixel or if the input text uses
non-ASCII characters.

Encoding may silently fail if the image does not have enough strong
colouring, but this should not happen often in practice. Specifically,
it depends on the RGB values of the image pixels, and how many of those
values are greater than 128.

The implementation has been somewhat obfuscated, and somewhat made
obvious; I had a hard time deciding which made the game more fun. ;)


## Copyright

Copyright (c) 2013, Mark Rada.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Mark Rada nor the names of its
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Mark Rada BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
