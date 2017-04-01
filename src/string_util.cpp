#include <string>
#include <sstream>
#include <vector>
#include "string_util.h"

/*******************************************************************************

Converts an integer to string

*******************************************************************************/
std::string int_to_string(int inval)
{
  std::string outstr;
  std::stringstream ss;

  ss << inval;
  ss >> outstr;
  return outstr;
}

/*******************************************************************************

Converts a char array to string

*******************************************************************************/
std::string chararray_to_string(char chararray[])
{
  std::string outstr;
  std::stringstream ss;

  ss << chararray;
  ss >> outstr;
  return outstr;
}

/*******************************************************************************

Converts a string to lower case

*******************************************************************************/
std::string string_to_lower(const std::string & instr)
{
  std::string outstr;
  std::string::size_type k, len;

  len = instr.size();
  for ( k = 0; k < len; k++ )
  {
    outstr.push_back(std::tolower(instr[k]));
  }
  return outstr;
}

/*******************************************************************************

Trims white space, line ending characters, etc. from end of string

*******************************************************************************/
std::string trim(const std::string & instr)
{
  int i, trimlen, len;

  len = instr.size();
  trimlen = len;
  for ( i = len-1; i >= 0; i-- )
  {
    if ( (instr[i] != ' ') && (instr[i] != '\n') && (instr[i] != '\0') )
    {
      trimlen = i+1;
      break;
    }
  }

  return instr.substr(0, trimlen);
}

/*******************************************************************************

Removes leading whitespace

*******************************************************************************/
std::string remove_leading_whitespace(const std::string & instr)
{
  unsigned int i, trimlen, len;

  len = instr.size();
  trimlen = 0;
  for ( i = 0; i < len; i++ )
  {
    if (instr[i] == ' ') { trimlen += 1; }
    else { break; }
  }

  return instr.substr(trimlen, len-trimlen);
}

/*******************************************************************************

Removes trailing comment from string

*******************************************************************************/
std::string remove_comment(const std::string & instr, char delim)
{
  std::size_t comment_pos;

  comment_pos = instr.find_first_of(delim);
  if (comment_pos != std::string::npos)
    return instr.substr(0, comment_pos);
  else
    return instr;
}

/*******************************************************************************

Splits a string into a vector of strings. Adapted from the answer here:
http://stackoverflow.com/questions/236129/split-a-string-in-c#236803

*******************************************************************************/
std::vector<std::string> split(const std::string & instr, char delim)
{
  std::stringstream ss(instr);
  std::vector<std::string> splitstr;
  std::string item;

  while (std::getline(ss, item, delim)) { splitstr.push_back(trim(item)); }

  return splitstr;
}

/*******************************************************************************

Wraps words in a string into separate lines, returned as a vector of strings

*******************************************************************************/
std::vector<std::string> wrap_words(const std::string & instr,
                                    unsigned int width)
{
  std::vector<std::string> wordvec, splitstr;
  unsigned int nwords, i;
  std::string line;

  splitstr = split(instr);
  nwords = splitstr.size();
  line = splitstr[0];
  for ( i = 1; i < nwords; i++ )
  { 
    if (line.size() + 1 + splitstr[i].size() > width)
    {
      wordvec.push_back(line);
      line = splitstr[i];
    }
    else { line += " " + splitstr[i]; }
  }
  wordvec.push_back(line);

  return wordvec;
}