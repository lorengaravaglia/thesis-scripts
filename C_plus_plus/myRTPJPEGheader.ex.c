#include "myRTPJPEGheader.h"


int getPSNR(IplImage * frameReference,  IplImage * frameUnderTest, double *mse, double *psnr)
{
	int i,j;
	double sse = 0;
	int val;
	if(frameReference->height != frameUnderTest ->height || frameReference->width != frameUnderTest->width)
	{
		printf("Cannot calculate PNSR for the two images becuse they are of different dimentions\n");
		printf("Dimentions of the reference are:\nHeight = %d\nWidth = %d\nnChannel=%d\n",frameReference->height,frameReference->width,frameReference->nChannels);
		printf("Dimentions of the test are:\nHeight = %d\nWidth = %d\nnChannel=%d\n",frameUnderTest->height,frameUnderTest->width,frameUnderTest->nChannels);
		return 0;
	}
	
	for(i=0; i<frameReference->height;i++)
	
		for(j=0;j<frameReference->width;j++)
			sse += pow(cvGet2D(frameReference,i,j).val[0]-cvGet2D(frameUnderTest,i,j).val[0],2);
	
	*mse =sse /(double)(frameReference->width*frameReference->height);

	if(*mse > 1e-10)
	{
		*psnr = 10.0*log10((255*255)/(*mse));
		val = 1;
	}
	else
	{
		*psnr=-100;
		val = 0;
	}
	return val;
}


void printByteArray(byte *a, int size, int elementSize,int lineLength)
{
	int i;
	//noElement = size/elementSize;
	twoBytes element,temp=0xFFFF;

	for(i=0;i<size;i = i + elementSize )
	{
		if(elementSize == 1)
			element = (unsigned short int) a[i];
		else if (elementSize == 2)
		{
			element = temp & a[i] << 8;
			//printf("%4X",temp);
			element = element | 0x00FF;
			//printf("%4X",temp);
			temp = 0xFF00 | a[i+1];
			element = element & temp;
		}

		printf("%6x",element);
		if ((i+1) % lineLength == 0)
			printf("\n");

	}
	printf("\n\n");
}

void printByteArrayAsEachNumberIsOneByte(byte *a, int size, int lineLength)
{
	int i;
	for(i=0;i<size;i++)
	{
		printf("%10hu",(unsigned int) a[i]);
		if (  (i-1) % lineLength == 0)
			printf("\n");
	}
}

void printByteArrayAsEachNumberIsTwoBytes(byte *a, int size, int lineLength)
{
	int i;
	twoBytes temp = (unsigned) 0xFFFF;
	twoBytes tb1 = 0;
	
	for(i=0;i<size;i=i+2)
	{
		//printf("%4X",a[i] << 8);
		temp = temp & a[i] << 8;
		//printf("%4X",temp);
		temp = temp | 0x00FF;
		//printf("%4X",temp);
		tb1 = 0xFF00 | a[i+1];
		temp = temp & tb1;

		//printf("%4X",temp);
		printf("%10hu",temp);
		if ( (i-1) % lineLength == 0)
			printf("\n");
	}
}

void skipBytes(byte **p, int s, int *index)
{
	int i=0;
	for ( i = 0; i< s ; i++)
	{
		*p = *p +1;
		*index = *index +1;
	}
	
}

byte readByte(byte **p, int *index)
{
	byte b;
	b = **p;
	*p = *p +1;
	*index = *index +1;
	return b;
}

twoBytes read2Bytes(byte **p, int *index)
{
	twoBytes b;
	twoBytes m = **p;
	byte l = *(*p+1);
	b = m << 8;
	b = b | l;
	*p=*p+2;//skip the two bytes read
	*index = *index+2;
	return b;
}

byte read3Bytes(byte **p, int *index)
{
	unsigned int b;
	byte u = readByte(p, index);
	twoBytes l2 = read2Bytes(p, index);
	b = u;
	b = b << 16;
	b = b | l2;
	return b;
}

byte read4Bytes(byte **p, int *index)
{
	unsigned int b;
	twoBytes u2 = read2Bytes(p, index);

	twoBytes l2 = read2Bytes(p, index);

	b = u2;
	b = b << 16;
	b = b | l2;
	return b;
}

void printSOF0(SOF0 sof0)
{
	int e;
	printf("marker : %04x\n", sof0.marker);// = m;// 2 bytes 0xff, 0xc0 to identify SOF0 marker
	printf("SOf0 Start of Frame\n");
	printf("length : %d\n",sof0.length);// read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2 ;// 2 bytes This value equals to 8 + components*3 value
	printf("dataPrecision : %d\n",sof0.dataPrecision);// = readByte(&jpgDataScanStart, &jpegFileByteIndex); // 1 byte This is in bits/sample, usually 8 (12 and 16 not supported by most software).
	printf("imageHeight : %d\n",sof0.imageHeight);// = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);// 2 bytes This must be > 0
	printf("imageWidth : %d\n",sof0.imageWidth);// = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);// 2 bytes This must be > 0
	printf("numberOfComponents: %d\n",sof0.numberOfComponents);// = readByte(&jpgDataScanStart, &jpegFileByteIndex); // 1 byte Usually 1 = grey scaled, 3 = color YcbCr or YIQ 4 = color CMYK
	for(e =0; e < sof0.numberOfComponents; e++)
	{
		printf("Component %d:\n",e+1);
		printf("componentId : %d\n",sof0.components[e].componentId);//=readByte(&jpgDataScanStart, &jpegFileByteIndex); //(1byte)(1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q),
		printf("sampling Virtical : %u\n",sof0.components[e].samplingFactors & 0x0f );//=readByte(&jpgDataScanStart, &jpegFileByteIndex);//  (1byte) (bit 0-3 vertical., 4-7 horizontal.),
		printf("sampling horiznntal : %u\n",sof0.components[e].samplingFactors & 0xf0 >> 4 );
		printf("quantizationTableID : %d\n",sof0.components[e].quantizationTableID);//=readByte(&jpgDataScanStart, &jpegFileByteIndex);  
	}
}

void printQuantizationInformation(QTables qtables)
{
	int i;
	for(i=0; i<qtables.qtablesCounter;i++)
	{
		printf("\nQuantization Table:\n");
		printf("Table ID : %u\n",qtables.qtables[i].tableID);
		printf("Table value size : %u\n",qtables.qtables[i].elementSize);
		printf("\nQuantization Values:\n");
		printByteArray(qtables.qtables[i].tableElements, qtables.qtables[i].tableSize,qtables.qtables[i].elementSize, 8);
	}
	printf("\n\n");
}

void printHuffmanInformation(HTables htables)
{
	int i;
	for(i=0; i<htables.htablesCounter;i++)
	{
		printf("\nHuffman Table\n");
		printf("Table ID : %u\n",htables.htables[i].tableID);
		printf("Table Class : %s\n",(htables.htables[i].isAC ? "AC":"DC"));
		printf("\nCount of Huffman Codes:\n");
		printByteArray(htables.htables[i].codeCount, 16, 1, 16);

		printf("\nHuffman Symbols:\n");
		printByteArray(htables.htables[i].tableElements, htables.htables[i].numSym,1, 8);
		

	}
	printf("\n\n");
}


