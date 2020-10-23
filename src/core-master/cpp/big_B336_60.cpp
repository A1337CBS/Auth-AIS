/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

/* CORE basic functions for BIG type */
/* SU=m, SU is Stack Usage */

#include "big_B336_60.h"

//int B336_60::BIGMULS=0;
//int B336_60::BIGSQRS=0;

/* test a=0? */
int B336_60::BIG_iszilch(BIG a)
{
    int i;
    chunk d=0;
    for (i = 0; i < NLEN_B336_60; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_B336_60));
}

/* test a=1? */
int B336_60::BIG_isunity(BIG a)
{
    int i;
    chunk d=0;
    for (i = 1; i < NLEN_B336_60; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_B336_60) & (((a[0]^1)-1)>>BASEBITS_B336_60));
}

/* test a=0? */
int B336_60::BIG_diszilch(DBIG a)
{
    int i;
    chunk d=0;
    for (i = 0; i < DNLEN_B336_60; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_B336_60));
}

/* SU= 56 */
/* output a */
void B336_60::BIG_output(BIG a)
{
    BIG b;
    int i, len;
    len = BIG_nbits(a);
    if (len % 4 == 0) len /= 4;
    else
    {
        len /= 4;
        len++;
    }
    if (len < MODBYTES_B336_60 * 2) len = MODBYTES_B336_60 * 2;

    for (i = len - 1; i >= 0; i--)
    {
        BIG_copy(b, a);
        BIG_shr(b, i * 4);
        printf("%01x", (unsigned int) b[0] & 15);
    }
}

/* SU= 16 */
void B336_60::BIG_rawoutput(BIG a)
{
    int i;
    printf("(");
    for (i = 0; i < NLEN_B336_60 - 1; i++)
#if CHUNK==64
        printf("%jx,", (uintmax_t) a[i]);
    printf("%jx)", (uintmax_t) a[NLEN_B336_60 - 1]);
#else
        printf("%x,", (unsigned int) a[i]);
    printf("%x)", (unsigned int) a[NLEN_B336_60 - 1]);
#endif
}

/* Swap a and b if d=1 */
void B336_60::BIG_cswap(BIG a, BIG b, int d)
{
    int i;
    chunk t, c = d;
    c = ~(c - 1);
#ifdef DEBUG_NORM
    for (i = 0; i < NLEN_B336_60 + 2; i++)
#else
    for (i = 0; i < NLEN_B336_60; i++)
#endif
    {
        t = c & (a[i] ^ b[i]);
        a[i] ^= t;
        b[i] ^= t;
    }
}

/* Move g to f if d=1 */
void B336_60::BIG_cmove(BIG f, BIG g, int d)
{
    int i;
    chunk b = (chunk) - d;
#ifdef DEBUG_NORM
    for (i = 0; i < NLEN_B336_60 + 2; i++)
#else
    for (i = 0; i < NLEN_B336_60; i++)
#endif
    {
        f[i] ^= (f[i] ^ g[i])&b;
    }
}

/* Move g to f if d=1 */
void B336_60::BIG_dcmove(DBIG f, DBIG g, int d)
{
    int i;
    chunk b = (chunk) - d;
#ifdef DEBUG_NORM
    for (i = 0; i < DNLEN_B336_60 + 2; i++)
#else
    for (i = 0; i < DNLEN_B336_60; i++)
#endif
    {
        f[i] ^= (f[i] ^ g[i])&b;
    }
}

/* convert BIG to/from bytes */
/* SU= 64 */
void B336_60::BIG_toBytes(char *b, BIG a)
{
    int i;
    BIG c;
    BIG_copy(c, a);
    BIG_norm(c);
    for (i = MODBYTES_B336_60 - 1; i >= 0; i--)
    {
        b[i] = c[0] & 0xff;
        BIG_fshr(c, 8);
    }
}

/* SU= 16 */
void B336_60::BIG_fromBytes(BIG a, char *b)
{
    int i;
    BIG_zero(a);
    for (i = 0; i < MODBYTES_B336_60; i++)
    {
        BIG_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_B336_60] = 1; a[MNV_B336_60] = 0;
#endif
}

void B336_60::BIG_fromBytesLen(BIG a, char *b, int s)
{
    int i, len = s;
    BIG_zero(a);

    if (len > MODBYTES_B336_60) len = MODBYTES_B336_60;
    for (i = 0; i < len; i++)
    {
        BIG_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_B336_60] = 1; a[MNV_B336_60] = 0;
#endif
}



/* SU= 88 */
void B336_60::BIG_doutput(DBIG a)
{
    DBIG b;
    int i, len;
    BIG_dnorm(a);
    len = BIG_dnbits(a);
    if (len % 4 == 0) len /= 4;
    else
    {
        len /= 4;
        len++;
    }

    for (i = len - 1; i >= 0; i--)
    {
        BIG_dcopy(b, a);
        BIG_dshr(b, i * 4);
        printf("%01x", (unsigned int) b[0] & 15);
    }
}


void B336_60::BIG_drawoutput(DBIG a)
{
    int i;
    printf("(");
    for (i = 0; i < DNLEN_B336_60 - 1; i++)
#if CHUNK==64
        printf("%jx,", (uintmax_t) a[i]);
    printf("%jx)", (uintmax_t) a[DNLEN_B336_60 - 1]);
#else
        printf("%x,", (unsigned int) a[i]);
    printf("%x)", (unsigned int) a[DNLEN_B336_60 - 1]);
#endif
}

/* Copy b=a */
void B336_60::BIG_copy(BIG b, BIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_B336_60] = a[MPV_B336_60];
    b[MNV_B336_60] = a[MNV_B336_60];
#endif
}

/* Copy from ROM b=a */
void B336_60::BIG_rcopy(BIG b, const BIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_B336_60] = 1; b[MNV_B336_60] = 0;
#endif
}

