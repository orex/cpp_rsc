/* 
 * File:   cpp_rsc.cpp
 * Author: kirill
 *
 * Created on September 23, 2013, 10:55 AM
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>


#include "rsc_file_parse.h"
#include "rsc_create_files.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
  //cpp_rsc [-h] input_file [-o output_file_base] [-b base_folder] 
  //        [-sh suffix_header] [-sc suffix_source] [-n namespace]
  //
  string input_file;
  map<string, string> map_opt;
  
  map_opt["-o"] = map_opt["-b"] = map_opt["-sh"] 
                = map_opt["-sc"] = map_opt["-n"] 
                = map_opt["-w"]  = "";
  
  if(argc < 2)
    input_file = "";
  else
    input_file = argv[1];

  if( (input_file == "-h") || (input_file == "") )
  {
    cout << "C++ Resource program creator. Command line is" << endl
         << "  cpp_rsc [-h] input_rsc_file [-o output_file_base] [-b base_path]" << endl
         << "          [-sh suffix_header] [-sc suffix_source] [-n namespace] [-w data_width]" << endl
         << "  see manual. man cpp_rsc. for details" << endl;
    return 0;     
  }
  
  string curr_opt = "";
  for(int i = 2; i < argc; i++)
  {
    if(curr_opt == "")
    {
      if(map_opt.count(argv[i]) == 0)
      {
        cerr << "ERROR: Invalid argument: " << argv[i] << endl;
        return 1;
      }
      else
      {  
        curr_opt = argv[i];
        if( map_opt[curr_opt] != "" )
        {
          cerr << "ERROR: Duplicate option: " << curr_opt << endl;
          return 1;
        }
      }
    }
    else
    {  
      if( map_opt.count(argv[i]) != 0 )
      {
        cerr << "ERROR: Missing argument: " << curr_opt << endl;
        return 1;
      }  
      else
      {  
        map_opt[curr_opt] = argv[i];
        curr_opt = "";
      }  
    }  
  }

  if( curr_opt != "" )
  {
    cerr << "ERROR: Missing argument: " << curr_opt << endl;
    return 1;
  }
  
  rsc_file_parse rp;
  
  if(!rp.parse(input_file, map_opt))
  {
    cerr << "ERROR: Invalid parsing file " << input_file << endl;
    return 2;
  }  

  rsc_create_files rsc_cf;

  if(!rsc_cf.create(rp))
    return 3;
  
  return 0;
}

