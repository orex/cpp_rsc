/* 
 * File:   rsc_create_files.h
 * Author: kirill
 *
 * Created on September 23, 2013, 4:45 PM
 */

#ifndef RSC_CREATE_FILES_H
#define	RSC_CREATE_FILES_H

#include "rsc_file_parse.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

class rsc_create_files 
{
protected:
  rsc_file_parse * rsfp;
  
  std::map<std::string, int> file_sizes;
  
  void write_title(std::ostream &os, const std::string &extra_info);
  
  void begin_namespace(std::ostream &os, const std::string &name);
  void end_namespace(std::ostream &os, const std::string &name);
  
  void begin_header_protection(std::ostream &os, const std::string &name);
  void end_header_protection(std::ostream &os, const std::string &name);
  
  void write_variable_def(std::ostream &os, const rsc_item &item, bool header, bool text);
  
  void write_hex_data(std::istream &is, std::ostream &os, bool text, int &size);
  
  bool create_header();
  bool create_src();
public:
  bool create(rsc_file_parse &prs);
};

bool rsc_create_files::create(rsc_file_parse &prs)
{
  rsfp = &prs;
  
  bool result = create_src();
  
  if(result)
    result = create_header();
  
  return result;
}        

bool rsc_create_files::create_header()
{
  using namespace std;  
  
  ofstream f_out((rsfp->output_file_name + "." + rsfp->suffix_header).c_str(), fstream::out);
  if(!f_out.is_open())  
    return false;

  string hdr_define = rsfp->output_file_name + "_" + rsfp->suffix_header;
  
  transform(hdr_define.begin(), hdr_define.end(), hdr_define.begin(), ::toupper);
  
  for(int i = hdr_define.size() - 1; i >= 0; i--)
  {
    if( (hdr_define[i] == ' ') || (hdr_define[i] == '.') || (hdr_define[i] == '-'))
      hdr_define[i] = '_';
    else if( (hdr_define[i] == '/') || (hdr_define[i] == '\\') )
    {
      hdr_define.erase(0, i + 1);
      break;
    }  
  }  
  
  write_title(f_out, "This file is header file");
  
  f_out << endl;
  
  begin_header_protection(f_out, hdr_define);
  begin_namespace(f_out, rsfp->name_namespace);
  
  for(int i = 0; i < rsfp->items.size(); i++)
  {  
    write_variable_def(f_out, rsfp->items[i], true, rsfp->items[i].text_data);
    f_out << endl;
  }  
 
  end_namespace(f_out, rsfp->name_namespace);
  end_header_protection(f_out, hdr_define);
  
  return true;
}

bool rsc_create_files::create_src()
{
  using namespace std;
  ofstream f_out((rsfp->output_file_name + "." + rsfp->suffix_src).c_str(), fstream::out);
  if(!f_out.is_open())  
    return false;
  
  write_title(f_out, "This file is source file");
  
  f_out << "#include \"" << rsfp->output_file_name + "." + rsfp->suffix_header << "\"" << endl << endl;
  
  begin_namespace(f_out, rsfp->name_namespace);
  
  for(int i = 0; i < rsfp->items.size(); i++)
  {  
    write_variable_def(f_out, rsfp->items[i], false, rsfp->items[i].text_data);
    f_out << "  {" << endl;
    
    std::string in_f_name = rsfp->base_path + "/" + rsfp->items[i].file_path;
    
    ifstream ifs(in_f_name.c_str(), rsfp->items[i].text_data ?  fstream::in : (fstream::binary | fstream::in) );
    
    if(!ifs.is_open())
    {  
      cerr << "ERROR: Resource file \"" << in_f_name << "\" cannot be open." << endl;
      return false;
    }  
    write_hex_data(ifs, f_out, rsfp->items[i].text_data, rsfp->items[i].file_size);
    f_out << "  }; /* End of " << rsfp->items[i].file_path << " */" << endl << endl;
  }  
 
  end_namespace(f_out, rsfp->name_namespace);
  
  return true;
}

void rsc_create_files::write_variable_def(std::ostream &os, const rsc_item &item, bool header, bool text)
{
  using namespace std;
  os << "  /* Variable " + item.var_name + " from file " + item.file_path + " */\n" +
        "  " + (header ? "extern ": "") + (text ? "const char " : "const unsigned char ") + item.var_name + "[]" + (header ? ";": " = ")
     << endl;
  
  if(header)
  {
    os << "  /* Variable " + item.var_name + " size. */" << endl; 
    os << "  const int " + item.var_name + "_size = " << item.file_size << "; " << endl;
  }  
}

void rsc_create_files::write_hex_data(std::istream &is, std::ostream &os, bool text, int &size)
{
  using namespace std;
  int sm;
  std::stringstream ts;
  
  std::string line = "  ";
  
  size = 0;
  
  while(!is.eof())  
  {
    sm = is.get();
    std::string curr_item;
    ts.str("");
    ts << std::hex;
    
    if(!is.eof())
    {  
      ts << sm;
      curr_item = ts.str();
      if(curr_item.size() == 1)
        curr_item = "0" + curr_item;
      if( text )
        curr_item = "'\\x" + curr_item + "', ";
      else  
        curr_item = "0x" + curr_item + ", ";
      size++;
    }  
    else
    {  
      if(text)
      {  
        curr_item = "'\\x00', ";
        size++;
      }  
    }  

    
    if( (line + curr_item).size() > rsfp->data_width )
    {
      os << line << endl;
      line = "  " + curr_item;
    }
    else
      line += curr_item;
    
  }

  line.erase(line.size() - 2);
  os << line << endl;
}

void rsc_create_files::write_title(std::ostream &os, const std::string &extra_info)
{
  using namespace std;
  os << "/* Automatic generated file. DO NOT EDIT." << endl; 
  os << "   The file was generated by cpp_rsc program" << endl;   
  os << "   https://github.com/orex/cpp_rsc" << endl;
  os << "   Resource file name: " << rsfp->rsc_file_name << endl;
  os << "   " << extra_info << "*/" << endl << endl;
}

void rsc_create_files::begin_namespace(std::ostream &os, const std::string &name)
{
  if(name != "")
    os << "namespace " << name << " {" << std::endl;
}

void rsc_create_files::end_namespace(std::ostream &os, const std::string &name)
{
  if(name != "")
    os << "}; /*namespace " << name << " */" << std::endl;
}
  
void rsc_create_files::begin_header_protection(std::ostream &os, const std::string &name)
{
  os << "#ifndef " << name << std::endl;
  os << "#define " << name << std::endl;
}

void rsc_create_files::end_header_protection(std::ostream &os, const std::string &name)
{
  os << "#endif /*" << name << " */" << std::endl;
}



#endif	/* RSC_CREATE_FILES_H */

