#include "../json.c"
#include <stdio.h>

U32 cmp16(const U16 src[], const U16 dst[]) {
  U32 j;

  for (j = 0; src[j] && dst[j] && src[j] == dst[j]; j++);

  return src[j] == '\0' && dst[j] == '\0';
}

#define BUFLEN 10
const U32 CONST_BUFLEN = BUFLEN;

I32 main(void) {
  U16 buf[BUFLEN];
  I32 number;
  F32 fnumber;

  assert(jparse_int("42", &number)  ==  2 && number == 42, "jparse_int(\"42\") failed");
  assert(jparse_int("-42", &number) == 3 && number == -42, "jparse_int(\"-42\") failed");
  assert(jparse_int("0", &number)   ==   1 && number == 0, "jparse_int(\"0\") failed");
  assert(jparse_int("-0", &number)  ==   2 && number == 0, "jparse_int(\"-0\") failed");

  assert(jparse_boolean("true",  5, &number) == 4 && number == 1, "jparse_boolean(\"true\") failed");
  assert(jparse_boolean("false", 6, &number) == 5 && number == 0, "jparse_boolean(\"false\") failed");

  assert(jparse_float("3.14", 5, &fnumber) == 4 && fnumber - 3.14 < 0.1, "fnumber = %f", fnumber);
  assert(jparse_float("-3.14", 6, &fnumber) == 5 && fnumber + 3.14 < 0.1, "fnumber = %f", fnumber);

  return 0;
}