/* double length DBIG copy b=a */
void B336_60::BIG_dcopy(DBIG b, DBIG a)
{
    int i;
    for (i = 0; i < DNLEN_B336_60; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[DMPV_B336_60] = a[DMPV_B336_60];
    b[DMNV_B336_60] = a[DMNV_B336_60];
#endif
}

/* Copy BIG to bottom half of DBIG */
void B336_60::BIG_dscopy(DBIG b, BIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60 - 1; i++)
        b[i] = a[i];

    b[NLEN_B336_60 - 1] = a[NLEN_B336_60 - 1] & BMASK_B336_60; /* top word normalized */
    b[NLEN_B336_60] = a[NLEN_B336_60 - 1] >> BASEBITS_B336_60;

    for (i = NLEN_B336_60 + 1; i < DNLEN_B336_60; i++) b[i] = 0;
#ifdef DEBUG_NORM
    b[DMPV_B336_60] = a[MPV_B336_60];
    b[DMNV_B336_60] = a[MNV_B336_60];
#endif
}

/* Copy BIG to top half of DBIG */
void B336_60::BIG_dsucopy(DBIG b, BIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        b[i] = 0;
    for (i = NLEN_B336_60; i < DNLEN_B336_60; i++)
        b[i] = a[i - NLEN_B336_60];
#ifdef DEBUG_NORM
    b[DMPV_B336_60] = a[MPV_B336_60];
    b[DMNV_B336_60] = a[MNV_B336_60];
#endif
}

/* Copy bottom half of DBIG to BIG */
void B336_60::BIG_sdcopy(BIG b, DBIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_B336_60] = a[DMPV_B336_60];
    b[MNV_B336_60] = a[DMNV_B336_60];
#endif
}

/* Copy top half of DBIG to BIG */
void B336_60::BIG_sducopy(BIG b, DBIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        b[i] = a[i + NLEN_B336_60];
#ifdef DEBUG_NORM
    b[MPV_B336_60] = a[DMPV_B336_60];
    b[MNV_B336_60] = a[DMNV_B336_60];

#endif
}

/* Set a=0 */
void B336_60::BIG_zero(BIG a)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[MPV_B336_60] = a[MNV_B336_60] = 0;
#endif
}

void B336_60::BIG_dzero(DBIG a)
{
    int i;
    for (i = 0; i < DNLEN_B336_60; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[DMPV_B336_60] = a[DMNV_B336_60] = 0;
#endif
}

/* set a=1 */
void B336_60::BIG_one(BIG a)
{
    int i;
    a[0] = 1;
    for (i = 1; i < NLEN_B336_60; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[MPV_B336_60] = 1;
    a[MNV_B336_60] = 0;
#endif
}

/* Set c=a+b */
/* SU= 8 */
void B336_60::BIG_add(BIG c, BIG a, BIG b)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        c[i] = a[i] + b[i];
#ifdef DEBUG_NORM
    c[MPV_B336_60] = a[MPV_B336_60] + b[MPV_B336_60];
    c[MNV_B336_60] = a[MNV_B336_60] + b[MNV_B336_60];
    if (c[MPV_B336_60] > NEXCESS_B336_60)  printf("add problem - positive digit overflow %d\n", (int)c[MPV_B336_60]);
    if (c[MNV_B336_60] > NEXCESS_B336_60)  printf("add problem - negative digit overflow %d\n", (int)c[MNV_B336_60]);

#endif
}

/* Set c=a or b */
/* SU= 8 */
void B336_60::BIG_or(BIG c, BIG a, BIG b)
{
    int i;
    BIG_norm(a);
    BIG_norm(b);
    for (i = 0; i < NLEN_B336_60; i++)
        c[i] = a[i] | b[i];
#ifdef DEBUG_NORM
    c[MPV_B336_60] = 1;
    c[MNV_B336_60] = 0;
#endif

}


/* Set c=c+d */
void B336_60::BIG_inc(BIG c, int d)
{
    BIG_norm(c);
    c[0] += (chunk)d;
#ifdef DEBUG_NORM
    c[MPV_B336_60] += 1;
#endif
}

/* Set c=a-b */
/* SU= 8 */
void B336_60::BIG_sub(BIG c, BIG a, BIG b)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++)
        c[i] = a[i] - b[i];
#ifdef DEBUG_NORM
    c[MPV_B336_60] = a[MPV_B336_60] + b[MNV_B336_60];
    c[MNV_B336_60] = a[MNV_B336_60] + b[MPV_B336_60];
    if (c[MPV_B336_60] > NEXCESS_B336_60)  printf("sub problem - positive digit overflow %d\n", (int)c[MPV_B336_60]);
    if (c[MNV_B336_60] > NEXCESS_B336_60)  printf("sub problem - negative digit overflow %d\n", (int)c[MNV_B336_60]);

#endif
}

/* SU= 8 */

void B336_60::BIG_dsub(DBIG c, DBIG a, DBIG b)
{
    int i;
    for (i = 0; i < DNLEN_B336_60; i++)
        c[i] = a[i] - b[i];
#ifdef DEBUG_NORM
    c[DMPV_B336_60] = a[DMPV_B336_60] + b[DMNV_B336_60];
    c[DMNV_B336_60] = a[DMNV_B336_60] + b[DMPV_B336_60];
    if (c[DMPV_B336_60] > NEXCESS_B336_60)  printf("double sub problem - positive digit overflow %d\n", (int)c[DMPV_B336_60]);
    if (c[DMNV_B336_60] > NEXCESS_B336_60)  printf("double sub problem - negative digit overflow %d\n", (int)c[DMNV_B336_60]);
#endif
}

void B336_60::BIG_dadd(DBIG c, DBIG a, DBIG b)
{
    int i;
    for (i = 0; i < DNLEN_B336_60; i++)
        c[i] = a[i] + b[i];
#ifdef DEBUG_NORM
    c[DMPV_B336_60] = a[DMPV_B336_60] + b[DMNV_B336_60];
    c[DMNV_B336_60] = a[DMNV_B336_60] + b[DMPV_B336_60];
    if (c[DMPV_B336_60] > NEXCESS_B336_60)  printf("double add problem - positive digit overflow %d\n", (int)c[DMPV_B336_60]);
    if (c[DMNV_B336_60] > NEXCESS_B336_60)  printf("double add problem - negative digit overflow %d\n", (int)c[DMNV_B336_60]);
#endif
}

