/****************************************************************************/
/*!
\file
   Log.hpp
\Author
   Ryan Dugie
\brief 
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0

    Master global Log class
*/
/****************************************************************************/

#ifndef LOG_HPP
#define LOG_HPP
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>

#define CHECK_FILE_OPEN(ofs, fname)             \
if (!ofs)                                       \
{                                               \
std::cerr << "ERROR: could not open log file "  \
<< fname << " for writing" << std::endl;        \
return false;                                   \
}                                               \

namespace DEBUG 
{

    static class Log 
    {

    public:

/****************************************************************************/
/*!
\brief
  Output some text to the error log
*/
/****************************************************************************/
        template <typename... Args>
        bool Error(Args&& ... args) 
        {
            std::string fname = std::string("Errors_") + PROJECT_NAME;

            if (!createLog[0]) 
            {
                if (!CreateLog(fname)) 
                {
                    return false;
                }
                createLog[0] = true;
            }

            // open the log file
            std::ofstream ofs(fname.c_str(), std::ofstream::app);

            // if it failed to be created
            CHECK_FILE_OPEN(ofs, fname);

            // write to the log
            int dummy[sizeof...(Args)] = { (ofs << std::forward<Args>(args) << ' ', 1)... };
            UNUSED(dummy);
            ofs << std::endl;

            // close the file
            ofs.close();

            // return success
            return true;
        }

/****************************************************************************/
/*!
\brief
    Output some text to the info log
*/
/****************************************************************************/
        template <typename... Args>
        bool Info(Args&& ... args) 
        {

#ifdef _DEBUG
            std::string fname = std::string("Errors_") + PROJECT_NAME;

            if (!createLog[1]) 
            {
                if (!CreateLog(fname)) 
                {
                    return false;
                }
                createLog[1] = true;
            }

            // open the log file
            std::ofstream ofs(fname.c_str(), std::ofstream::app);

            // if it failed to be created
            CHECK_FILE_OPEN(ofs, fname);

            // write to the log
            int dummy[sizeof...(Args)] = { (ofs << std::forward<Args>(args) << ' ', 1)... };
            UNUSED(dummy);
            ofs << std::endl;

            // close the file
            ofs.close();
#endif // _DEBUG

            // return success
            return true;
        }

    private:

/****************************************************************************/
/*!
\brief
    Create a fresh, empty log
*/
/****************************************************************************/
        bool CreateLog(std::string fname)
        {
            // create a new log file or overwrite the existing file
            std::ofstream ofs(fname, std::ofstream::out);

            // if the file failed to open
            CHECK_FILE_OPEN(ofs, fname);

            // get the current system time
            std::time_t CurrentTime = time(nullptr);

            // output time to the top of the debug log
            ofs << fname << " File local time: " << std::time(&CurrentTime) << std::endl;
            ofs << "Build version: " << __DATE__ << " " << __TIME__ << std::endl << std::endl;
            ofs.close();

            // return success
            return true;
        }

        bool createLog[2] = { false, false };

    }log;
}


#endif // LOG_H