#ifndef _MYRTPJPEGHEADER
#define _MYRTPJPEGHEADER

#include<stdio.h>

#include "cv.h"
#include "highgui.h"

#include "cxcore.h"

#if defined (__cplusplus)
extern "C" {
#endif


/* openCV image object format
IplImage
  |-- int  nChannels;     // Number of color channels (1,2,3,4)
  |-- int  depth;         // Pixel depth in bits: 
  |                       //   IPL_DEPTH_8U, IPL_DEPTH_8S, 
  |                       //   IPL_DEPTH_16U,IPL_DEPTH_16S, 
  |                       //   IPL_DEPTH_32S,IPL_DEPTH_32F, 
  |                       //   IPL_DEPTH_64F
  |-- int  width;         // image width in pixels
  |-- int  height;        // image height in pixels
  |-- char* imageData;    // pointer to aligned image data
  |                       // Note that color images are stored in BGR order
  |-- int  dataOrder;     // 0 - interleaved color channels, 
  |                       // 1 - separate color channels
  |                       // cvCreateImage can only create interleaved images
  |-- int  origin;        // 0 - top-left origin,
  |                       // 1 - bottom-left origin (Windows bitmaps style)
  |-- int  widthStep;     // size of aligned image row in bytes
  |-- int  imageSize;     // image data size in bytes = height*widthStep
  |-- struct _IplROI *roi;// image ROI. when not NULL specifies image
  |                       // region  to be processed.
  |-- char *imageDataOrigin; // pointer to the unaligned origin of image data
  |                          // (needed for correct image deallocation)
  |
  |-- int  align;         // Alignment of image rows: 4 or 8 byte alignment
  |                       // OpenCV ignores this and uses widthStep instead
  |-- char colorModel[4]; // Color model - ignored by OpenCV
 */

//JPEG file markers unused in my code listed for information only
typedef enum {		/* JPEG marker codes			*/
  M_SOF0  = 0xc0,	/* baseline DCT				*/
  M_SOF1  = 0xc1,	/* extended sequential DCT		*/
  M_SOF2  = 0xc2,	/* progressive DCT			*/
  M_SOF3  = 0xc3,	/* lossless (sequential)		*/
  
  M_SOF5  = 0xc5,	/* differential sequential DCT		*/
  M_SOF6  = 0xc6,	/* differential progressive DCT		*/
  M_SOF7  = 0xc7,	/* differential lossless		*/
  
  M_JPG   = 0xc8,	/* JPEG extensions			*/
  M_SOF9  = 0xc9,	/* extended sequential DCT		*/
  M_SOF10 = 0xca,	/* progressive DCT			*/
  M_SOF11 = 0xcb,	/* lossless (sequential)		*/
  
  M_SOF13 = 0xcd,	/* differential sequential DCT		*/
  M_SOF14 = 0xce,	/* differential progressive DCT		*/
  M_SOF15 = 0xcf,	/* differential lossless		*/
  
  M_DHT   = 0xc4,	/* define Huffman tables		*/
  
  M_DAC   = 0xcc,	/* define arithmetic conditioning table	*/
  
  M_RST0  = 0xd0,	/* restart				*/
  M_RST1  = 0xd1,	/* restart				*/
  M_RST2  = 0xd2,	/* restart				*/
  M_RST3  = 0xd3,	/* restart				*/
  M_RST4  = 0xd4,	/* restart				*/
  M_RST5  = 0xd5,	/* restart				*/
  M_RST6  = 0xd6,	/* restart				*/
  M_RST7  = 0xd7,	/* restart				*/
  
  M_SOI   = 0xd8,	/* start of image			*/
  M_EOI   = 0xd9,	/* end of image				*/
  M_SOS   = 0xda,	/* start of scan			*/
  M_DQT   = 0xdb,	/* define quantization tables		*/
  M_DNL   = 0xdc,	/* define number of lines		*/
  M_DRI   = 0xdd,	/* define restart interval		*/
  M_DHP   = 0xde,	/* define hierarchical progression	*/
  M_EXP   = 0xdf,	/* expand reference image(s)		*/
  
  M_APP0  = 0xe0,	/* application marker, used for JFIF	*/
  M_APP1  = 0xe1,	/* application marker			*/
  M_APP2  = 0xe2,	/* application marker			*/
  M_APP3  = 0xe3,	/* application marker			*/
  M_APP4  = 0xe4,	/* application marker			*/
  M_APP5  = 0xe5,	/* application marker			*/
  M_APP6  = 0xe6,	/* application marker			*/
  M_APP7  = 0xe7,	/* application marker			*/
  M_APP8  = 0xe8,	/* application marker			*/
  M_APP9  = 0xe9,	/* application marker			*/
  M_APP10 = 0xea,	/* application marker			*/
  M_APP11 = 0xeb,	/* application marker			*/
  M_APP12 = 0xec,	/* application marker			*/
  M_APP13 = 0xed,	/* application marker			*/
  M_APP14 = 0xee,	/* application marker, used by Adobe	*/
  M_APP15 = 0xef,	/* application marker			*/
  
  M_JPG0  = 0xf0,	/* reserved for JPEG extensions		*/
  M_JPG13 = 0xfd,	/* reserved for JPEG extensions		*/
  M_COM   = 0xfe,	/* comment				*/
  
  M_TEM   = 0x01,	/* temporary use			*/

  M_ERROR = 0x100	/* dummy marker, internal use only	*/
} JPEG_MARKER;




typedef unsigned char byte;
typedef unsigned short int twoBytes;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;

void printByteArray(byte *a, int size, int elementSize,int lineLength);


void printByteArrayAsEachNumberIsOneByte(byte *a, int size, int lineLength);

void printByteArrayAsEachNumberIsTwoBytes(byte *a, int size, int lineLength);

void skipBytes(byte **p, int s, int *index);
byte readByte(byte **p, int *index);
twoBytes read2Bytes(byte **p, int *index);

byte read3Bytes(byte **p, int *index);
byte read4Bytes(byte **p, int *index);


#define HUFFMAN_BITS_SIZE  256
#define HUFFMAN_HASH_NBITS 9
#define HUFFMAN_HASH_SIZE  (1UL<<HUFFMAN_HASH_NBITS)
#define HUFFMAN_HASH_MASK  (HUFFMAN_HASH_SIZE-1)

#define HUFFMAN_TABLES	   4
#define COMPONENTS	   3

#define PACKET_SIZE 1024 * 8


typedef struct 
{
	twoBytes marker;// 2 bytes 0xff, 0xc0 to identify SOF0 marker
	twoBytes length;       /* 04h  Length of APP0 Field      */
	byte identifier[5];   /* 06h  "JFIF" (zero terminated) Id String */
	twoBytes version;      /* 07h  JFIF Format Revision      */
	byte units;           /* 09h  Units used for Resolution */
	twoBytes xDensity;     /* 0Ah  Horizontal Resolution     */
	twoBytes yDensity;     /* 0Ch  Vertical Resolution       */
	byte xThumbnail;      /* 0Eh  Horizontal Pixel Count    */
	byte yThumbnail;      /* 0Fh  Vertical Pixel Count      */
} JFIFHeader;

typedef struct
{	//Each component 3 bytes Read each component data of 3 bytes. It contains,
	byte componentId;//(1byte)(1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q),
	byte samplingFactors; //(1byte) (bit 0-3 vertical., 4-7 horizontal.),
	byte quantizationTableID;// (1 byte
} FrameComponent;
typedef struct
{
	twoBytes marker;// 2 bytes 0xff, 0xc0 to identify SOF0 marker
	twoBytes length;// 2 bytes This value equals to 8 + components*3 value
	byte dataPrecision;// 1 byte This is in bits/sample, usually 8 (12 and 16 not supported by most software).
	twoBytes imageHeight;// 2 bytes This must be > 0
	twoBytes imageWidth;// 2 bytes This must be > 0
	byte numberOfComponents;// 1 byte Usually 1 = grey scaled, 3 = color YcbCr or YIQ 4 = color CMYK (unsuported)
	FrameComponent components[3]; 
} SOF0;

void printSOF0(SOF0 sof0);


typedef struct
{	//Each component 3 bytes Read each component data of 3 bytes. It contains,
	byte componentId;//(1byte)(1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q),
	byte huffmanTable;// bit 0..3 : AC table (0..3) bit 4..7 : DC table (0..3)
} ScanComponent;
typedef struct
{
	twoBytes marker;// Identifier 2 bytes 0xff, 0xda identify SOS marker
	twoBytes length;// 2 bytes This must be equal to 6+2*(number of components in scan).
	byte numberOfComponents;// 1 byte This must be >= 1 and <=4 (otherwise error), usually 1 or 3
	ScanComponent components[3];//Each component 2 bytes For each component, read 2 bytes. It contains,
	byte Ignorable1;//Ignorable Bytes 3 bytes We have to skip 3 bytes.
	byte Ignorable2;
	byte Ignorable3;
} SOS;

void printSOS(SOS sos);

typedef struct 
{
	byte value;
	int length;
	int index;
} Marker;

typedef struct {
	unsigned int tableSize;//in bytes  assumed to be always 64
	unsigned int tableID;//0 for Luma and 1 for chroma
	unsigned int elementSize;//element size in bits for now it is assumed alwayse to be 8 bits
	byte tableElements[64];//table elements as bits
	float ftableElements[64];//table elements in float will be constructed using the function build quantization table
} QTable;

typedef struct {
	QTable qtables[2];//0 for Luma and 1 for chroma
	int qtablesCounter;// there will be only one table in gray images
} QTables;

void printQuantizationInformation(QTables qtables);

typedef struct {
	unsigned int isAC:1;//true if AC and false if DC
	unsigned int tableID;//0 for luma and 1 for chroma

	unsigned int numSym;
	byte codeCount[16];

	byte * tableElements;
	
	/* Fast look up table, using HUFFMAN_HASH_NBITS bits we can have directly the symbol,
	* if the symbol is <0, then we need to look into the tree table */
	short int lookup[HUFFMAN_HASH_SIZE];
	/* code size: give the number of bits of a symbol is encoded */
	unsigned char code_size[HUFFMAN_HASH_SIZE];
	/* some place to store value that is not encoded in the lookup table 
	* FIXME: Calculate if 256 value is enough to store all values
	*/
	twoBytes slowtable[16-HUFFMAN_HASH_NBITS][256];

} HTable;

typedef struct {
	HTable htables[4];
	int htablesCounter;
} HTables;

void printHuffmanInformation(HTables htables);

/*
 * 4 functions to manage the stream
 *
 *  fill_nbits: put at least nbits in the reservoir of bits.
 *              But convert any 0xff,0x00 into 0xff
 *  get_nbits: read nbits from the stream, and put it in result,
 *             bits is removed from the stream and the reservoir is filled
 *             automaticaly. The result is signed according to the number of
 *             bits.
 *  look_nbits: read nbits from the stream without marking as read.
 *  skip_nbits: read nbits from the stream but do not return the result.
 * 
 * stream: current pointer in the jpeg data (read bytes per bytes)
 * nbits_in_reservoir: number of bits filled into the reservoir
 * reservoir: register that contains bits information. Only nbits_in_reservoir
 *            is valid.
 *                          nbits_in_reservoir
 *                        <--    17 bits    -->
 *            Ex: 0000 0000 1010 0000 1111 0000   <== reservoir
 *                        ^
 *                        bit 1
 *            To get two bits from this example
 *                 result = (reservoir >> 15) & 3
 */
 
#define fill_nbits(reservoir,nbits_in_reservoir,stream,stream_end,nbits_wanted) do { \
   while (nbits_in_reservoir < nbits_wanted) \
    { \
      unsigned char c; \
      if (stream > stream_end) \
		break; \
	  c = *stream++; \
      reservoir <<= 8; \
      if (c == 0xff && *stream == 0x00) \
        stream++; \
      reservoir |= c; \
      nbits_in_reservoir+=8; \
    } \
}  while(0);