/* Set c=c-1 */
void B336_60::BIG_dec(BIG c, int d)
{
    BIG_norm(c);
    c[0] -= (chunk)d;
#ifdef DEBUG_NORM
    c[MNV_B336_60] += 1;
#endif
}

/* multiplication r=a*c by c<=NEXCESS_B336_60 */
void B336_60::BIG_imul(BIG r, BIG a, int c)
{
    int i;
    for (i = 0; i < NLEN_B336_60; i++) r[i] = a[i] * c;
#ifdef DEBUG_NORM
    r[MPV_B336_60] = a[MPV_B336_60] * c;
    r[MNV_B336_60] = a[MNV_B336_60] * c;
    if (r[MPV_B336_60] > NEXCESS_B336_60)  printf("int mul problem - positive digit overflow %d\n", (int)r[MPV_B336_60]);
    if (r[MNV_B336_60] > NEXCESS_B336_60)  printf("int mul problem - negative digit overflow %d\n", (int)r[MNV_B336_60]);

#endif
}

/* multiplication r=a*c by larger integer - c<=FEXCESS */
/* SU= 24 */
chunk B336_60::BIG_pmul(BIG r, BIG a, int c)
{
    int i;
    chunk ak, carry = 0;
    for (i = 0; i < NLEN_B336_60; i++)
    {
        ak = a[i];
        r[i] = 0;
        carry = muladd(ak, (chunk)c, carry, &r[i]);
    }
#ifdef DEBUG_NORM
    r[MPV_B336_60] = 1;
    r[MNV_B336_60] = 0;
#endif
    return carry;
}

/* r/=3 */
/* SU= 16 */
/*
int B336_60::BIG_div3(BIG r)
{
    int i;
    chunk ak,base,carry=0;
    BIG_norm(r);
    base=((chunk)1<<BASEBITS_B336_60);
    for (i=NLEN_B336_60-1; i>=0; i--)
    {
        ak=(carry*base+r[i]);
        r[i]=ak/3;
        carry=ak%3;
    }
    return (int)carry;
}
*/
/* multiplication c=a*b by even larger integer b>FEXCESS, resulting in DBIG */
/* SU= 24 */
void B336_60::BIG_pxmul(DBIG c, BIG a, int b)
{
    int j;
    chunk carry;
    BIG_dzero(c);
    carry = 0;
    for (j = 0; j < NLEN_B336_60; j++)
        carry = muladd(a[j], (chunk)b, carry, &c[j]);
    c[NLEN_B336_60] = carry;
#ifdef DEBUG_NORM
    c[DMPV_B336_60] = 1;
    c[DMNV_B336_60] = 0;
#endif
}

/* .. if you know the result will fit in a BIG, c must be distinct from a and b */
/* SU= 40 */
void B336_60::BIG_smul(BIG c, BIG a, BIG b)
{
    int i, j;
    chunk carry;

    BIG_zero(c);
    for (i = 0; i < NLEN_B336_60; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_B336_60; j++)
        {
            if (i + j < NLEN_B336_60)
                carry = muladd(a[i], b[j], carry, &c[i + j]);
        }
    }
#ifdef DEBUG_NORM
    c[MPV_B336_60] = 1;
    c[MNV_B336_60] = 0;
#endif

}

/* Set c=a*b */
/* SU= 72 */
void B336_60::BIG_mul(DBIG c, BIG a, BIG b)
{
    int i,k;
#ifdef dchunk
    dchunk co,t;
    dchunk s;
    dchunk d[NLEN_B336_60];
    int m;
#endif

//B336_60::BIGMULS++;

#ifdef DEBUG_NORM
    if ((a[MPV_B336_60] != 1 && a[MPV_B336_60] != 0) || a[MNV_B336_60] != 0) printf("First input to mul not normed\n");
    if ((b[MPV_B336_60] != 1 && b[MPV_B336_60] != 0) || b[MNV_B336_60] != 0) printf("Second input to mul not normed\n");
#endif

    /* Faster to Combafy it.. Let the compiler unroll the loops! */

#ifdef COMBA

    /* faster psuedo-Karatsuba method */
#ifdef UNWOUND

    /* Insert output of faster.cpp here */

#else
/*
    t=(dchunk)a[0]*b[0];
    c[0]=(chunk)t & BMASK_B336_60;
    t = t >> BASEBITS_B336_60;
    for (i=1;i<NLEN_B336_60;i++)
    {
        k=0; 
        while (k<=i) {t+=(dchunk)a[k]*b[i-k]; k++;}
        c[i]=(chunk)t & BMASK_B336_60;
        t = t >> BASEBITS_B336_60;
    }

    for (i=NLEN_B336_60;i<2*NLEN_B336_60-1;i++)
    {
        k=i-(NLEN_B336_60-1);
        while (k<=NLEN_B336_60-1) {t+=(dchunk)a[k]*b[i-k]; k++;}
        c[i]=(chunk)t & BMASK_B336_60;
        t = t >> BASEBITS_B336_60;
    }

    c[2 * NLEN_B336_60 - 1] = (chunk)t;
*/

    for (i = 0; i < NLEN_B336_60; i++)
        d[i] = (dchunk)a[i] * b[i];

    s = d[0];
    t = s;
    c[0] = (chunk)t & BMASK_B336_60;
    t = t >> BASEBITS_B336_60;

    for (k = 1; k < NLEN_B336_60; k++)
    {
        s += d[k];
        t += s;
        /*for (i = k; i >= 1 + k / 2; i--) This causes a huge slow down! gcc/g++ optimizer problem (I think) */
        for (i=1+k/2;i<=k;i++) t += (dchunk)(a[i] - a[k - i]) * (b[k - i] - b[i]);
        c[k] = (chunk)t & BMASK_B336_60;
        t = t >> BASEBITS_B336_60;
    }
    for (k = NLEN_B336_60; k < 2 * NLEN_B336_60 - 1; k++)
    {
        s -= d[k - NLEN_B336_60];
        t += s;
        for (i=1+k/2;i<NLEN_B336_60;i++) t += (dchunk)(a[i] - a[k - i]) * (b[k - i] - b[i]);
        c[k] = (chunk)t & BMASK_B336_60;
        t = t >> BASEBITS_B336_60;
    }
    c[2 * NLEN_B336_60 - 1] = (chunk)t;

#endif

#else
    int j;
    chunk carry;
    BIG_dzero(c);
    for (i = 0; i < NLEN_B336_60; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_B336_60; j++)
            carry = muladd(a[i], b[j], carry, &c[i + j]);

        c[NLEN_B336_60 + i] = carry;
    }

