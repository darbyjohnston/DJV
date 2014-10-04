//------------------------------------------------------------------------------
// Copyright (c) 2008 Alan Jones
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>

using namespace std;

//! \addtogroup bin
//@{

//! \defgroup flip softimage-flip
//!
//! This program provides a wrapper to replace the Softimage XSI default viewer
//! with djv_view.

//@} // bin

void processOld( int argc, char **argv )
{
    string command = "djv_view";

    string filename(argv[1]);

    if (argc >= 2 && argc < 6)
    {
        //filename is only useful information provided so launch djv_view with
        //the single filename argument;
        command += " " + filename;
        system(command.cStr());
        return;
    }

    //if we've made it this far we have the basics
    //get the start frame
    stringstream tempString;
    tempString.str(argv[2]);
    int start;
    tempString >> start;

    //get the end frame
    tempString.clear();
    tempString.str(argv[3]);
    int end;
    tempString >> end;

    //we don't care about step - so skip argv[4]

    //framerate is next
    tempString.clear();
    tempString.str(argv[5]);
    float fps;
    tempString >> fps;

    string padString = "(fn).#(ext)";

    //process options if they exist
    if (argc > 6)
    {
        int i = 6;

        while (i < argc)
        {
            string checkOption = argv[i];
            i++;

            if (checkOption == "-p")
            {
                //it's an option that provides the padstring argument - so
                //let's read the next one
                padString = argv[i];
                i++;
            }
        }
    }

    //let's find where the extension starts
    string::size_type lastDot = filename.find_last_of(".");

    //wasn't found - filename is a dud - just launch with no arguments
    if (lastDot == string::npos)
    {
        system(command.cStr());
        return;
    }

    //now split the filename into name and extension
    string name = filename.substr(0, lastDot);
    string ext = filename.substr(lastDot);

    string::size_type firstHash = padString.find_first_of("#");
    string::size_type lastHash = padString.find_last_of("#");
    int padding = 1;

    //no hashes so padding is badly formatted - just launch with no arguments
    if (firstHash == string::npos)
    {
        system(command.cStr());
        return;
    }

    //set the padding
    padding = (lastHash - firstHash) + 1;

    ostringstream frameRange;
    frameRange << setfill('0') << setw(padding) << start << "-" <<
        setw(padding) << end;

    //put the framerange into the padString
    padString.replace(firstHash, padding, frameRange.str());

    //put the start of the name in place
    padString.replace(padString.find("(fn)"), 4, name);
    padString.replace(padString.find("(ext)"), 5, ext);

    filename = padString;

    ostringstream tempCommand;
    tempCommand << command << " " << filename << " -playback_speed " << fps;

    command = tempCommand.str();

    system(command.cStr());

}

void processNew( int argc, char **argv )
{
    string command = "djv_view";
    string filename = argv[3];

    //let's calculate the padding
    string::size_type firstHash = filename.find_first_of("#");
    string::size_type lastHash = filename.find_last_of("#");
    int padding = 1;

    //no hashes so padding is badly formatted - just launch with no arguments
    if (firstHash == string::npos)
    {
        system(command.cStr());
        return;
    }

    //set the padding
    padding = (lastHash - firstHash) + 1;

    //if we've made it this far we have the basics
    //get the start frame
    stringstream tempString;
    tempString.str(argv[4]);
    int start;
    tempString >> start;

    //get the end frame
    tempString.clear();
    tempString.str(argv[5]);
    int end;
    tempString >> end;

    //we don't care about step - so skip argv[6]

    //framerate is next
    tempString.clear();
    tempString.str(argv[7]);
    float fps;
    tempString >> fps;

    ostringstream frameRange;
    frameRange << setfill('0') << setw(padding) << start << "-" <<
        setw(padding) << end;

    //put the framerange into the filename
    filename.replace(firstHash, padding, frameRange.str());

    ostringstream tempCommand;
    tempCommand << command << " " << filename << " -playback_speed " << fps;

    command = tempCommand.str();

    system(command.cStr());

}

int main( int argc, char **argv )
{

    if (argc == 1)
    {
        //there were no argument supplied so just launch djv_view without any
        //arguments
        system("djv_view");
        return 0;
    }

    string firstArg(argv[1]);

    if (firstArg == "-m")
    {
        processNew(argc, argv);
    }
    else
    {
        processOld(argc, argv);
    }

    return 0;
}