// Signed version !!!! 
#define get_nbits(reservoir,nbits_in_reservoir,stream,stream_end,nbits_wanted,result) do { \
   fill_nbits(reservoir,nbits_in_reservoir,stream,stream_end,(nbits_wanted)); \
   result = ((reservoir)>>(nbits_in_reservoir-(nbits_wanted))); \
   nbits_in_reservoir -= (nbits_wanted);  \
   reservoir &= ((1U<<nbits_in_reservoir)-1); \
   if ((unsigned int)result < (1UL<<((nbits_wanted)-1))) \
       result += (0xFFFFFFFFUL<<(nbits_wanted))+1; \
}  while(0);

#define look_nbits(reservoir,nbits_in_reservoir,stream,stream_end,nbits_wanted,result) do { \
   fill_nbits(reservoir,nbits_in_reservoir,stream,stream_end,(nbits_wanted)); \
   result = ((reservoir)>>(nbits_in_reservoir-(nbits_wanted))); \
}  while(0);

 /*To speed up the decoding, we assume that the reservoir have enough bit 
 * slow version:
 * #define skip_nbits(reservoir,nbits_in_reservoir,stream,nbits_wanted) do { \
 *   fill_nbits(reservoir,nbits_in_reservoir,stream,(nbits_wanted)); \
 *   nbits_in_reservoir -= (nbits_wanted); \
 *   reservoir &= ((1U<<nbits_in_reservoir)-1); \
 * }  while(0);
 */