#endif

#ifdef DEBUG_NORM
    c[DMPV_B336_60] = 1;
    c[DMNV_B336_60] = 0;
#endif
}

/* Set c=a*a */
/* SU= 80 */
void B336_60::BIG_sqr(DBIG c, BIG a)
{
    int i, j;
#ifdef dchunk
    dchunk t, co;
#endif
//B336_60::BIGSQRS++;
#ifdef DEBUG_NORM
    if ((a[MPV_B336_60] != 1 && a[MPV_B336_60] != 0) || a[MNV_B336_60] != 0) printf("Input to sqr not normed\n");
#endif
    /* Note 2*a[i] in loop below and extra addition */

#ifdef COMBA

#ifdef UNWOUND

    /* Insert output of faster.cpp here */

#else


    t = (dchunk)a[0] * a[0];
    c[0] = (chunk)t & BMASK_B336_60;
    co = t >> BASEBITS_B336_60;

    for (j = 1; j < NLEN_B336_60 - 1; )
    {
        t = (dchunk)a[j] * a[0];
        for (i = 1; i < (j + 1) / 2; i++) {t += (dchunk)a[j - i] * a[i]; }
        t += t; t += co;
        c[j] = (chunk)t & BMASK_B336_60;
        co = t >> BASEBITS_B336_60;
        j++;
        t = (dchunk)a[j] * a[0];
        for (i = 1; i < (j + 1) / 2; i++) {t += (dchunk)a[j - i] * a[i]; }
        t += t; t += co;
        t += (dchunk)a[j / 2] * a[j / 2];
        c[j] = (chunk)t & BMASK_B336_60;
        co = t >> BASEBITS_B336_60;
        j++;
    }

    for (j = NLEN_B336_60 - 1 + NLEN_B336_60 % 2; j < DNLEN_B336_60 - 3; )
    {
        t = (dchunk)a[NLEN_B336_60 - 1] * a[j - NLEN_B336_60 + 1];
        for (i = j - NLEN_B336_60 + 2; i < (j + 1) / 2; i++) {t += (dchunk)a[j - i] * a[i];  }
        t += t; t += co;
        c[j] = (chunk)t & BMASK_B336_60;
        co = t >> BASEBITS_B336_60;
        j++;
        t = (dchunk)a[NLEN_B336_60 - 1] * a[j - NLEN_B336_60 + 1];
        for (i = j - NLEN_B336_60 + 2; i < (j + 1) / 2; i++) {t += (dchunk)a[j - i] * a[i];  }
        t += t; t += co;
        t += (dchunk)a[j / 2] * a[j / 2];
        c[j] = (chunk)t & BMASK_B336_60;
        co = t >> BASEBITS_B336_60;
        j++;
    }

    t = (dchunk)a[NLEN_B336_60 - 2] * a[NLEN_B336_60 - 1];
    t += t; t += co;
    c[DNLEN_B336_60 - 3] = (chunk)t & BMASK_B336_60;
    co = t >> BASEBITS_B336_60;

    t = (dchunk)a[NLEN_B336_60 - 1] * a[NLEN_B336_60 - 1] + co;
    c[DNLEN_B336_60 - 2] = (chunk)t & BMASK_B336_60;
    co = t >> BASEBITS_B336_60;
    c[DNLEN_B336_60 - 1] = (chunk)co;


#endif

#else
    chunk carry;
    BIG_dzero(c);
    for (i = 0; i < NLEN_B336_60; i++)
    {
        carry = 0;
        for (j = i + 1; j < NLEN_B336_60; j++)
            carry = muladd(a[i], a[j], carry, &c[i + j]);
        c[NLEN_B336_60 + i] = carry;
    }

    for (i = 0; i < DNLEN_B336_60; i++) c[i] *= 2;

    for (i = 0; i < NLEN_B336_60; i++)
        c[2 * i + 1] += muladd(a[i], a[i], 0, &c[2 * i]);

    BIG_dnorm(c);
#endif


#ifdef DEBUG_NORM
    c[DMPV_B336_60] = 1;
    c[DMNV_B336_60] = 0;
#endif

}

