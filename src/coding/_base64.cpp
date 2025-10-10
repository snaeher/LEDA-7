#include <LEDA/coding/base64.h>

#include <assert.h>

LEDA_BEGIN_NAMESPACE

string base64(unsigned char* bytes, size_t len)
{
  static char b64chars[] = 
       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  if (bytes == NULL || len == 0) return "";

  // compute b64 size

  size_t sz = len;
  while (sz % 3 != 0) sz++;
  sz = 4*sz/3 + 1;

  char* buf = new char[sz];
  buf[sz-1] = '\0';

  for (size_t i=0, j=0; i<len; i+=3, j+=4) 
  { assert(j+3 < sz);
    size_t v = bytes[i];
    v = i+1 < len ? v << 8 | bytes[i+1] : v << 8;
    v = i+2 < len ? v << 8 | bytes[i+2] : v << 8;

    buf[j]   = b64chars[(v >> 18) & 0x3F];
    buf[j+1] = b64chars[(v >> 12) & 0x3F];

    if (i+1 < len)
      buf[j+2] = b64chars[(v >> 6) & 0x3F];
    else
      buf[j+2] = '=';

    if (i+2 < len)  
      buf[j+3] = b64chars[v & 0x3F];
    else  
      buf[j+3] = '=';
  }

  string result = buf;
  delete[] buf;
  return result;
}


/*

static char encoding_table[] =
       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64_encode(unsigned char* data, size_t data_sz)
{
  size_t buf_sz = 4 * ((data_sz + 2) / 3);

  char* buf = new char[buf_sz + 1];
  if (buf == NULL) return "";

  size_t i = 0;
  size_t j = 0;

  while (i < data_sz) 
  { uint32_t octet_a = i < data_sz ? (unsigned char)data[i++] : 0;
    uint32_t octet_b = i < data_sz ? (unsigned char)data[i++] : 0;
    uint32_t octet_c = i < data_sz ? (unsigned char)data[i++] : 0;
    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
    buf[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
    buf[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
    buf[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
    buf[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
   }

  int r = data_sz % 3;

  if (r == 1) r = 2;
  if (r == 2) r = 1;

  //for (int i = 0; i < mod_table[data_sz % 3]; i++)
  for (int i=0; i < r; i++) buf[buf_sz-i-1] = '=';

  buf[buf_sz] = '\0';

  string result = buf;
  delete[] buf;
  return result;;
}

static char decoding_table[256];

unsigned char* base64_decode(string data, size_t& sz)
{
  for (int i = 0; i < 64; i++) decoding_table[encoding_table[i]] = i;

  int data_sz = data.length();

  if (data_sz % 4 != 0) return 0;

  sz = 3*(data_sz/4);
  if (data[data_sz - 1] == '=') sz--;
  if (data[data_sz - 2] == '=') sz--;

  unsigned char* buf = new unsigned char[sz];

  if (buf == NULL) return 0;

  size_t i = 0;
  size_t j = 0;

  while (i < data_sz) 
  { uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
    uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
    uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
    uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

    uint32_t triple = (sextet_a << 3 * 6)
                    + (sextet_b << 2 * 6)
                    + (sextet_c << 1 * 6)
                    + (sextet_d << 0 * 6);

    if (j < sz) buf[j++] = (triple >> 2 * 8) & 0xFF;
    if (j < sz) buf[j++] = (triple >> 1 * 8) & 0xFF;
    if (j < sz) buf[j++] = (triple >> 0 * 8) & 0xFF;
  }

  return buf;
}
*/

LEDA_END_NAMESPACE