#define skip_nbits(reservoir,nbits_in_reservoir,stream,stream_end,nbits_wanted) do { \
   nbits_in_reservoir -= (nbits_wanted); \
   reservoir &= ((1U<<nbits_in_reservoir)-1); \
}  while(0);



typedef struct  
{
  unsigned int Hfactor;
  unsigned int Vfactor;
  float *Q_table;		/* Pointer to the quantisation table to use */
  HTable *AC_table;
  HTable *DC_table;
  short int previous_DC;	/* Previous DC coefficient */
  short int DCT[64];		/* DCT coef */
 
  byte block[64]; //result after invers DCT
  int huffmanDecodedFalg;
  byte * hBlock; //huffman code unit
  unsigned int hBlockSize; //hblocksize in bytes
  unsigned int hBlockSizeBits;//hblock size in bits
  unsigned int packetID;//RTP packet that this component belongs to
  
} component;


typedef struct 
{
	JFIFHeader JFIFdata;
	SOF0 sof0;
	SOS sos;
	twoBytes restartInterval;
	QTables qtables;
	HTables htables;
	byte *stream_start,*stream_end;
	unsigned int stream_length;//jpeg data size in bytes. this wil include restart markers if any
	component *components[3];
	
	int pcketizedFlag;
	int huffmanDecodedFalg;
	int dequantizedFlag;

	//utility variables
	const unsigned char *stream;	/* Pointer to the current stream */
	unsigned int reservoir, nbits_in_reservoir;
	uint8_t Y[64*4], Cr[64], Cb[64];
	component componentsInfo[3];

	unsigned int rtpPacketSize; //will be used to set the required RTP packet size
	unsigned int rtpPacketsNeeded;//will be set to the totoal number of rtp packets needed to represent the image
	// Internal Pointer use for colorspace conversion, do not modify it !!! */
	//uint8_t *plane[COMPONENTS];

} myImageStructure;


void build_huffman_table(HTable * htable);

void process_Huffman_data_unit(myImageStructure *ids,component *c, int cID);
int get_next_huffman_code(myImageStructure *ids,HTable *huffman_table);
void process_Huffman_data_unit_PKT(myImageStructure *ids,component *c, int cID);



void build_quantization_table(QTable *qtable);



//#include "stdint.h"

unsigned char descale_and_clamp(int x, int shift);


void IDCT (component *compptr,  int stride);

void decode(myImageStructure * ids);
void decodeHuffman(myImageStructure * ids);

int decodeHuffman_PKT(myImageStructure * ids, int includeQuantizationFlag);


void decodeQuantization(myImageStructure * ids);

void deleteImageDataStructure(myImageStructure *ids);

myImageStructure * createImageDataStructure(char fileName[], int packetSize);

int arraySearch(int a[],int size, int key);
int reconstruct(myImageStructure * ids,int * dp, int ndp);
		
IplImage * convertToOPENCV(myImageStructure *ids);

int getPSNR(IplImage * frameReference,  IplImage * frameUnderTest, double *mse, double *psnr);

#if defined (__cplusplus)
} /* end of 'extern "C" {' */
#endif


#endif