/* Montgomery reduction */
void B336_60::BIG_monty(BIG a, BIG md, chunk MC, DBIG d)
{
    int i, k;

#ifdef dchunk
    dchunk c,t,s;
    dchunk dd[NLEN_B336_60];
    chunk v[NLEN_B336_60];
#endif


#ifdef DEBUG_NORM
    if ((d[DMPV_B336_60] != 1 && d[DMPV_B336_60] != 0) || d[DMNV_B336_60] != 0) printf("Input to redc not normed\n");
#endif

#ifdef COMBA

#ifdef UNWOUND

    /* Insert output of faster.cpp here */

#else
/*
    t = d[0];
    v[0] = ((chunk)t * MC)&BMASK_B336_60;
    t += (dchunk)v[0] * md[0];
    t = (t >> BASEBITS_B336_60) + d[1];
   
    for (i = 1; i < NLEN_B336_60; i++)
    {
        k=1;
        t += (dchunk)v[0] * md[i];
        while (k<i) {t += (dchunk)v[k]*md[i-k]; k++;}
        v[i] = ((chunk)t * MC)&BMASK_B336_60;
        t += (dchunk)v[i] * md[0];
        t = (t >> BASEBITS_B336_60) + d[i + 1];
    }
    for (i = NLEN_B336_60; i < 2 * NLEN_B336_60 - 1; i++)
    {
        k=i-(NLEN_B336_60-1);
        while (k<=NLEN_B336_60-1) {t += (dchunk)v[k]*md[i-k]; k++;}
        a[i - NLEN_B336_60] = (chunk)t & BMASK_B336_60;
        t = (t >> BASEBITS_B336_60) + d[i + 1];
    }
    a[NLEN_B336_60 - 1] = (chunk)t & BMASK_B336_60;
*/

    t = d[0];
    v[0] = ((chunk)t * MC)&BMASK_B336_60;
    t += (dchunk)v[0] * md[0];
    t = (t >> BASEBITS_B336_60) + d[1];
    s = 0;

    for (k = 1; k < NLEN_B336_60; k++)
    {
        t = t + s + (dchunk)v[0] * md[k];

        for (i=1+k/2;i<k;i++) t += (dchunk)(v[k - i] - v[i]) * (md[i] - md[k - i]);
        v[k] = ((chunk)t * MC)&BMASK_B336_60;
        t += (dchunk)v[k] * md[0];
        t = (t >> BASEBITS_B336_60) + d[k + 1];
        dd[k] = (dchunk)v[k] * md[k];
        s += dd[k];
    }
    for (k = NLEN_B336_60; k < 2 * NLEN_B336_60 - 1; k++)
    {
        t = t + s;
        for (i=1+k/2;i<NLEN_B336_60;i++) t += (dchunk)(v[k - i] - v[i]) * (md[i] - md[k - i]);
        a[k - NLEN_B336_60] = (chunk)t & BMASK_B336_60;
        t = (t >> BASEBITS_B336_60) + d[k + 1];
        s -= dd[k - NLEN_B336_60 + 1];
    }
    a[NLEN_B336_60 - 1] = (chunk)t & BMASK_B336_60;

#endif



#else
    int j;
    chunk m, carry;
    for (i = 0; i < NLEN_B336_60; i++)
    {
        if (MC == -1) m = (-d[i])&BMASK_B336_60;
        else
        {
            if (MC == 1) m = d[i];
            else m = (MC * d[i])&BMASK_B336_60;
        }
        carry = 0;
        for (j = 0; j < NLEN_B336_60; j++)
            carry = muladd(m, md[j], carry, &d[i + j]);
        d[NLEN_B336_60 + i] += carry;
    }
    BIG_sducopy(a, d);
    BIG_norm(a);

#endif

#ifdef DEBUG_NORM
    a[MPV_B336_60] = 1;  a[MNV_B336_60] = 0;
#endif
}

/* General shift left of a by n bits */
/* a MUST be normalised */
/* SU= 32 */
void B336_60::BIG_shl(BIG a, int k)
{
    int i;
    int n = k % BASEBITS_B336_60;
    int m = k / BASEBITS_B336_60;

    a[NLEN_B336_60 - 1] = ((a[NLEN_B336_60 - 1 - m] << n));
    if (NLEN_B336_60 >= m + 2) a[NLEN_B336_60 - 1] |= (a[NLEN_B336_60 - m - 2] >> (BASEBITS_B336_60 - n));

    for (i = NLEN_B336_60 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_B336_60) | (a[i - m - 1] >> (BASEBITS_B336_60 - n));
    a[m] = (a[0] << n)&BMASK_B336_60;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* Fast shift left of a by n bits, where n less than a word, Return excess (but store it as well) */
/* a MUST be normalised */
/* SU= 16 */
int B336_60::BIG_fshl(BIG a, int n)
{
    int i;

    a[NLEN_B336_60 - 1] = ((a[NLEN_B336_60 - 1] << n)) | (a[NLEN_B336_60 - 2] >> (BASEBITS_B336_60 - n)); /* top word not masked */
    for (i = NLEN_B336_60 - 2; i > 0; i--)
        a[i] = ((a[i] << n)&BMASK_B336_60) | (a[i - 1] >> (BASEBITS_B336_60 - n));
    a[0] = (a[0] << n)&BMASK_B336_60;

    return (int)(a[NLEN_B336_60 - 1] >> ((8 * MODBYTES_B336_60) % BASEBITS_B336_60)); /* return excess - only used in ff.c */
}

/* double length left shift of a by k bits - k can be > BASEBITS_B336_60 , a MUST be normalised */
/* SU= 32 */
void B336_60::BIG_dshl(DBIG a, int k)
{
    int i;
    int n = k % BASEBITS_B336_60;
    int m = k / BASEBITS_B336_60;

    a[DNLEN_B336_60 - 1] = ((a[DNLEN_B336_60 - 1 - m] << n)) | (a[DNLEN_B336_60 - m - 2] >> (BASEBITS_B336_60 - n));

    for (i = DNLEN_B336_60 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_B336_60) | (a[i - m - 1] >> (BASEBITS_B336_60 - n));
    a[m] = (a[0] << n)&BMASK_B336_60;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* General shift rightof a by k bits */
/* a MUST be normalised */
/* SU= 32 */
void B336_60::BIG_shr(BIG a, int k)
{
    int i;
    int n = k % BASEBITS_B336_60;
    int m = k / BASEBITS_B336_60;
    for (i = 0; i < NLEN_B336_60 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_B336_60 - n))&BMASK_B336_60);
    if (NLEN_B336_60 > m)  a[NLEN_B336_60 - m - 1] = a[NLEN_B336_60 - 1] >> n;
    for (i = NLEN_B336_60 - m; i < NLEN_B336_60; i++) a[i] = 0;

}

