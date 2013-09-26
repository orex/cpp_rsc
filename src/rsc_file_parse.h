/* 
 * File:   rsc_file_parse.h
 * Author: kirill
 *
 * Created on September 23, 2013, 1:43 PM
 */

#ifndef RSC_FILE_PARSE_H
#define	RSC_FILE_PARSE_H

#include <string>
#include <vector>
#include <map>
#include <fstream>

class rsc_item
{
public:
  std::string file_path;
  std::string var_name;
  int file_size; //Will be used in file creator.
  bool text_data;
public:
  rsc_item()
  { clear(); }
  
  void clear()
  { 
    file_path = "";
    var_name = "";
    text_data = false;
  }
};

  //cpp_rsc [-h] input_file [-o output_file_base] [-b base_folder] 
  //        [-sh suffix_header] [-sc suffix_source] [-n namespace]
 

class rsc_file_parse 
{
public:
  std::string rsc_file_name;
  std::string output_file_name;
  std::string base_path;
  std::string suffix_header;
  std::string suffix_src;
  std::string name_namespace;
  int data_width;
  std::vector<rsc_item> items;
protected:
  enum rsc_line {rcComment, rcData, rcSectionGeneral, rcSectionFile, rcError};
  rsc_line parse_line(std::string line, std::string &name, std::string &val);
  void trim(std::string &str);
  bool parse_sec_gen(const std::string &name, const std::string &val);
  bool parse_sec_file(rsc_item &item, const std::string &name, const std::string &val);
public:
  bool parse(std::string rsc_file_name_v, const std::map<std::string, std::string> &map_opt);
  static std::string get_file_name(const std::string &full_file_name);
private:

};

std::string rsc_file_parse::get_file_name(const std::string &full_file_name)
{
  int pos_dot   = full_file_name.size();
  int pos_slash = -1;  
  for(int i = full_file_name.size() - 1; i >= 0; i-- )
  {
    if( (full_file_name[i] == '.') && (pos_dot == full_file_name.size()) )
    {
      pos_dot = i;
    } else if( (full_file_name[i] == '/') || (full_file_name[i] == '\\') )
    {
      pos_slash = i;
      break;
    }  
  }  
  
  return full_file_name.substr(pos_slash + 1, pos_dot - pos_slash - 1);
}

bool rsc_file_parse::parse(std::string rsc_file_name_v, const std::map<std::string,std::string> &map_opt)
{
  rsc_file_name = rsc_file_name_v;

  output_file_name = get_file_name(rsc_file_name);
  
  base_path = "";
  suffix_header = "h";
  suffix_src = "cpp";
  name_namespace = "";
  data_width = 80;  
  

  using namespace std;
  
  char buff[300];
  
  ifstream rsc_file(rsc_file_name.c_str());
  if(!rsc_file.is_open())
  {
    cerr << "ERROR: File " <<  rsc_file_name << " did not open." << endl;
    return false;
  }
  
  rsc_item curr_item;
  rsc_line rp = rcComment;
  int i = 0;  
  while(!rsc_file.eof())
  {
    i++;
    rsc_line rc;
    std::string line, name, val;
    rsc_file.getline(buff, sizeof(buff) - 2);
    rc = parse_line(buff, name, val);
    if(rc == rcError)
    {
      cerr << "ERROR: Line " << i << "\"" << buff << "\" is not valid." 
           << "File: " << rsc_file_name << endl;
      return false;
    }
    if(rc == rcData)
    {
      bool good_parse = false;
      if(rp == rcSectionGeneral)
      {
        good_parse = parse_sec_gen(name, val);
      } 
      else if (rp == rcSectionFile)
      {
        good_parse = parse_sec_file(curr_item, name, val);        
      }
      if(!good_parse)
      {
        cerr << "ERROR: Line " << i << "\"" << buff << "\" is not parsed." 
             << "File: " << rsc_file_name << endl;
        return false;
      }  
    }
    
    if( (rc == rcSectionFile) || (rc == rcSectionGeneral) || rsc_file.eof() )
    {
      if(rp == rcSectionFile)
      {
        items.push_back(curr_item);
        curr_item.clear();
      }  
      rp = rc;
    }  
  }
  
  #define SET_DATA(opt, var) if( map_opt.at(opt) != "" ) var = map_opt.at(opt)

  SET_DATA("-o" , output_file_name);
  SET_DATA("-b" , base_path);
  SET_DATA("-sh", suffix_header);
  SET_DATA("-sc", suffix_src);
  SET_DATA("-n" , name_namespace);
  
  if( map_opt.at("-w") != "" ) data_width = atoi(map_opt.at("-w").c_str());
  
  if(data_width == 0)
  {
    cerr << "ERROR: Data width is incorrect." << endl;
    return false;
  }  
     
  return true;  
}        

void rsc_file_parse::trim(std::string &str)
{
  while( (str.size() > 0) && (str[0] == ' ') )
    str.erase(0, 1);
  
  while( (str.size() > 0) && (str[str.size() - 1] == ' ') )
    str.erase(str.size() - 1);
}


rsc_file_parse::rsc_line rsc_file_parse::parse_line(std::string line, std::string &name, std::string &val)
{
  using namespace std;
  trim(line);
  
  if( line.size() == 0 )
    return rcComment;    
  
  if( (line[0] == '#') || (line[0] == ';'))
    return rcComment;
  
  for(int i = 0; i < line.size(); i++)
  {
    if( (line[i] == '#') || (line[i] == ';'))
    {  
      line.erase(i);
      break;
    }  
  }

  trim(line);
    
  if( (line == "[general]" ) || (line == "[General]" ) )
    return rcSectionGeneral;
  
  if( (line == "[file]" ) || (line == "[File]" ) )
    return rcSectionFile;

  size_t pos = line.find("=");
  if(pos == string::npos)
    return rcError;
  else
  {
    name = line.substr(0, pos);
    trim(name);
    val = line.substr(pos + 1);
    trim(val);
    return rcData;
  }  
}

bool rsc_file_parse::parse_sec_gen(const std::string &name, const std::string &val)
{
  bool result = true;
  if(name == "output-file-name")
    output_file_name = val;
  else if (name == "base-path")
    base_path = val;
  else if (name == "suffix-header")
    suffix_header = val;
  else if (name == "suffix-src")
    suffix_src = val;
  else if (name == "namespace")
    name_namespace = val;
  else if (name == "data-width")
  {  
    data_width = atoi(val.c_str());
    result = data_width != 0;
  }
  else  
    result = false;
  
  return result;
}

bool rsc_file_parse::parse_sec_file(rsc_item &item, const std::string &name, const std::string &val)
{
  bool result = true;
  if(name == "file-path")
    item.file_path = val;
  else if (name == "var-name")
    item.var_name = val;
  else if (name == "text-file")
    item.text_data = (val == "true") || (val == "1") || (val == "TRUE");
  else
    result = false;
  
  return result;
}


#endif	/* RSC_FILE_PARSE_H */

