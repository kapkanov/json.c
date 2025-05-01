#include "./includes/types32.c"
#include "./includes/limits32.c"


I32 cmp8len(const U8 src[], const U8 dst[], const U32 len) {
  U32 j;

  for (j = 0; j < len && src[j] == dst[j]; j++);

  return j == len;
}


void strhex2char(const U8 str[], U8 dst[]) {
  U32 j;
  U8  table[103];

  for (j = '0'; j <= '9'; j++)
    table[j] = j - '0';
  for (j = 'A'; j <= 'F'; j++)
    table[j] = j - 'A' + 10;
  for (j = 'a'; j <= 'f'; j++)
    table[j] = j - 'a' + 10;

  for (j = 0; j < 4; j++)
    assert('0' <= str[j] && str[j] <= '9' || 'A' <= str[j] && str[j] <= 'F' || 'a' <= str[j] && str[j] <= 'f', "strhex2char: '%c' is not valid hex digit", str[j]);

  dst[0] = 0 | str[0] << 4 | str[1];
  dst[1] = 0 | str[2] << 4 | str[3];
}


U32 jparse_string(const U8 src[], U8 dst[], const U32 dstlen) {
  U32 j, k;

  assert(src[0] == '"', "jparse_string: string should start from a '\"' character. But it start with %.20s", src);

  for (j = 0, k = 0; j < U32_MAX && k < U32_MAX && src[j] && src[j] != '"'; j++, k++) {
    assert(k < dstlen, "String \"%.20s...\" is too big", src);
    if (src[j] != '\\') {
      dst[k] = src[j];
      continue;
    }
    switch (src[j + 1]) {
    case '"':
      dst[k] = '"';
      break;
    case '\\':
      dst[k] = '\\';
      break;
    case '/':
      dst[k] = '/';
      break;
    case 'b':
      dst[k] = '\b';
      break;
    case 'f':
      dst[k] = '\f';
      break;
    case 'n':
      dst[k] = '\n';
      break;
    case 'r':
      dst[k] = '\r';
      break;
    case 't':
      dst[k] = '\t';
      break;
    case 'u':
      assert(k + 1 < dstlen, "jparse_string: String is too long");
      strhex2char(src + j + 2, dst + k);
      j += 4;
      k++;
      break;
    default:
      j--;
    }
    j++;
  }
}


U32 is_whitespace(const U8 c) {
  return c == ' ' || c == '\t' || c == '\n';
}


U32 jparse_int(const U8 src[], I32 *res) {
        U32 j;
        I32 sign;
  const I32 RES_LIMIT = I32_MAX / 10;

  j    = 0;
  sign = 1;

  if (src[0] == '-') {
    sign = -1;
    j    =  1;
  }

  for (*res = 0; '0' <= src[j] && src[j] <= '9'; j++) {
    assert(*res < RES_LIMIT && *res * 10 < I32_MAX - (src[j] - '0'), "jparse_int: The number is too big for I32. The first parsed part is %d", *res);
    *res = *res * 10 + src[j] - '0';
  }

  *res *= sign;

  return j;
}


U32 jparse_boolean(const U8 src[], const U32 srclen, I32 *res) {
  if (srclen > 3 && cmp8len(src, "true", 4)) {
    *res = 1;
    return 4;
  }
  if (srclen > 4 && cmp8len(src, "false", 5)) {
    *res = 0;
    return 5;
  }

  assert(0, "jparse_boolean: %.4s is not a valid boolean", src);
}

