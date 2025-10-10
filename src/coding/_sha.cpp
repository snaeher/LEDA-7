#include <LEDA/coding/sha.h>
#include <string.h> 

LEDA_BEGIN_NAMESPACE

// DBL_INT_ADD treats two unsigned ints a and b as one 64-bit integer 
// and adds c to it

#define DBL_INT_ADD(a,b,c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

struct SHA256_CTX {
   uint8_t data[64];
   uint32_t datalen;
   uint32_t bitlen[2];
   uint32_t state[8];
};


static uint32_t k[64] = {
   0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
   0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
   0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
   0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
   0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
   0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
   0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
   0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
   0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
   0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
   0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
   0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
   0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
   0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
   0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
   0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};


static void sha256_transform(SHA256_CTX *ctx, uint8_t data[])
{
   uint32_t a,b,c,d,e,f,g,h,i,j,t1,t2,m[64];
      
   for (i=0,j=0; i < 16; ++i, j += 4)
      m[i] = (data[j] << 24) | (data[j+1] << 16) | (data[j+2] << 8) | (data[j+3]);
   for ( ; i < 64; ++i)
      m[i] = SIG1(m[i-2]) + m[i-7] + SIG0(m[i-15]) + m[i-16];

   a = ctx->state[0];
   b = ctx->state[1];
   c = ctx->state[2];
   d = ctx->state[3];
   e = ctx->state[4];
   f = ctx->state[5];
   g = ctx->state[6];
   h = ctx->state[7];
   
   for (i = 0; i < 64; ++i) {
      t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
      t2 = EP0(a) + MAJ(a,b,c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
   }
   
   ctx->state[0] += a;
   ctx->state[1] += b;
   ctx->state[2] += c;
   ctx->state[3] += d;
   ctx->state[4] += e;
   ctx->state[5] += f;
   ctx->state[6] += g;
   ctx->state[7] += h;
}

static void sha256_init(SHA256_CTX *ctx)
{  
   ctx->datalen = 0; 
   ctx->bitlen[0] = 0; 
   ctx->bitlen[1] = 0; 
   ctx->state[0] = 0x6a09e667;
   ctx->state[1] = 0xbb67ae85;
   ctx->state[2] = 0x3c6ef372;
   ctx->state[3] = 0xa54ff53a;
   ctx->state[4] = 0x510e527f;
   ctx->state[5] = 0x9b05688c;
   ctx->state[6] = 0x1f83d9ab;
   ctx->state[7] = 0x5be0cd19;
}

static void sha256_update(SHA256_CTX *ctx, uint8_t data[], size_t len)
{  
   for (size_t i=0; i < len; ++i) { 
      ctx->data[ctx->datalen] = data[i]; 
      ctx->datalen++; 
      if (ctx->datalen == 64) { 
         sha256_transform(ctx,ctx->data);
         DBL_INT_ADD(ctx->bitlen[0],ctx->bitlen[1],512); 
         ctx->datalen = 0; 
      }  
   }  
}  

static void sha256_final(SHA256_CTX *ctx, uint8_t hash[])
{  
   uint32_t i = ctx->datalen; 
   
   // Pad whatever data is left in the buffer. 
   if (ctx->datalen < 56) { 
      ctx->data[i++] = 0x80; 
      while (i < 56) 
         ctx->data[i++] = 0x00; 
   }  
   else { 
      ctx->data[i++] = 0x80; 
      while (i < 64) 
         ctx->data[i++] = 0x00; 
      sha256_transform(ctx,ctx->data);
      memset(ctx->data,0,56); 
   }  
   
   // Append to the padding the total message's length in bits and transform. 
   DBL_INT_ADD(ctx->bitlen[0],ctx->bitlen[1],ctx->datalen * 8);
   ctx->data[63] = ctx->bitlen[0]; 
   ctx->data[62] = ctx->bitlen[0] >> 8; 
   ctx->data[61] = ctx->bitlen[0] >> 16; 
   ctx->data[60] = ctx->bitlen[0] >> 24; 
   ctx->data[59] = ctx->bitlen[1]; 
   ctx->data[58] = ctx->bitlen[1] >> 8; 
   ctx->data[57] = ctx->bitlen[1] >> 16;  
   ctx->data[56] = ctx->bitlen[1] >> 24; 
   sha256_transform(ctx,ctx->data);
   
   // Since this implementation uses little endian byte ordering 
   // and SHA uses big endian, reverse all the bytes when copying 
   // the final state to the output hash. 
   for (i=0; i < 4; ++i) { 
      hash[i]    = (ctx->state[0] >> (24-i*8)) & 0x000000ff; 
      hash[i+4]  = (ctx->state[1] >> (24-i*8)) & 0x000000ff; 
      hash[i+8]  = (ctx->state[2] >> (24-i*8)) & 0x000000ff;
      hash[i+12] = (ctx->state[3] >> (24-i*8)) & 0x000000ff;
      hash[i+16] = (ctx->state[4] >> (24-i*8)) & 0x000000ff;
      hash[i+20] = (ctx->state[5] >> (24-i*8)) & 0x000000ff;
      hash[i+24] = (ctx->state[6] >> (24-i*8)) & 0x000000ff;
      hash[i+28] = (ctx->state[7] >> (24-i*8)) & 0x000000ff;
   }  
}  


void sha256(uint8_t* buffer, uint8_t* data, size_t data_sz)
{ // buffer must be an array of length >=32
  SHA256_CTX ctx;
  sha256_init(&ctx);
  sha256_update(&ctx,data,data_sz);
  sha256_final(&ctx,buffer);
}


string sha256(string txt)
{ uint8_t hash[32];
  sha256(hash,(uint8_t*)txt.cstring(),txt.length());
  string result = "";
  for(int i=0; i<32; i++) result += string("%02x",hash[i]);
  return result;
}


// sha1

#define ROTATE(value, bits) \
  (((value) << (bits)) | ((value) >> (32 - (bits))))

void sha1(uint8_t* buffer, uint8_t* data, size_t databytes)
{
  uint32_t W[80];
  uint32_t H[] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

  uint64_t databits = ((uint64_t)databytes) * 8;
  //uint32_t loopcount = (databytes + 8) / 64 + 1;
  uint32_t loopcount = uint32_t((databytes + 8) / 64 + 1);
  //uint32_t tailbytes = 64 * loopcount - databytes;
  uint32_t tailbytes = uint32_t(64 * loopcount - databytes);

  uint8_t datatail[128] = {0};


  /* Pre-processing of data tail (includes padding to fill out 512-bit chunk):
     Add bit '1' to end of message (big-endian)
     Add 64-bit message length in bits at very end (big-endian) */
  datatail[0] = 0x80;
  datatail[tailbytes - 8] = (uint8_t) (databits >> 56 & 0xFF);
  datatail[tailbytes - 7] = (uint8_t) (databits >> 48 & 0xFF);
  datatail[tailbytes - 6] = (uint8_t) (databits >> 40 & 0xFF);
  datatail[tailbytes - 5] = (uint8_t) (databits >> 32 & 0xFF);
  datatail[tailbytes - 4] = (uint8_t) (databits >> 24 & 0xFF);
  datatail[tailbytes - 3] = (uint8_t) (databits >> 16 & 0xFF);
  datatail[tailbytes - 2] = (uint8_t) (databits >>  8 & 0xFF);
  datatail[tailbytes - 1] = (uint8_t) (databits >>  0 & 0xFF);


  uint32_t didx = 0;

  /* Process each 512-bit chunk */
  for (uint32_t lidx = 0; lidx < loopcount; lidx++)
  {
    /* Compute all elements in W */
    memset (W, 0, 80 * sizeof (uint32_t));

    /* break 512-bit chunk into sixteen 32-bit, big endian words */
    for (int widx = 0; widx <= 15; widx++)
    { int wcount = 24;

      /* copy byte-per byte from specified buffer */
      while (didx < databytes && wcount >= 0)
      { W[widx] += (((uint32_t)data[didx]) << wcount);
        didx++;
        wcount -= 8;
      }

      /* fill W with padding as needed */
      while (wcount >= 0)
      { W[widx] += (((uint32_t)datatail[didx - databytes]) << wcount);
        didx++;
        wcount -= 8;
      }
    }

    /* extend the sixteen 32-bit words into eighty 32-bit words, 
       with potential optimization from:
       "Improving the Performance of the Secure Hash Algorithm (SHA-1)" 
       by Max Locktyukhin 
    */

    for (int widx = 16; widx <= 31; widx++) 
      W[widx] = ROTATE((W[widx-3] ^ W[widx-8] ^ W[widx-14] ^ W[widx-16]), 1);

    for (int widx = 32; widx <= 79; widx++) 
      W[widx] = ROTATE((W[widx-6] ^ W[widx-16] ^ W[widx-28] ^ W[widx-32]), 2);

    /* main loop */

    uint32_t a = H[0];
    uint32_t b = H[1];
    uint32_t c = H[2];
    uint32_t d = H[3];
    uint32_t e = H[4];
    uint32_t f = 0;
    uint32_t k = 0;

    for (int idx = 0; idx <= 79; idx++)
    {
      if (idx <= 19)
      { f = (b & c) | ((~b) & d);
        k = 0x5A827999;
      }
      else if (idx <= 39)
      { f = b ^ c ^ d;
        k = 0x6ED9EBA1;
      }
      else if (idx <= 59)
      { f = (b & c) | (b & d) | (c & d);
        k = 0x8F1BBCDC;
      }
      else //if (idx <= 79)
      { f = b ^ c ^ d;
        k = 0xCA62C1D6;
      }

      uint32_t temp = ROTATE(a,5) + f + e + k + W[idx];
      e = d;
      d = c;
      c = ROTATE(b,30);
      b = a;
      a = temp;
    }

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
  }

  /* store result in supplied buffer */

  for (int i = 0; i < 5; i++)
  { buffer[4*i+0] = (uint8_t) (H[i] >> 24);
    buffer[4*i+1] = (uint8_t) (H[i] >> 16);
    buffer[4*i+2] = (uint8_t) (H[i] >>  8);
    buffer[4*i+3] = (uint8_t) (H[i] >>  0);
  }

}

string sha1(string txt)
{ uint8_t hash[20];
  sha1(hash,(uint8_t*)txt.cstring(),txt.length());
  string result = "";
  for(int i=0; i<20; i++) result += string("%02x",hash[i]);
  return result;
}




LEDA_END_NAMESPACE