/*
 * Takes two array of bits, and build the huffman table for size, and code
 * 
 * lookup will return the symbol if the code is less or equal than HUFFMAN_HASH_NBITS.
 * code_size will be used to known how many bits this symbol is encoded.
 * slowtable will be used when the first lookup didn't give the result.
 */
void build_huffman_table(HTable * htable)
{

	const unsigned char *bits = htable->codeCount; 
	const unsigned char *vals = htable->tableElements; 
	

  unsigned int i, j, code, code_size, val, nbits;
  unsigned char huffsize[HUFFMAN_BITS_SIZE+1], *hz;
  unsigned int huffcode[HUFFMAN_BITS_SIZE+1], *hc;
  int next_free_entry;

  /*
   * Build a temp array 
   *   huffsize[X] => numbers of bits to write vals[X]
   */
  hz = huffsize;
  for (i=0; i<16; i++)
   {
     for (j=1; j<=bits[i]; j++)
       *hz++ = i + 1;
   }
  *hz = 0;

  //for debug
  //for(i=0;i<257;i++)
	//  printf("%d " , huffsize[i]);
  //printf("\n");

  //printf("sizeof(table->lookup) = %d\n",sizeof(table->lookup));

  memset(htable->lookup, 0xff, sizeof(htable->lookup));
  for (i=0; i<(16-HUFFMAN_HASH_NBITS); i++)
    htable->slowtable[i][0] = 0;

  /* Build a temp array
   *   huffcode[X] => code used to write vals[X]
   */
  code = 0;
  hc = huffcode;
  hz = huffsize;
  nbits = *hz;
  while (*hz)
   {
     while (*hz == nbits)
      {
	*hc++ = code++;
	hz++;
      }
     code <<= 1;
     nbits++;
   }

  //for(i=0;i<257;i++)
	//  printf("%8X " , huffcode[i]);
  //printf("\n");

  /*
   * Build the lookup table, and the slowtable if needed.
   */
  next_free_entry = -1;
  for (i=0; huffsize[i]; i++)
   {
     val = vals[i];
     code = huffcode[i];
     code_size = huffsize[i];

     //printf("val=%2.2x code=%8.8x codesize=%2.2d\n", val, code, code_size);

     htable->code_size[val] = code_size;
     if (code_size <= HUFFMAN_HASH_NBITS)
      {
	/*
	 * Good: val can be put in the lookup table, so fill all value of this
	 * column with value val 
	 */
	int repeat = 1UL<<(HUFFMAN_HASH_NBITS - code_size);

	//printf("repeat = %d\n",repeat);
	code <<= HUFFMAN_HASH_NBITS - code_size;
	while ( repeat-- )
	  htable->lookup[code++] = val;

      }
     else
      {
	/* Perhaps sorting the array will be an optimization */
	uint16_t *slowtable = htable->slowtable[code_size-HUFFMAN_HASH_NBITS-1];
	while(slowtable[0])
	  slowtable+=2;
	slowtable[0] = code;
	slowtable[1] = val;
	slowtable[2] = 0;
	/* TODO: NEED TO CHECK FOR AN OVERFLOW OF THE TABLE */
      }

   }


  //for(i=0;i<sizeof(table->lookup);i++)
	//  printf("(%d,%d) " , i,table->lookup[i]);
  //printf("\n");
}



/**
 * Get the next (valid) huffman code in the stream.
 *
 * To speedup the procedure, we look HUFFMAN_HASH_NBITS bits and the code is
 * lower than HUFFMAN_HASH_NBITS we have automaticaly the length of the code
 * and the value by using two lookup table.
 * Else if the value is not found, just search (linear) into an array for each
 * bits is the code is present.
 *
 * If the code is not present for any reason, -1 is return.
 */
int get_next_huffman_code(myImageStructure *ids,HTable *huffman_table)
{
  int value; 
  unsigned int extra_nbits, nbits,hcode;
  uint16_t *slowtable;

  look_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,HUFFMAN_HASH_NBITS, hcode);
  value = huffman_table->lookup[hcode];
  if (value >= 0)
  { 
     unsigned int code_size = huffman_table->code_size[value];
     skip_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,code_size);
     return value;
  }

  /* Decode more bits each time ... */
  for (extra_nbits=0; extra_nbits<16-HUFFMAN_HASH_NBITS; extra_nbits++)
   {
     nbits = HUFFMAN_HASH_NBITS + 1 + extra_nbits;

     look_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,nbits, hcode);
     slowtable = huffman_table->slowtable[extra_nbits];
     /* Search if the code is in this array */
     while (slowtable[0]) {
	if (slowtable[0] == hcode) {
	   skip_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end, nbits);
	   return slowtable[1];
	}
	slowtable+=2;
     }
   }
  return 0;
}

/**
 *
 * Decode a single block that contains the DCT coefficients.
 * The table coefficients is already dezigzaged at the end of the operation.
 *
 */
void process_Huffman_data_unit(myImageStructure *ids,component *c, int cID)
{
	const unsigned char zigzag[64] = 
{
   0,  1,  5,  6, 14, 15, 27, 28,
   2,  4,  7, 13, 16, 26, 29, 42,
   3,  8, 12, 17, 25, 30, 41, 43,
   9, 11, 18, 24, 31, 40, 44, 53,
  10, 19, 23, 32, 39, 45, 52, 54,
  20, 22, 33, 38, 46, 51, 55, 60,
  21, 34, 37, 47, 50, 56, 59, 61,
  35, 36, 48, 49, 57, 58, 62, 63
};
  unsigned char j;
  unsigned int huff_code;
  unsigned char size_val, count_0;

  byte * startOfunitforbits = ids-> stream;
  byte * startOfunitforbytes = ids-> stream - (int)ceil(ids->nbits_in_reservoir/8.0);

  //struct component *c = &priv->component_infos[component];
  short int DCT[64];

  c->hBlockSizeBits = ids->nbits_in_reservoir;

  /* Initialize the DCT coef table */
  memset(DCT, 0, sizeof(DCT));

  /* DC coefficient decoding */
  huff_code = get_next_huffman_code(ids,ids->componentsInfo[cID].DC_table);
  //printf("+ %x\n", huff_code);
  if (huff_code) {
	get_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,huff_code, DCT[0]);
		DCT[0] += ids->componentsInfo[cID].previous_DC;
		ids->componentsInfo[cID].previous_DC = DCT[0];
  } else {
     DCT[0] = ids->componentsInfo[cID].previous_DC;
  }

  /* AC coefficient decoding */
  j = 1;
  while (j<64)
   {
     huff_code = get_next_huffman_code(ids,ids->componentsInfo[cID].AC_table);
     //printf("- %x\n", huff_code);

     size_val = huff_code & 0xF;
     count_0 = huff_code >> 4;

     if (size_val == 0)
      { /* RLE */
	if (count_0 == 0)
	  break;	/* EOB found, go out */
	else if (count_0 == 0xF)
	  j += 16;	/* skip 16 zeros */
      }
     else
      {
	j += count_0;	/* skip count_0 zeroes */
	if (j >= 64)
	 {
	   //snprintf(error_string, sizeof(error_string), "Bad huffman data (buffer overflow)");
	   break;
	 }
	get_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,size_val, DCT[j]);
	j++;
      }
   }
  c->hBlockSize = ceil(((ids-> stream - (startOfunitforbytes))*8 - ids->nbits_in_reservoir)/8.0) ;
  c->hBlockSizeBits += (ids-> stream - startOfunitforbits)*8 - ids->nbits_in_reservoir;
  
  //c->hBlock = malloc(c->hBlockSize);
  //memcpy(c->hBlock,startOfunitforbytes,c->hBlockSize);

  
 	for (j = 0; j < 64; j++)
		c->DCT[j] = DCT[zigzag[j]];
	c->huffmanDecodedFalg = 1;
  
}



