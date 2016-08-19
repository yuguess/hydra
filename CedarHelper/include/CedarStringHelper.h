#ifndef CEDAR_STRING_HELPER_H
#define CEDAR_STRING_HELPER_H

#include <bitset>

template<typename C>
int cedarSplit(std::string const& s, char const* d, C& ret) {
  C output;

  std::bitset<255> delims;
  while (*d) {
    unsigned char code = *d++;
    delims[code] = true;
  }

  typedef std::string::const_iterator iter;
  iter beg;
  bool in_token = false;
  for (std::string::const_iterator it = s.begin(), end = s.end(); it != end; ++it) {
    if (delims[*it]) {
      if(in_token) {
        output.push_back(typename C::value_type(beg, it));
        in_token = false;
      }
    } else if (!in_token) {
      beg = it;
      in_token = true;
    }
  }

  if (in_token)
    output.push_back(typename C::value_type(beg, s.end()));
  output.swap(ret);

  return 0;
}

#endif
