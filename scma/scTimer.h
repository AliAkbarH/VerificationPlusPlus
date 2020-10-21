#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef TIME_ON
#include <time.h>
#include <sys/time.h>
class TIME_DIFF {
  struct timeval start, end;
  unsigned long long & v;
  bool isOn;
  public:
  TIME_DIFF(unsigned long long & av, unsigned int flags, unsigned int cond) 
    : v(av), isOn( flags & cond != 0)
  {
    if (isOn) {
      gettimeofday(&start, NULL);
    }
  }
  ~TIME_DIFF() {
    if (isOn) {
      gettimeofday(&end, NULL);
      unsigned long long diff = (end.tv_sec * 1000000 + end.tv_usec)
        - (start.tv_sec * 1000000 + start.tv_usec);
      v += diff;
    }
  }
};

#define TIME_IT(var,flags,cond,code) \
  { TIME_DIFF tttt(var, flags, cond); code ; }

#else

#define TIME_IT(var,flags,cond,code) \
  code;

#endif
////////////////////
//testing code
#ifdef TEST_TIME_ON
int f(int x) {
  for(int i = 0; i < 1<<20; i++){
    x = x+2*x;
  }
  return 5;
}

#include <iostream>
using namespace std;

int main() {
  unsigned long long v=0;
  int flags = 3;
  TIME_IT(v,1,f(4));

  cout << "v: " << v;

  return 0;
}
#endif
// end testing code
/////////////////////

#endif