void process_Huffman_data_unit_PKT(myImageStructure *ids,component *c, int cID)
{
  unsigned char j;
  unsigned int huff_code;
  unsigned char size_val, count_0;

  byte * startOfunitforbits = ids-> stream;
  byte * startOfunitforbytes = ids-> stream - (int)ceil(ids->nbits_in_reservoir/8.0);

  //struct component *c = &priv->component_infos[component];
  short int DCT[64];

  c->hBlockSizeBits = ids->nbits_in_reservoir;

  /* Initialize the DCT coef table */
  memset(DCT, 0, sizeof(DCT));

  /* DC coefficient decoding */
  huff_code = get_next_huffman_code(ids,ids->componentsInfo[cID].DC_table);
  //printf("+ %x\n", huff_code);
  if (huff_code) {
	get_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,huff_code, DCT[0]);
		DCT[0] += ids->componentsInfo[cID].previous_DC;
		ids->componentsInfo[cID].previous_DC = DCT[0];
  } else {
     DCT[0] = ids->componentsInfo[cID].previous_DC;
  }

  /* AC coefficient decoding */
  j = 1;
  while (j<64)
   {
     huff_code = get_next_huffman_code(ids,ids->componentsInfo[cID].AC_table);
     //printf("- %x\n", huff_code);

     size_val = huff_code & 0xF;
     count_0 = huff_code >> 4;

     if (size_val == 0)
      { /* RLE */
	if (count_0 == 0)
	  break;	/* EOB found, go out */
	else if (count_0 == 0xF)
	  j += 16;	/* skip 16 zeros */
      }
     else
      {
	j += count_0;	/* skip count_0 zeroes */
	if (j >= 64)
	 {
	   //snprintf(error_string, sizeof(error_string), "Bad huffman data (buffer overflow)");
	   break;
	 }
	get_nbits(ids->reservoir, ids->nbits_in_reservoir, ids->stream, ids->stream_end,size_val, DCT[j]);
	j++;
      }
   }
  c->hBlockSize = ceil(((ids-> stream - (startOfunitforbytes))*8 - ids->nbits_in_reservoir)/8.0) ;
  c->hBlockSizeBits += (ids-> stream - startOfunitforbits)*8 - ids->nbits_in_reservoir;
  
  //c->hBlock = malloc(c->hBlockSize);
  //memcpy(c->hBlock,startOfunitforbytes,c->hBlockSize);

  
 //	for (j = 0; j < 64; j++)
//		c->DCT[j] = DCT[zigzag[j]];
  
}


void build_quantization_table(QTable *qtable)
{
	const unsigned char zigzag[64] = 
{
   0,  1,  5,  6, 14, 15, 27, 28,
   2,  4,  7, 13, 16, 26, 29, 42,
   3,  8, 12, 17, 25, 30, 41, 43,
   9, 11, 18, 24, 31, 40, 44, 53,
  10, 19, 23, 32, 39, 45, 52, 54,
  20, 22, 33, 38, 46, 51, 55, 60,
  21, 34, 37, 47, 50, 56, 59, 61,
  35, 36, 48, 49, 57, 58, 62, 63
};
  /* Taken from libjpeg. Copyright Independent JPEG Group's LLM idct.
   * For float AA&N IDCT method, divisors are equal to quantization
   * coefficients scaled by scalefactor[row]*scalefactor[col], where
   *   scalefactor[0] = 1
   *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
   * We apply a further scale factor of 8.
   * What's actually stored is 1/divisor so that the inner loop can
   * use a multiplication rather than a division.
   */

	int i, j;
		static const double aanscalefactor[8] = {
			1.0, 1.387039845, 1.306562965, 1.175875602,
			1.0, 0.785694958, 0.541196100, 0.275899379
		};
	const unsigned char *zz = zigzag;

	float *fqtable = qtable->ftableElements;
	const unsigned char *ref_table = qtable->tableElements;
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			*fqtable++ = ref_table[*zz++] * aanscalefactor[i] * aanscalefactor[j];
		}
	}
}


/*
 * jidctflt.c
 *
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 *
 * The authors make NO WARRANTY or representation, either express or implied,
 * with respect to this software, its quality, accuracy, merchantability, or 
 * fitness for a particular purpose.  This software is provided "AS IS", and you,
 * its user, assume the entire risk as to its quality and accuracy.
 *
 * This software is copyright (C) 1991-1998, Thomas G. Lane.
 * All Rights Reserved except as specified below.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * software (or portions thereof) for any purpose, without fee, subject to these
 * conditions:
 * (1) If any part of the source code for this software is distributed, then this
 * README file must be included, with this copyright and no-warranty notice
 * unaltered; and any additions, deletions, or changes to the original files
 * must be clearly indicated in accompanying documentation.
 * (2) If only executable code is distributed, then the accompanying
 * documentation must state that "this software is based in part on the work of
 * the Independent JPEG Group".
 * (3) Permission for use of this software is granted only if the user accepts
 * full responsibility for any undesirable consequences; the authors accept
 * NO LIABILITY for damages of any kind.
 * 
 * These conditions apply to any software derived from or based on the IJG code,
 * not just to the unmodified library.  If you use our work, you ought to
 * acknowledge us.
 * 
 * Permission is NOT granted for the use of any IJG author's name or company name
 * in advertising or publicity relating to this software or products derived from
 * it.  This software may be referred to only as "the Independent JPEG Group's
 * software".
 * 
 * We specifically permit and encourage the use of this software as the basis of
 * commercial products, provided that all warranty or liability claims are
 * assumed by the product vendor.
 *
 *
 * This file contains a floating-point implementation of the
 * inverse DCT (Discrete Cosine Transform).  In the IJG code, this routine
 * must also perform dequantization of the input coefficients.
 *
 * This implementation should be more accurate than either of the integer
 * IDCT implementations.  However, it may not give the same results on all
 * machines because of differences in roundoff behavior.  Speed will depend
 * on the hardware's floating point capacity.
 *
 * A 2-D IDCT can be done by 1-D IDCT on each column followed by 1-D IDCT
 * on each row (or vice versa, but it's more convenient to emit a row at
 * a time).  Direct algorithms are also available, but they are much more
 * complex and seem not to be any faster when reduced to code.
 *
 * This implementation is based on Arai, Agui, and Nakajima's algorithm for
 * scaled DCT.  Their original paper (Trans. IEICE E-71(11):1095) is in
 * Japanese, but the algorithm is described in the Pennebaker & Mitchell
 * JPEG textbook (see REFERENCES section in file README).  The following code
 * is based directly on figure 4-8 in P&M.
 * While an 8-point DCT cannot be done in less than 11 multiplies, it is
 * possible to arrange the computation so that many of the multiplies are
 * simple scalings of the final outputs.  These multiplies can then be
 * folded into the multiplications or divisions by the JPEG quantization
 * table entries.  The AA&N method leaves only 5 multiplies and 29 adds
 * to be done in the DCT itself.
 * The primary disadvantage of this method is that with a fixed-point
 * implementation, accuracy is lost due to imprecise representation of the
 * scaled quantization values.  However, that problem does not arise if
 * we use floating point arithmetic.
 */