/* Fast combined shift, subtract and norm. Return sign of result */
int B336_60::BIG_ssn(BIG r, BIG a, BIG m)
{
    int i, n = NLEN_B336_60 - 1;
    chunk carry;
    m[0] = (m[0] >> 1) | ((m[1] << (BASEBITS_B336_60 - 1))&BMASK_B336_60);
    r[0] = a[0] - m[0];
    carry = r[0] >> BASEBITS_B336_60;
    r[0] &= BMASK_B336_60;

    for (i = 1; i < n; i++)
    {
        m[i] = (m[i] >> 1) | ((m[i + 1] << (BASEBITS_B336_60 - 1))&BMASK_B336_60);
        r[i] = a[i] - m[i] + carry;
        carry = r[i] >> BASEBITS_B336_60;
        r[i] &= BMASK_B336_60;
    }

    m[n] >>= 1;
    r[n] = a[n] - m[n] + carry;
#ifdef DEBUG_NORM
    r[MPV_B336_60] = 1; r[MNV_B336_60] = 0;
#endif
    return ((r[n] >> (CHUNK - 1)) & 1);
}

/* Faster shift right of a by k bits. Return shifted out part */
/* a MUST be normalised */
/* SU= 16 */
int B336_60::BIG_fshr(BIG a, int k)
{
    int i;
    chunk r = a[0] & (((chunk)1 << k) - 1); /* shifted out part */
    for (i = 0; i < NLEN_B336_60 - 1; i++)
        a[i] = (a[i] >> k) | ((a[i + 1] << (BASEBITS_B336_60 - k))&BMASK_B336_60);
    a[NLEN_B336_60 - 1] = a[NLEN_B336_60 - 1] >> k;
    return (int)r;
}

/* double length right shift of a by k bits - can be > BASEBITS_B336_60 */
/* SU= 32 */
void B336_60::BIG_dshr(DBIG a, int k)
{
    int i;
    int n = k % BASEBITS_B336_60;
    int m = k / BASEBITS_B336_60;
    for (i = 0; i < DNLEN_B336_60 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_B336_60 - n))&BMASK_B336_60);
    a[DNLEN_B336_60 - m - 1] = a[DNLEN_B336_60 - 1] >> n;
    for (i = DNLEN_B336_60 - m; i < DNLEN_B336_60; i++ ) a[i] = 0;
}

/* Split DBIG d into two BIGs t|b. Split happens at n bits, where n falls into NLEN_B336_60 word */
/* d MUST be normalised */
/* SU= 24 */
chunk B336_60::BIG_split(BIG t, BIG b, DBIG d, int n)
{
    int i;
    chunk nw, carry = 0;
    int m = n % BASEBITS_B336_60;

    if (m == 0)
    {
        for (i = 0; i < NLEN_B336_60; i++) b[i] = d[i];
        if (t != b)
        {
            for (i = NLEN_B336_60; i < 2 * NLEN_B336_60; i++) t[i - NLEN_B336_60] = d[i];
            carry = t[NLEN_B336_60 - 1] >> BASEBITS_B336_60;
            t[NLEN_B336_60 - 1] = t[NLEN_B336_60 - 1] & BMASK_B336_60; /* top word normalized */
        }
        return carry;
    }

    for (i = 0; i < NLEN_B336_60 - 1; i++) b[i] = d[i];

    b[NLEN_B336_60 - 1] = d[NLEN_B336_60 - 1] & (((chunk)1 << m) - 1);

    if (t != b)
    {
        carry = (d[DNLEN_B336_60 - 1] << (BASEBITS_B336_60 - m));
        for (i = DNLEN_B336_60 - 2; i >= NLEN_B336_60 - 1; i--)
        {
            nw = (d[i] >> m) | carry;
            carry = (d[i] << (BASEBITS_B336_60 - m))&BMASK_B336_60;
            t[i - NLEN_B336_60 + 1] = nw;
        }
    }
#ifdef DEBUG_NORM
    t[MPV_B336_60] = 1; t[MNV_B336_60] = 0;
    b[MPV_B336_60] = 1; b[MNV_B336_60] = 0;
#endif
    return carry;
}

/* you gotta keep the sign of carry! Look - no branching! */
/* Note that sign bit is needed to disambiguate between +ve and -ve values */
/* normalise BIG - force all digits < 2^BASEBITS_B336_60 */
chunk B336_60::BIG_norm(BIG a)
{
    int i;
    chunk d, carry = 0;
    for (i = 0; i < NLEN_B336_60 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_B336_60;
        carry = d >> BASEBITS_B336_60;
    }
    a[NLEN_B336_60 - 1] = (a[NLEN_B336_60 - 1] + carry);

#ifdef DEBUG_NORM
    a[MPV_B336_60] = 1; a[MNV_B336_60] = 0;
#endif
    return (a[NLEN_B336_60 - 1] >> ((8 * MODBYTES_B336_60) % BASEBITS_B336_60)); /* only used in ff.c */
}

void B336_60::BIG_dnorm(DBIG a)
{
    int i;
    chunk d, carry = 0;
    for (i = 0; i < DNLEN_B336_60 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_B336_60;
        carry = d >> BASEBITS_B336_60;
    }
    a[DNLEN_B336_60 - 1] = (a[DNLEN_B336_60 - 1] + carry);
#ifdef DEBUG_NORM
    a[DMPV_B336_60] = 1; a[DMNV_B336_60] = 0;
#endif
}

/* Compare a and b. Return 1 for a>b, -1 for a<b, 0 for a==b */
/* a and b MUST be normalised before call */
/* sodium constant time implementation */

int B336_60::BIG_comp(BIG a, BIG b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = NLEN_B336_60-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_B336_60) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_B336_60;
    }
    return (int)(gt+gt+eq-1);
}

int B336_60::BIG_dcomp(DBIG a, DBIG b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = DNLEN_B336_60-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_B336_60) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_B336_60;
    }
    return (int)(gt+gt+eq-1);
}

