//Class definition - SettingsFile
//
//Modified by Joanes Lizarraga (2013/06/21)
//LoopAnalysis
//
//A class desinged to make reading in runtime settings
//easier. Settings are read / written to a file in the
//form: SettingName=SettingValue.
//
//When opening files, or using a constructor to do so,
//a mode should be specified: either:
//
// SettingsFile::auto_create  - create file if not there
//                              and add any missing settings
//                              with values already at
//                              memory locations
//
// SettingsFile::no_create - do not create a non-present
//                           file or add any missing settings
//                           and exit if anything is not found
//
//If the command-line arguments are input via optional inputs on
//either the constructor or open member function, then these
//take president: they are effectively first in the file. 
//
//Note, when used with std::string objects, only one word
//is allowed per setting, ie. spaces are not allowed. This
//is because of the way that the >> operator works for
//this class. This fits nicely with the command-line override, however.
//
//Note that the string specified followed by = is searched
//for in the file and then the input read. If one setting
//name is also the end of another that preceeds it in the file
//then the wrong one will be read.
//
//If the preprocessor defintion PARALLEL_MPI is set, then only the
//primary MPI process is able to create or add to the setting
//file in anyway. Further processes will be sent the file contents via
//MPI.

#ifndef SETTINGSFILE_HPP
#define SETTINGSFILE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


//CLASS PROTOTYPE======================

class SettingsFile
{
private:
  std::string filename_;
  std::fstream file_;
  std::stringstream stream_;
  int mode_;
  //bool isRoot_;    //is process the root one (false if non-parallel)
	//Search=================================
	bool search(const std::string search_string);
	
public:
  static int noCreate;
  static int autoCreate;


	//Constructors=======================
	SettingsFile();
	SettingsFile(const std::string filename, const int argc = 0, char** argv = NULL);
	
	//Destructor=========================
	~SettingsFile();
	
	//File open / close / create ========
	void open(const std::string filename, const int argc = 0, char** argv = NULL);
	void close();
	//void create(const std::string filename);
	
	//Settings read / write==================
	template<class TemplateClass>
	void read(const std::string parameter_name, TemplateClass& parameter);
	template<class TemplateClass>
	void add(const std::string parameter_name, const TemplateClass& parameter);
 
};


//CONSTANTS===========================
int SettingsFile::noCreate = 1;
int SettingsFile::autoCreate = 0;


//CONSTRUCTORS========================
SettingsFile::SettingsFile(){}

SettingsFile::SettingsFile(const std::string filename, const int argc, char** argv) 
{
  this->open(filename, argc, argv);
}

//DESTRUCTOR==========================
SettingsFile::~SettingsFile() {this->close();}


//OPEN================================
void SettingsFile::open(const std::string filename, const int argc, char** argv)
{
  char c;

  filename_=filename;
    
    //Open file
    file_.open(filename_.c_str(), std::fstream::in);
    if(!file_.is_open())
    {
        std::cout<<"SettingsFile: "<<filename_<<" not found."<<std::endl;
    }
    
    //Read command line into stringstream
    for(int i=0; i<argc; i++)
    {
        for(int j=0; argv[i][j]!='\0'; j++)
        {
            stream_<<argv[i][j];
        }
        stream_<<endl;
    }
    
    //Read file into stringstream
    while(!file_.eof())
    {
        c=file_.get();
        if(c=='#')
        {
            while(!file_.eof() && c!='\n') { c=file_.get(); }
            if(file_.eof()) { break; }
        }
        stream_.put(c);       
    }
    file_.close();  
}

//FILE CLOSE============================
void SettingsFile::close()
{
    filename_=".";
}

//FILE CREATE===========================
/*void SettingsFile::create(const std::string filename)
{
  if(isRoot_)
    {
      filename_=filename;
      mode_=0;
      
      file_.open(filename_.c_str(), std::fstream::out);
      if(!file_.is_open())
	  {
		  std::cout<<"SettingsFile: Cannot create: "<<filename<<std::endl;
		  std::cout<<"SettingsFile: Exiting..."<<std::endl;
#ifndef SERIAL
		  parallel.abortRequest();
#else
		  exit(555);
#endif	
	  }
      else
	  {
		  file_.close();
		  file_.clear();
		  file_.open(filename.c_str(), std::fstream::in);
	  }
    }

  //parallel.barrier();
}*/

//PARAMETER READ===========================
template<class TemplateClass>
void SettingsFile::read(const std::string parameterName, TemplateClass &parameter)
{
  if(this->search(parameterName+'='))
    {
      stream_>>parameter;
    }
}

//PARAMETER WRITE===========================
template<class TemplateClass>
void SettingsFile::add(const std::string parameter_name, const TemplateClass &parameter)
{
      file_.clear();
      file_.open(filename_.c_str(), std::ios::out | std::ios::app);
      file_ << parameter_name << '=' << parameter << std::endl;
      if(!file_.good())
	{
	  std::cout << "SettingsFile: Could not write to file: " << filename_ << std::endl;
	  std::cout << "SettingsFile: Exiting... " << std::endl;
/*#ifndef SERIAL
		parallel.abortRequest();
#else
		exit(555);
 #endif*/

	}
      file_.close();
}



//SEARCH=====================================
bool SettingsFile::search(const std::string searchString)
{
  unsigned int i=0;
  char c;
  //Set to beginning of file
  stream_.clear(); //clear any errors from having failed a previous search
  stream_.seekg(0);
 
  //Search
  while(stream_.good() && i<searchString.length())
    {
      c=stream_.get();
      if(c==searchString[i]){i++;}
      else{i=0;}
    }
  return stream_.good();
}

#endif