#define FAST_FLOAT float
#define DCTSIZE	   8
#define DCTSIZE2   (DCTSIZE*DCTSIZE)

#define DEQUANTIZE(coef,quantval)  (((FAST_FLOAT) (coef)) * (quantval))

#if 1 && defined(__GNUC__) && (defined(__i686__) || defined(__x86_64__))

unsigned char descale_and_clamp(int x, int shift)
{
  __asm__ (
      "add %3,%1\n"
      "\tsar %2,%1\n"
      "\tsub $-128,%1\n"
      "\tcmovl %5,%1\n"	/* Use the sub to compare to 0 */
      "\tcmpl %4,%1\n" 
      "\tcmovg %4,%1\n"
      : "=r"(x) 
      : "0"(x), "Ir"(shift), "ir"(1UL<<(shift-1)), "r" (0xff), "r" (0)
      );
  return x;
}

#else
unsigned char descale_and_clamp(int x, int shift)
{
  x += (1UL<<(shift-1));
  if (x<0)
    x = (x >> shift) | ((~(0UL)) << (32-(shift)));
  else
    x >>= shift;
  x += 128;
  if (x>255)
    return 255;
  else if (x<0)
    return 0;
  else 
    return x;
}
#endif


/*
 * Perform dequantization and inverse DCT on one block of coefficients.
 */

void
IDCT (component *compptr,  int stride)
{
  FAST_FLOAT tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  FAST_FLOAT tmp10, tmp11, tmp12, tmp13;
  FAST_FLOAT z5, z10, z11, z12, z13;
  int16_t *inptr;
  FAST_FLOAT *quantptr;
  FAST_FLOAT *wsptr;
  uint8_t *outptr;
  int ctr;
  uint8_t *output_buf = compptr->block;
  FAST_FLOAT workspace[DCTSIZE2]; /* buffers data between passes */

  /* Pass 1: process columns from input, store into work array. */

  inptr = compptr->DCT;
  quantptr = compptr->Q_table;
  wsptr = workspace;
  for (ctr = DCTSIZE; ctr > 0; ctr--) {
    /* Due to quantization, we will usually find that many of the input
     * coefficients are zero, especially the AC terms.  We can exploit this
     * by short-circuiting the IDCT calculation for any column in which all
     * the AC terms are zero.  In that case each output is equal to the
     * DC coefficient (with scale factor as needed).
     * With typical images and quantization tables, half or more of the
     * column DCT calculations can be simplified this way.
     */
    
    if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*2] == 0 &&
	inptr[DCTSIZE*3] == 0 && inptr[DCTSIZE*4] == 0 &&
	inptr[DCTSIZE*5] == 0 && inptr[DCTSIZE*6] == 0 &&
	inptr[DCTSIZE*7] == 0) {
      /* AC terms all zero */
      FAST_FLOAT dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
      
      wsptr[DCTSIZE*0] = dcval;
      wsptr[DCTSIZE*1] = dcval;
      wsptr[DCTSIZE*2] = dcval;
      wsptr[DCTSIZE*3] = dcval;
      wsptr[DCTSIZE*4] = dcval;
      wsptr[DCTSIZE*5] = dcval;
      wsptr[DCTSIZE*6] = dcval;
      wsptr[DCTSIZE*7] = dcval;
      
      inptr++;			/* advance pointers to next column */
      quantptr++;
      wsptr++;
      continue;
    }
    
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    tmp3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp10 = tmp0 + tmp2;	/* phase 3 */
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;	/* phases 5-3 */
    tmp12 = (tmp1 - tmp3) * ((FAST_FLOAT) 1.414213562) - tmp13; /* 2*c4 */

    tmp0 = tmp10 + tmp13;	/* phase 2 */
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;
    
    /* Odd part */

    tmp4 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    tmp5 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    tmp6 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp7 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    z13 = tmp6 + tmp5;		/* phase 6 */
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;		/* phase 5 */
    tmp11 = (z11 - z13) * ((FAST_FLOAT) 1.414213562); /* 2*c4 */

    z5 = (z10 + z12) * ((FAST_FLOAT) 1.847759065); /* 2*c2 */
    tmp10 = ((FAST_FLOAT) 1.082392200) * z12 - z5; /* 2*(c2-c6) */
    tmp12 = ((FAST_FLOAT) -2.613125930) * z10 + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;	/* phase 2 */
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    wsptr[DCTSIZE*0] = tmp0 + tmp7;
    wsptr[DCTSIZE*7] = tmp0 - tmp7;
    wsptr[DCTSIZE*1] = tmp1 + tmp6;
    wsptr[DCTSIZE*6] = tmp1 - tmp6;
    wsptr[DCTSIZE*2] = tmp2 + tmp5;
    wsptr[DCTSIZE*5] = tmp2 - tmp5;
    wsptr[DCTSIZE*4] = tmp3 + tmp4;
    wsptr[DCTSIZE*3] = tmp3 - tmp4;

    inptr++;			/* advance pointers to next column */
    quantptr++;
    wsptr++;
  }
  
  /* Pass 2: process rows from work array, store into output array. */
  /* Note that we must descale the results by a factor of 8 == 2**3. */

  wsptr = workspace;
  outptr = output_buf;
  for (ctr = 0; ctr < DCTSIZE; ctr++) {
    /* Rows of zeroes can be exploited in the same way as we did with columns.
     * However, the column calculation has created many nonzero AC terms, so
     * the simplification applies less often (typically 5% to 10% of the time).
     * And testing floats for zero is relatively expensive, so we don't bother.
     */
    
    /* Even part */

    tmp10 = wsptr[0] + wsptr[4];
    tmp11 = wsptr[0] - wsptr[4];

    tmp13 = wsptr[2] + wsptr[6];
    tmp12 = (wsptr[2] - wsptr[6]) * ((FAST_FLOAT) 1.414213562) - tmp13;

    tmp0 = tmp10 + tmp13;
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;

    /* Odd part */

    z13 = wsptr[5] + wsptr[3];
    z10 = wsptr[5] - wsptr[3];
    z11 = wsptr[1] + wsptr[7];
    z12 = wsptr[1] - wsptr[7];

    tmp7 = z11 + z13;
    tmp11 = (z11 - z13) * ((FAST_FLOAT) 1.414213562);

    z5 = (z10 + z12) * ((FAST_FLOAT) 1.847759065); /* 2*c2 */
    tmp10 = ((FAST_FLOAT) 1.082392200) * z12 - z5; /* 2*(c2-c6) */
    tmp12 = ((FAST_FLOAT) -2.613125930) * z10 + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    /* Final output stage: scale down by a factor of 8 and range-limit */

    outptr[0] = descale_and_clamp((int)(tmp0 + tmp7), 3);
    outptr[7] = descale_and_clamp((int)(tmp0 - tmp7), 3);
    outptr[1] = descale_and_clamp((int)(tmp1 + tmp6), 3);
    outptr[6] = descale_and_clamp((int)(tmp1 - tmp6), 3);
    outptr[2] = descale_and_clamp((int)(tmp2 + tmp5), 3);
    outptr[5] = descale_and_clamp((int)(tmp2 - tmp5), 3);
    outptr[4] = descale_and_clamp((int)(tmp3 + tmp4), 3);
    outptr[3] = descale_and_clamp((int)(tmp3 - tmp4), 3);

    
    wsptr += DCTSIZE;		/* advance pointer to next row */
    outptr += stride;
  }
  
}