/* return number of bits in a */
/* SU= 8 */
int B336_60::BIG_nbits(BIG a)
{
    int bts, k = NLEN_B336_60 - 1;
    BIG t;
    chunk c;
    BIG_copy(t, a);
    BIG_norm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_B336_60 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

/* SU= 8, Calculate number of bits in a DBIG - output normalised */
int B336_60::BIG_dnbits(DBIG a)
{
    int bts, k = DNLEN_B336_60 - 1;
    DBIG t;
    chunk c;
    BIG_dcopy(t, a);
    BIG_dnorm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_B336_60 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}


/* Set b=b mod c */
/* SU= 16 */
void B336_60::BIG_mod(BIG b, BIG c1)
{
    int k = 0;
    BIG r; /**/
    BIG c;
    BIG_copy(c, c1);

    BIG_norm(b);
    if (BIG_comp(b, c) < 0)
        return;
    do
    {
        BIG_fshl(c, 1);
        k++;
    }
    while (BIG_comp(b, c) >= 0);

    while (k > 0)
    {
        BIG_fshr(c, 1);

// constant time...
        BIG_sub(r, b, c);
        BIG_norm(r);
        BIG_cmove(b, r, 1 - ((r[NLEN_B336_60 - 1] >> (CHUNK - 1)) & 1));
        k--;
    }
}

/* Set a=b mod c, b is destroyed. Slow but rarely used. */
/* SU= 96 */
void B336_60::BIG_dmod(BIG a, DBIG b, BIG c)
{
    int k = 0;
    DBIG m, r;
    BIG_dnorm(b);
    BIG_dscopy(m, c);

    if (BIG_dcomp(b, m) < 0)
    {
        BIG_sdcopy(a, b);
        return;
    }

    do
    {
        BIG_dshl(m, 1);
        k++;
    }
    while (BIG_dcomp(b, m) >= 0);

    while (k > 0)
    {
        BIG_dshr(m, 1);
// constant time...
        BIG_dsub(r, b, m);
        BIG_dnorm(r);
        BIG_dcmove(b, r, 1 - ((r[DNLEN_B336_60 - 1] >> (CHUNK - 1)) & 1));

        k--;
    }
    BIG_sdcopy(a, b);
}

/* Set a=b/c,  b is destroyed. Slow but rarely used. */
/* SU= 136 */

void B336_60::BIG_ddiv(BIG a, DBIG b, BIG c)
{
    int d, k = 0;
    DBIG m, dr;
    BIG e, r;
    BIG_dnorm(b);
    BIG_dscopy(m, c);

    BIG_zero(a);
    BIG_zero(e);
    BIG_inc(e, 1);

    while (BIG_dcomp(b, m) >= 0)
    {
        BIG_fshl(e, 1);
        BIG_dshl(m, 1);
        k++;
    }

    while (k > 0)
    {
        BIG_dshr(m, 1);
        BIG_fshr(e, 1);

        BIG_dsub(dr, b, m);
        BIG_dnorm(dr);
        d = 1 - ((dr[DNLEN_B336_60 - 1] >> (CHUNK - 1)) & 1);
        BIG_dcmove(b, dr, d);

        BIG_add(r, a, e);
        BIG_norm(r);
        BIG_cmove(a, r, d);

        k--;
    }
}

/* SU= 136 */

void B336_60::BIG_sdiv(BIG a, BIG c)
{
    int d, k = 0;
    BIG m, e, b, r;
    BIG_norm(a);
    BIG_copy(b, a);
    BIG_copy(m, c);

    BIG_zero(a);
    BIG_zero(e);
    BIG_inc(e, 1);

    while (BIG_comp(b, m) >= 0)
    {
        BIG_fshl(e, 1);
        BIG_fshl(m, 1);
        k++;
    }

    while (k > 0)
    {
        BIG_fshr(m, 1);
        BIG_fshr(e, 1);

        BIG_sub(r, b, m);
        BIG_norm(r);
        d = 1 - ((r[NLEN_B336_60 - 1] >> (CHUNK - 1)) & 1);
        BIG_cmove(b, r, d);

        BIG_add(r, a, e);
        BIG_norm(r);
        BIG_cmove(a, r, d);
        k--;
    }
}

/* return LSB of a */
int B336_60::BIG_parity(BIG a)
{
    return a[0] % 2;
}

/* return n-th bit of a */
/* SU= 16 */
int B336_60::BIG_bit(BIG a, int n)
{
    if (a[n / BASEBITS_B336_60] & ((chunk)1 << (n % BASEBITS_B336_60))) return 1;
    else return 0;
}

/* return last n bits of a, where n is small < BASEBITS_B336_60 */
/* SU= 16 */
int B336_60::BIG_lastbits(BIG a, int n)
{
    int msk = (1 << n) - 1;
    BIG_norm(a);
    return ((int)a[0])&msk;
}

/* get 8*MODBYTES_B336_60 size random number */
void B336_60::BIG_random(BIG m, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int len = 8 * MODBYTES_B336_60;

    BIG_zero(m);
    /* generate random BIG */
    for (i = 0; i < len; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_shl(m, 1);
        m[0] += b;
        j++;
        j &= 7;
    }

#ifdef DEBUG_NORM
    m[MPV_B336_60] = 1; m[MNV_B336_60] = 0;
#endif
}

/* get random BIG from rng, modulo q. Done one bit at a time, so its portable */

void B336_60::BIG_randomnum(BIG m, BIG q, csprng *rng)
{
    int i, b, j = 0, r = 0;
    DBIG d;
    BIG_dzero(d);
    /* generate random DBIG */
    for (i = 0; i < 2 * BIG_nbits(q); i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_dshl(d, 1);
        d[0] += b;
        j++;
        j &= 7;
    }
    /* reduce modulo a BIG. Removes bias */
    BIG_dmod(m, d, q);
#ifdef DEBUG_NORM
    m[MPV_B336_60] = 1; m[MNV_B336_60] = 0;
#endif
}

/* create randum BIG less than r and less than trunc bits */
void B336_60::BIG_randtrunc(BIG s, BIG r, int trunc, csprng *rng)
{
    BIG_randomnum(s, r, rng);
    if (BIG_nbits(r) > trunc)
        BIG_mod2m(s, trunc);
}

/* Set r=a*b mod m */
/* SU= 96 */
void B336_60::BIG_modmul(BIG r, BIG a1, BIG b1, BIG m)
{
    DBIG d;
    BIG a, b;
    BIG_copy(a, a1);
    BIG_copy(b, b1);
    BIG_mod(a, m);
    BIG_mod(b, m);

    BIG_mul(d, a, b);
    BIG_dmod(r, d, m);
}

/* Set a=a*a mod m */
/* SU= 88 */
void B336_60::BIG_modsqr(BIG r, BIG a1, BIG m)
{
    DBIG d;
    BIG a;
    BIG_copy(a, a1);
    BIG_mod(a, m);
    BIG_sqr(d, a);
    BIG_dmod(r, d, m);
}

/* Set r=-a mod m */
/* SU= 16 */
void B336_60::BIG_modneg(BIG r, BIG a1, BIG m)
{
    BIG a;
    BIG_copy(a, a1);
    BIG_mod(a, m);
    BIG_sub(r, m, a);
    BIG_mod(r, m);
}

/* Set r=a+b mod m */
void B336_60::BIG_modadd(BIG r, BIG a1, BIG b1, BIG m)
{
    BIG a, b;
    BIG_copy(a, a1);
    BIG_copy(b, b1);
    BIG_mod(a, m);
    BIG_mod(b, m);
    BIG_add(r,a,b); BIG_norm(r);
    BIG_mod(r,m);
}

/* Set a=a/b mod m */
/* SU= 136 */
void B336_60::BIG_moddiv(BIG r, BIG a1, BIG b1, BIG m)
{
    DBIG d;
    BIG z;
    BIG a, b;
    BIG_copy(a, a1);
    BIG_copy(b, b1);
    BIG_mod(a, m);
    BIG_invmodp(z, b, m);

    BIG_mul(d, a, z);
    BIG_dmod(r, d, m);
}

/* Get jacobi Symbol (a/p). Returns 0, 1 or -1 */
/* SU= 216 */
int B336_60::BIG_jacobi(BIG a, BIG p)
{
    int n8, k, m = 0;
    BIG t, x, n, zilch, one;
    BIG_one(one);
    BIG_zero(zilch);
    if (BIG_parity(p) == 0 || BIG_comp(a, zilch) == 0 || BIG_comp(p, one) <= 0) return 0;
    BIG_norm(a);
    BIG_copy(x, a);
    BIG_copy(n, p);
    BIG_mod(x, p);

    while (BIG_comp(n, one) > 0)
    {
        if (BIG_comp(x, zilch) == 0) return 0;
        n8 = BIG_lastbits(n, 3);
        k = 0;
        while (BIG_parity(x) == 0)
        {
            k++;
            BIG_shr(x, 1);
        }
        if (k % 2 == 1) m += (n8 * n8 - 1) / 8;
        m += (n8 - 1) * (BIG_lastbits(x, 2) - 1) / 4;
        BIG_copy(t, n);

        BIG_mod(t, x);
        BIG_copy(n, x);
        BIG_copy(x, t);
        m %= 2;

    }
    if (m == 0) return 1;
    else return -1;
}

/* Set r=1/a mod p. Binary method */
/* SU= 240 */
void B336_60::BIG_invmodp(BIG r, BIG a, BIG p)
{
    BIG u, v, x1, x2, t, one;

    BIG_mod(a, p);
    if (BIG_iszilch(a))
    {
        BIG_zero(r);
        return;
    }

    BIG_copy(u, a);
    BIG_copy(v, p);
    BIG_one(one);
    BIG_copy(x1, one);
    BIG_zero(x2);

    while (BIG_comp(u, one) != 0 && BIG_comp(v, one) != 0)
    {
        while (BIG_parity(u) == 0)
        {
            BIG_fshr(u, 1);
            if (BIG_parity(x1) != 0)
            {
                BIG_add(x1, p, x1);
                BIG_norm(x1);
            }
            BIG_fshr(x1, 1);
        }
        while (BIG_parity(v) == 0)
        {
            BIG_fshr(v, 1);
            if (BIG_parity(x2) != 0)
            {
                BIG_add(x2, p, x2);
                BIG_norm(x2);
            }
            BIG_fshr(x2, 1);
        }
        if (BIG_comp(u, v) >= 0)
        {
            BIG_sub(u, u, v);
            BIG_norm(u);
            if (BIG_comp(x1, x2) >= 0) BIG_sub(x1, x1, x2);
            else
            {
                BIG_sub(t, p, x2);
                BIG_add(x1, x1, t);
            }
            BIG_norm(x1);
        }
        else
        {
            BIG_sub(v, v, u);
            BIG_norm(v);
            if (BIG_comp(x2, x1) >= 0) BIG_sub(x2, x2, x1);
            else
            {
                BIG_sub(t, p, x1);
                BIG_add(x2, x2, t);
            }
            BIG_norm(x2);
        }
    }
    if (BIG_comp(u, one) == 0)
        BIG_copy(r, x1);
    else
        BIG_copy(r, x2);
}

/* set x = x mod 2^m */
void B336_60::BIG_mod2m(BIG x, int m)
{
    int i, wd, bt;
    chunk msk;
    BIG_norm(x);

    wd = m / BASEBITS_B336_60;
    bt = m % BASEBITS_B336_60;
    msk = ((chunk)1 << bt) - 1;
    x[wd] &= msk;
    for (i = wd + 1; i < NLEN_B336_60; i++) x[i] = 0;
}

// new
/* Convert to DBIG number from byte array of given length */
void B336_60::BIG_dfromBytesLen(DBIG a, char *b, int s)
{
    int i, len = s;
    BIG_dzero(a);

    for (i = 0; i < len; i++)
    {
        BIG_dshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[DMPV_B336_60] = 1; a[DMNV_B336_60] = 0;
#endif
}
