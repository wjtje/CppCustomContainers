#include <stdint.h>
#include <stdio.h>

#include "include/set.h"

enum class Options {
  kOptions1,
  kOptions2,
  kOptions3,
  kOptions4,
  kOptions5,
  kOptions6,
  kOptions7,
  kOptions8,
  kOptions9,
  kOptions10
};

int main() {
  typedef Set<Options, Options::kOptions1, Options::kOptions10> SetType;

  SetType set;
  set << Options::kOptions1 << Options::kOptions2 << Options::kOptions3;

  SetType set2;
  set2 << Options::kOptions4 << Options::kOptions2 << Options::kOptions6;

  set *= set2;  // Exclusive OR

  // set should now only contain kOptions2
  if (set == SetType() << Options::kOptions2) {
    printf("Success\n");
  }
  sizeof(set);

  return 0;
}