//for now assume image is gray
void decode(myImageStructure * ids)
{
	int w,h,i=0;
	
	for(h=0; h < ceil(ids->sof0.imageHeight/8.0);h++)
		for(w = 0; w < ceil(ids->sof0.imageWidth/8.0);w++)
		{
			process_Huffman_data_unit(ids,&ids->components[0][i],0); //this will fill hblock information in the component variable
			
			//fill in required data for dequantization
			ids->components[0][i].AC_table = ids->componentsInfo[0].AC_table;
			ids->components[0][i].DC_table = ids->componentsInfo[0].DC_table;
			ids->components[0][i].Hfactor = ids->componentsInfo[0].Hfactor;
			ids->components[0][i].Q_table=ids->componentsInfo[0].Q_table;
			ids->components[0][i].Vfactor=ids->componentsInfo[0].Vfactor;
			IDCT(&ids->components[0][i], 8);
			i++;
		}
		//ids->rtpPacketsNeeded = packetCounter;
}

void decodeHuffman(myImageStructure * ids)
{
	int w,h,i=0;

	for(h=0; h < ceil(ids->sof0.imageHeight/8.0);h++)
		for(w = 0; w < ceil(ids->sof0.imageWidth/8.0);w++)
		{
			if(ids->components[0][i].huffmanDecodedFalg)
			{
				i++;
				continue;
			}
			process_Huffman_data_unit(ids,&ids->components[0][i],0); //this will fill hblock information in the component variable
			//fill in required data for dequantization
			ids->components[0][i].AC_table = ids->componentsInfo[0].AC_table;
			ids->components[0][i].DC_table = ids->componentsInfo[0].DC_table;
			ids->components[0][i].Hfactor = ids->componentsInfo[0].Hfactor;
			ids->components[0][i].Q_table=ids->componentsInfo[0].Q_table;
			ids->components[0][i].Vfactor=ids->componentsInfo[0].Vfactor;
			//ids->components[0][i].huffmanDecodedFalg = 1;//mark the component as huffman decoded
		}
		ids->huffmanDecodedFalg = 1;//mark the image as huffman decoded
}


int decodeHuffman_PKT(myImageStructure * ids, int includeQuantizationFlag)
{
	//i will fill the packets here to save some time
	int RTPhead = 23;
	int RTPheadWithQ = 23 + 64;
	int packetCounter = 0;
	int currentPacketSize = 0;
	int w,h,i=0;
	unsigned int temp_nbits_in_reservoir = ids->nbits_in_reservoir;  
	unsigned int temp_reservoir = ids->reservoir;

	currentPacketSize = RTPhead *8 + 64*8; //initilaize current PacketSize for packet 0;
	for(h=0; h < ceil(ids->sof0.imageHeight/8.0);h++)
		for(w = 0; w < ceil(ids->sof0.imageWidth/8.0);w++)
		{
			process_Huffman_data_unit_PKT(ids,&ids->components[0][i],0); //this will fill hblock information in the component variable
			//fill in required data for dequantization moved to reconstruct 
			//ids->components[0][i].AC_table = ids->componentsInfo[0].AC_table;
			//ids->components[0][i].DC_table = ids->componentsInfo[0].DC_table;
			//ids->components[0][i].Hfactor = ids->componentsInfo[0].Hfactor;
			//ids->components[0][i].Q_table=ids->componentsInfo[0].Q_table;
			//ids->components[0][i].Vfactor=ids->componentsInfo[0].Vfactor;
			if(currentPacketSize + ids->components[0][i].hBlockSizeBits < ids->rtpPacketSize) //check if this component fit in the current packet
			{
				ids->components[0][i].packetID = packetCounter;
				currentPacketSize += ids->components[0][i].hBlockSizeBits;
			}
			else
			{ //reset for next packet
				currentPacketSize = RTPhead *8;
				if(includeQuantizationFlag)
					currentPacketSize += 64*8;
				packetCounter++;
				ids->components[0][i].packetID = packetCounter;
				currentPacketSize += ids->components[0][i].hBlockSizeBits;
			}
			i++;
		}
		ids->rtpPacketsNeeded = (packetCounter>0?packetCounter+1:1);
		//ids->rtpPacketsNeeded = (packetCounter>0?packetCounter:1);
		ids->pcketizedFlag = 1;

		ids->stream = ids->stream_start;
		ids->nbits_in_reservoir = temp_nbits_in_reservoir;
		ids->reservoir=temp_reservoir;
		return currentPacketSize;
}

//for now assume image is gray
void decodeQuantization(myImageStructure * ids)
{
	int w,h,i=0;
	if(ids->dequantizedFlag)//image is already dequantized
		return;
	for(h=0; h < ceil(ids->sof0.imageHeight/8.0);h++)
		for(w = 0; w < ceil(ids->sof0.imageWidth/8.0);w++)
		{
			IDCT(&ids->components[0][i], 8);
			i++;
		}
	ids->dequantizedFlag=1;
		
}

void deleteImageDataStructure(myImageStructure *ids)
{
	int i =0;
	for(i=0;i<ids->sof0.numberOfComponents;i++)
		free(ids->components[i]);
	for(i = 0; i<ids->htables.htablesCounter;i++)
		free(ids->htables.htables[i].tableElements);

	free(ids->stream_start);
	free(ids);
}


