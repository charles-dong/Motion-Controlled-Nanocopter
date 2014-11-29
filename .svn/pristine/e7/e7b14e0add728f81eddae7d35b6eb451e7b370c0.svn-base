// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#include <cflie/CCrazyflie.h>
#include <stdio.h>

using namespace std;

int main(int argc, char **argv) {
  CCrazyRadio *crRadio = new CCrazyRadio;
  CCrazyRadioConstructor(crRadio,"radio://0/10/250K");
  
  if(startRadio(crRadio)) {
    CCrazyflie* cflieCopter=new CCrazyflie;
    CCrazyflieConstructor(crRadio,cflieCopter);

    //Initialize the set value
    setThrust(cflieCopter,10001);
    // Enable sending the setpoints. This can be used to temporarily
    // stop updating the internal controller setpoints and instead
    // sending dummy packets (to keep the connection alive).
    setSendSetpoints(cflieCopter,true);

    int i=0;
    while(cycle(cflieCopter)) {
      // Main loop. Currently empty.

      if(i<1200)setThrust(cflieCopter,37001);
      
      if(i>1200){
        if(39001-i*10>=10001)
          setThrust(cflieCopter,37001-i);
        else
          setThrust(cflieCopter,10001);
      }
      
      }
    
    delete cflieCopter;
  } else {
    printf("%s\n","Could not connect to dongle. Did you plug it in?" ); 
  }
  
  delete crRadio;
  return 0;
}