myImageStructure * createImageDataStructure(char fileName[], int packetSize)
{
	FILE *file;
	byte *buffer;
	unsigned long fileLen;
	int e, length, leftlength, jpegFileByteIndex =0;
	twoBytes m;
	byte *jpgDataScanStart, temp;

	int mode = 0;

	myImageStructure *ids = (myImageStructure *) calloc(1,sizeof(myImageStructure));
	if (!ids)
		{
			fprintf(stderr, "Memory error!");
			return NULL;
		}

	ids->dequantizedFlag = 0;
	//ids->huffmanDecodedFalg=0;
	ids->pcketizedFlag=0;
	ids->qtables.qtablesCounter = 0;
	ids->htables.htablesCounter = 0;
	ids->rtpPacketSize = packetSize;

	if(mode == 0) //creat from image file
	{
		file = fopen(fileName, "rb");
		if (!file)
		{
			fprintf(stderr, "Unable to open file %s",fileName);
			return NULL;
		}

		//Get file length
		fseek(file, 0, SEEK_END);
		fileLen=ftell(file);
		rewind (file);
	
		//printf("%d\n",(int)fileLen);
		//Allocate memory
		buffer=(char *)malloc(fileLen+1);
		//bufferRTP = (char *)malloc(fileLen+1);// for now i will assume that I know the image size to receive 
		if (!buffer)
		{
			fprintf(stderr, "Memory error!");
			fclose(file);
			return NULL;
		}
		fread(buffer,1,fileLen,file);
		jpgDataScanStart = buffer;
		fclose(file);

		//start splitting data
		//jpgFileHeader=buffer;

		jpgDataScanStart = buffer;
	
		m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
		if(m != 0xFFD8) 
		{
			//printf("I image %s is not a jpeg image\n", fileName);
			deleteImageDataStructure(ids);
			return NULL;
		}
		m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
		while( m!= 0xFFDA )//looking for the start of scan
		{
			//printf("m = %4X\n",m);
			//e = getchar();
			switch(m)
			{
			case 0xFFE0://JFIF header
				ids->JFIFdata.length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex) - 2;
				for(e = 0; e < 5; e++)
					ids->JFIFdata.identifier[e] = readByte(&jpgDataScanStart, &jpegFileByteIndex);   /* 06h  "JFIF" (zero terminated) Id String */
				ids->JFIFdata.version=read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);      /* 07h  JFIF Format Revision      */
				ids->JFIFdata.units = readByte(&jpgDataScanStart, &jpegFileByteIndex);           /* 09h  Units used for Resolution */
				ids->JFIFdata.xDensity=read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);     /* 0Ah  Horizontal Resolution     */
				ids->JFIFdata.yDensity=read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);     /* 0Ch  Vertical Resolution       */
				ids->JFIFdata.xThumbnail=0;      /* 0Eh  Horizontal Pixel Count    */
				ids->JFIFdata.yThumbnail=0;      /* 0Fh  Vertical Pixel Count      */
				skipBytes(&jpgDataScanStart, ids->JFIFdata.length-12,&jpegFileByteIndex);
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;
			case 0xFFE1://APPn	variable size	Application-specific	For example, an Exif JPEG file uses an APP1 marker to store metadata, laid out in a structure based closely on TIFF.
			case 0xFFE2:
			case 0xFFE3:
			case 0xFFE4:
			case 0xFFE5:
			case 0xFFE6:
			case 0xFFE7:
			case 0xFFE8:
			case 0xFFE9:
			case 0xFFEA:
			case 0xFFEB:
			case 0xFFEC:
			case 0xFFED:
			case 0xFFEE:
			case 0xFFEF:
				length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex) - 2;
				//printf("length is %4x = %d\n",length, (unsigned int) length);
				skipBytes(&jpgDataScanStart, length, &jpegFileByteIndex);
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;
			case 0xFFC0://SOF0		variable size	Start Of Frame (Baseline DCT)	Indicates that this is a baseline DCT-based JPEG, and specifies the width, height, number of components, and component subsampling (e.g., 4:2:0). 
				ids->sof0.marker = m;// 2 bytes 0xff, 0xc0 to identify SOF0 marker
				ids->sof0.length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2 ;// 2 bytes This value equals to 8 + components*3 value
				ids->sof0.dataPrecision = readByte(&jpgDataScanStart, &jpegFileByteIndex); // 1 byte This is in bits/sample, usually 8 (12 and 16 not supported by most software).
				ids->sof0.imageHeight = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);// 2 bytes This must be > 0
				ids->sof0.imageWidth = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);// 2 bytes This must be > 0
				ids->sof0.numberOfComponents = readByte(&jpgDataScanStart, &jpegFileByteIndex); // 1 byte Usually 1 = grey scaled, 3 = color YcbCr or YIQ 4 = color CMYK
				for(e =0; e < ids->sof0.numberOfComponents; e++)
				{
					ids->sof0.components[e].componentId=readByte(&jpgDataScanStart, &jpegFileByteIndex); //(1byte)(1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q),
					ids->sof0.components[e].samplingFactors=readByte(&jpgDataScanStart, &jpegFileByteIndex);//  (1byte) (bit 0-3 vertical., 4-7 horizontal.),
					ids->sof0.components[e].quantizationTableID=readByte(&jpgDataScanStart, &jpegFileByteIndex);  
				}
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;
			case 0xFFFE:
				length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2;
				skipBytes(&jpgDataScanStart, length, &jpegFileByteIndex);
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;
	
			case 0xFFdd:	/* define restart interval		*/
				//for now i will skip it.
				length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2;
				//printf("length is %4x = %d\n",length, (unsigned int) length);
				//skipBytes(&jpgDataScanStart, length-2, &jpegFileByteIndex);
				ids->restartInterval = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				//printf("restart interval is %4x = %d\n",restartInterval, (unsigned int) restartInterval);
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;

			case 0xFFC1:case 0xFFC2:case 0xFFC3:case 0xFFC5:case 0xFFC6:case 0xFFC7:
			case 0xFFC8:case 0xFFC9:case 0xFFCA:case 0xFFCB:case 0xFFCC:case 0xFFCD:case 0xFFCE:case 0xFFCF://start of unsuported frame
				//printf("Image %s is in an unsuported jpeg image\n", fileName);
				return 0;
				break;
	
			case 0xffdb://quantization table
				length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2;
				//printf("length is %4x = %d\n",length, (unsigned int) length);
				temp = readByte(&jpgDataScanStart, &jpegFileByteIndex);
				leftlength = length - 1;
				ids->qtables.qtables[ids->qtables.qtablesCounter].elementSize = ((temp & 0xF0) >> 4==0?1:2);
				ids->qtables.qtables[ids->qtables.qtablesCounter].tableID= temp & 0x0F;
				ids->qtables.qtables[ids->qtables.qtablesCounter].tableSize = 64 * ids->qtables.qtables[ids->qtables.qtablesCounter].elementSize ;
				// changed to static array qtables.qtables[qtables.qtablesCounter].tableElements = malloc(qtables.qtables[qtables.qtablesCounter].tableSize);
				//changed to static array qtables.qtables[qtables.qtablesCounter].ftableElements = (float *)calloc(qtables.qtables[qtables.qtablesCounter].tableSize, sizeof(float));
				for(e = 0; e < ids->qtables.qtables[ids->qtables.qtablesCounter].tableSize; e++)
					ids->qtables.qtables[ids->qtables.qtablesCounter].tableElements[e] = readByte(&jpgDataScanStart, &jpegFileByteIndex);
	
				build_quantization_table(&ids->qtables.qtables[ids->qtables.qtablesCounter]);
				leftlength = leftlength - 64 * ids->qtables.qtables[ids->qtables.qtablesCounter].elementSize;
				ids->qtables.qtablesCounter++;
				while(leftlength > 0)
				{
					temp = readByte(&jpgDataScanStart, &jpegFileByteIndex);
					leftlength = length - 1;
					ids->qtables.qtables[ids->qtables.qtablesCounter].elementSize = ((temp & 0xF0) >> 4==0?1:2);
					ids->qtables.qtables[ids->qtables.qtablesCounter].tableID= temp & 0x0F;
					ids->qtables.qtables[ids->qtables.qtablesCounter].tableSize = 64 * ids->qtables.qtables[ids->qtables.qtablesCounter].elementSize ;
					// changed to static array qtables.qtables[qtables.qtablesCounter].tableElements = malloc(qtables.qtables[qtables.qtablesCounter].tableSize);
					//changed to static array qtables.qtables[qtables.qtablesCounter].ftableElements = (float *)calloc(qtables.qtables[qtables.qtablesCounter].tableSize, sizeof(float));
					for(e = 0; e < ids->qtables.qtables[ids->qtables.qtablesCounter].tableSize; e++)
						ids->qtables.qtables[ids->qtables.qtablesCounter].tableElements[e] = readByte(&jpgDataScanStart, &jpegFileByteIndex);
	
					build_quantization_table(&ids->qtables.qtables[ids->qtables.qtablesCounter]);
					leftlength = leftlength - 64 * ids->qtables.qtables[ids->qtables.qtablesCounter].elementSize;
					ids->qtables.qtablesCounter++;
				}
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;
			case 0xFFC4:
				length = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2;
				//printf("length of hufmann table is %4x = %d\n",length, (unsigned int) length);
	
				leftlength = length - 1;
				temp = readByte(&jpgDataScanStart, &jpegFileByteIndex);
				ids->htables.htables[ids->htables.htablesCounter].isAC = (temp & 0xF0) >> 4;
				ids->htables.htables[ids->htables.htablesCounter].tableID = temp & 0x0F;
				ids->htables.htables[ids->htables.htablesCounter].numSym = 0;
				for(e = 0; e<16;e++)
				{	
					ids->htables.htables[ids->htables.htablesCounter].codeCount[e]= readByte(&jpgDataScanStart, &jpegFileByteIndex);
					ids->htables.htables[ids->htables.htablesCounter].numSym += (unsigned int) ids->htables.htables[ids->htables.htablesCounter].codeCount[e];
				}
				leftlength = leftlength - 16;
			
				ids->htables.htables[ids->htables.htablesCounter].tableElements = malloc(ids->htables.htables[ids->htables.htablesCounter].numSym);
				
				for(e=0;e<ids->htables.htables[ids->htables.htablesCounter].numSym;e++)
					ids->htables.htables[ids->htables.htablesCounter].tableElements[e]=readByte(&jpgDataScanStart, &jpegFileByteIndex);
				leftlength = leftlength - ids->htables.htables[ids->htables.htablesCounter].numSym;
				build_huffman_table(&ids->htables.htables[ids->htables.htablesCounter]);
				ids->htables.htablesCounter++;
				while(leftlength > 0)
				{
					leftlength = length - 1;
					temp = readByte(&jpgDataScanStart, &jpegFileByteIndex);
					ids->htables.htables[ids->htables.htablesCounter].isAC = (temp & 0xF0) >> 4;
					ids->htables.htables[ids->htables.htablesCounter].tableID = temp & 0x0F;
					ids->htables.htables[ids->htables.htablesCounter].numSym = 0;
					for(e = 0; e<16;e++)
					{	
						ids->htables.htables[ids->htables.htablesCounter].codeCount[e]= readByte(&jpgDataScanStart, &jpegFileByteIndex);
						ids->htables.htables[ids->htables.htablesCounter].numSym += (unsigned int) ids->htables.htables[ids->htables.htablesCounter].codeCount[e];
					}
					leftlength = leftlength - 16;
			
					ids->htables.htables[ids->htables.htablesCounter].tableElements = malloc(ids->htables.htables[ids->htables.htablesCounter].numSym);
					
					for(e=0;e<ids->htables.htables[ids->htables.htablesCounter].numSym;e++)
						ids->htables.htables[ids->htables.htablesCounter].tableElements[e]=readByte(&jpgDataScanStart, &jpegFileByteIndex);
					leftlength = leftlength - ids->htables.htables[ids->htables.htablesCounter].numSym;
					build_huffman_table(&ids->htables.htables[ids->htables.htablesCounter]);
					ids->htables.htablesCounter++;
				}
				m = read2Bytes(&jpgDataScanStart, &jpegFileByteIndex);
				break;
			}
		}

		ids->sos.marker = m;// Identifier 2 bytes 0xff, 0xda identify SOS marker
		ids->sos.length=read2Bytes(&jpgDataScanStart, &jpegFileByteIndex)-2;// 2 bytes This must be equal to 6+2*(number of components in scan).
		ids->sos.numberOfComponents=readByte(&jpgDataScanStart, &jpegFileByteIndex);// 1 byte This must be >= 1 and <=4 (otherwise error), usually 1 or 3
		for(e = 0; e < ids->sos.numberOfComponents;e++)
		{
			ids->sos.components[e].componentId=readByte(&jpgDataScanStart, &jpegFileByteIndex);//(1byte)(1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q),
			ids->sos.components[e].huffmanTable=readByte(&jpgDataScanStart, &jpegFileByteIndex);// bit 0..3 : AC table (0..3) bit 4..7 : DC table (0..3);//Each component 2 bytes For each component, read 2 bytes. It contains,
		}
		ids->sos.Ignorable1=readByte(&jpgDataScanStart, &jpegFileByteIndex);//Ignorable Bytes 3 bytes We have to skip 3 bytes.
		ids->sos.Ignorable2=readByte(&jpgDataScanStart, &jpegFileByteIndex);
		ids->sos.Ignorable3=readByte(&jpgDataScanStart, &jpegFileByteIndex);


		ids->nbits_in_reservoir = 0;
		ids ->reservoir=0;
		ids->stream_length = fileLen -(jpgDataScanStart - buffer) - 2;
		ids->stream_start = malloc(ids->stream_length);
		memcpy(ids->stream_start,jpgDataScanStart,ids->stream_length);
		ids->stream=ids->stream_start;
		free(buffer);
		ids->stream_end = ids->stream_start + ids->stream_length-1;

		if(ids->sof0.numberOfComponents > 1)//for now i will assume that the picture is in graycode colorspace
		{
			printf("Image should be in gray code\n");
			deleteImageDataStructure(ids);
			return NULL;
		}
		for(e = 0; e < ids->sof0.numberOfComponents; e++)//this code will do the allocation for all the componenets
			ids->components[e]=calloc(ceil(ids->sof0.imageHeight/8.0) * ceil(ids->sof0.imageWidth/8.0),sizeof(component));
			
		ids->componentsInfo[0].previous_DC=0;
		ids->componentsInfo[0].Q_table=ids->qtables.qtables[ids->sof0.components[0].quantizationTableID].ftableElements;
		ids->componentsInfo[0].DC_table = &ids->htables.htables[0];
		ids->componentsInfo[0].AC_table = &ids->htables.htables[(ids->sof0.numberOfComponents > 1 ? 2:1)];
		ids->componentsInfo[0].Hfactor = ids->sof0.components[0].samplingFactors>>4;
		ids->componentsInfo[0].Vfactor = ids->sof0.components[0].samplingFactors&0xf;

		if(ids->sof0.numberOfComponents > 1)
		{//this code should not be excecuted for gray images
			ids->componentsInfo[1].previous_DC=0;
			ids->componentsInfo[1].Q_table= ids->qtables.qtables[ids->sof0.components[1].quantizationTableID].ftableElements;
			ids->componentsInfo[1].DC_table = & ids->htables.htables[1];
			ids->componentsInfo[1].AC_table = & ids->htables.htables[3];
			ids->componentsInfo[1].Hfactor = ids->sof0.components[1].samplingFactors>>4;
			ids->componentsInfo[1].Vfactor = ids->sof0.components[1].samplingFactors&0xf;

			ids->componentsInfo[2].previous_DC=0;
			ids->componentsInfo[2].Q_table= ids->qtables.qtables[ids->sof0.components[2].quantizationTableID].ftableElements;
			ids->componentsInfo[2].DC_table = & ids->htables.htables[1];
			ids->componentsInfo[2].AC_table = & ids->htables.htables[3];
			ids->componentsInfo[2].Hfactor = ids->sof0.components[2].samplingFactors>>4;
			ids->componentsInfo[2].Vfactor = ids->sof0.components[2].samplingFactors&0xf;
		}
	}
	return ids;
}


int arraySearch(int a[],int size, int key)
{
	int i;

	for(i=0;i<size;i++)
		if(key==a[i])
			return 1;
	return 0;
}


//return 1 if sucessfull
int reconstruct(myImageStructure * ids,int * dp, int ndp)
{
	int c = 0, nextc,dct,i;
	double w1 = 0.1, w2 = 0.4, w3 = 0.1, w4 = 0.4;
	int noOfComponents = ceil(ids->sof0.imageHeight/8.0) * ceil(ids->sof0.imageWidth/8.0);
	
	
	if(ndp==0)
		return 1;
	if(ids->pcketizedFlag==0)//if image is not packetized, we caanot continue
	{
		decodeHuffman_PKT(ids,0);
		
	}
	ids->componentsInfo[0].previous_DC = 0;
	
	//printf("before loop\n");
	for(c=0;c < noOfComponents;c++)
	{
		//printf( " c = %d\n", c);
		if(arraySearch(dp,ndp,ids->components[0][c].packetID)==1)
		{
			
			if(c == 0 || c==1 || c == 2|| c==3)
				return 0;//image cannot be reconstructed
			
			//printf("before calling process_Huffman_data_unit_PKT\n");
			
			process_Huffman_data_unit_PKT(ids,&ids->components[0][c],0);//to skipp unit (unit is here but i will assume that it is not
			
			//printf("calculating dct\n");

			dct = w1*ids->components[0][c-4].DCT[0] + w2*ids->components[0][c-3].DCT[0]+w3*ids->components[0][c-2].DCT[0]+w4*ids->components[0][c-1].DCT[0];
			//for(nextc = c +1;nextc <noOfCompnents;nextc++)
			//{
			//	ids->components[0][nextc].DCT[0] = (dct-ids->components[0][c].DCT[0])+ ids->components[0][nextc].DCT[0];
			//	ids->components[0][nextc].previous_DC = ids->components[0][nextc-1].DCT[0];
			//}

			ids->components[0][c].DCT[0]=dct;
			ids->components[0][c+1].previous_DC = dct;
			ids->componentsInfo[0].previous_DC = dct;
			for(i=1;i<64;i++)
				ids->components[0][c].DCT[i] = 0;

			//fill in required data for dequantization
			ids->components[0][c].AC_table = ids->componentsInfo[0].AC_table;
			ids->components[0][c].DC_table = ids->componentsInfo[0].DC_table;
			ids->components[0][c].Hfactor = ids->componentsInfo[0].Hfactor;
			ids->components[0][c].Q_table=ids->componentsInfo[0].Q_table;
			ids->components[0][c].Vfactor=ids->componentsInfo[0].Vfactor;
			IDCT(&ids->components[0][c], 8);
						
			//printf("after changing component\n");
			
		}
		else if(arraySearch(dp,ndp,ids->components[0][c].packetID)==0)
		{
			//printf("component is not in lost packet\n");
			//if(ids->components[0][c].huffmanDecodedFalg == 0)
			//{

				process_Huffman_data_unit(ids,&ids->components[0][c],0); //hoon

				//fill in required data for dequantization
				ids->components[0][c].AC_table = ids->componentsInfo[0].AC_table;
				ids->components[0][c].DC_table = ids->componentsInfo[0].DC_table;
				ids->components[0][c].Hfactor = ids->componentsInfo[0].Hfactor;
				ids->components[0][c].Q_table=ids->componentsInfo[0].Q_table;
				ids->components[0][c].Vfactor=ids->componentsInfo[0].Vfactor;
				IDCT(&ids->components[0][c], 8);
			//}
		}
	}
	
	return 1;
}

/*
IplImage * convertToOPENCV(myImageStructure *ids)//assumes ids hold gray image data
{
	int i,w=0,h=0,winC,hinC,bytes = 0,hc=0;
	IplImage *img;
 
	if( ids->sof0.numberOfComponents > 1)
	{
		printf("input structure does not represent gray image\n");
		return NULL;
	}

	winC = ceil(ids->sof0.imageWidth/8.0);
	hinC = ceil(ids->sof0.imageHeight/8.0);
	
	img = cvCreateImage(cvSize(winC*8.0, hinC*8.0), IPL_DEPTH_8U, ids->sof0.numberOfComponents);
	for(hc = 0; hc < hinC;hc++)
	{
		for(h = 0;h < 8;h++)
		{		
			for (i=0;i <winC;i++)
			{
				for(w=0;w<8;w++)
				{
					img->imageData[bytes] = ids->components[0][i+winC*hc].block[w+8*h];
					bytes++;
					//if(bytes >= ids->sof0.imageWidth *(h+1))
						//break;
				}
			}
		}
	}
	return img;
}
*/

IplImage * convertToOPENCV(myImageStructure *ids)//assumes ids hold gray image data
{
	int i,w=0,h=0,winC,hinC,bytes = 0,hc=0, totalWidth = 0, totalHight = 0;
	IplImage *img;

 
	
	if( ids->sof0.numberOfComponents > 1)
	{
		printf("input structure does not represent gray image\n");
		return NULL;
	}

	winC = ceil(ids->sof0.imageWidth/8.0);
	hinC = ceil(ids->sof0.imageHeight/8.0);
	
	//img = cvCreateImage(cvSize(ids->sof0.imageWidth, ids->sof0.imageHeight), IPL_DEPTH_8U, ids->sof0.numberOfComponents);
	
	img = cvCreateImage(cvSize(winC*8, hinC * 8), IPL_DEPTH_8U, ids->sof0.numberOfComponents);
	for(hc = 0; hc < hinC;hc++)
	{
		for(h = 0;h < 8;h++)
		{
			for (i=0;i <winC;i++)
			{
				for(w=0;w<8;w++)
				{
					
					//if( bytes >= ceil((ids->sof0.imageHeight*ids->sof0.imageWidth)/64)+ (ids->sof0.imageHeight*ids->sof0.imageWidth-ceil((ids->sof0.imageHeight*ids->sof0.imageWidth)/64)) )//working parthally
					//if( abs(ids->sof0.imageWidth - (winC *8+8)) > abs(ids->sof0.imageWidth - ((winC -1) *8 + 8)) &&   ( i*8+w >= ids->sof0.imageWidth || hc * 8 +h >= ids->sof0.imageHeight )) 
					
					//	continue;
					//if( abs(ids->sof0.imageWidth - (winC *8+8)) < abs(ids->sof0.imageWidth - ((winC -1) *8 +8)) && (bytes >= ids->sof0.imageHeight*ids->sof0.imageWidth || bytes > (hc * 8 + h + 1) *ids->sof0.imageWidth   ))
					//	continue;
					img->imageData[bytes] = ids->components[0][i+winC*hc].block[w+8*h];
					bytes++;
				}
			}
		}
	}
	//printf("image width = %d\nimage height = %d\nbytes = %d\n",ids->sof0.imageWidth, ids->sof0.imageHeight,bytes);
	return img;
}

