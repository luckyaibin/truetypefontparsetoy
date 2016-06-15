#ifndef __TRUETYPEFONT_H__
#define __TRUETYPEFONT_H__

#include<stdio.h>
#include<stdlib.h>
#include <memory.h>
#include <assert.h>
//https://www.microsoft.com/typography/otspec/otff.htm
#ifndef __TTF_TYPE__
#define __TTF_TYPE__
typedef unsigned char       BYTE;
typedef char                CHAR;
typedef unsigned short      USHORT;
typedef short               SHORT;
typedef signed int          INT24;
typedef signed int          INT;
typedef unsigned int        UINT;
typedef unsigned long       ULONG;
typedef long                LONG;
//typedef signed int          FIXED;
typedef unsigned int        FUNIT;
typedef signed short        FWORD;
typedef unsigned short      UFWORD;
typedef unsigned short      F2DOT14;
typedef signed __int64      LONGDATETIME;
typedef unsigned char       TAG;
typedef unsigned short		GlyphID;
typedef unsigned short		Offset;
#endif

//ƽ̨id
#define CMAP_PLATFORM_ID_CUSTOM		4
#define CMAP_PLATFORM_ID_WIN		3
#define CMAP_PLATFORM_ID_MAC		1
#define CMAP_PLATFORM_ID_UNICODE	0

//����id
#define CMAP_WIN_ENCODING_ID_Symbol 0
#define CMAP_WIN_ENCODING_ID_Unicode_BMP_UCS_2 1
#define CMAP_WIN_ENCODING_ID_ShiftJIS 2
#define CMAP_WIN_ENCODING_ID_PRC 3
#define CMAP_WIN_ENCODING_ID_Big5 4
#define CMAP_WIN_ENCODING_ID_Wansung 5
#define CMAP_WIN_ENCODING_ID_Johab 6
#define CMAP_WIN_ENCODING_ID_Reserved7 7
#define CMAP_WIN_ENCODING_ID_Reserved8 8
#define CMAP_WIN_ENCODING_ID_Reserved9 9
#define CMAP_WIN_ENCODING_ID_Unicode_UCS_4 10


#define POINT_TYPE_NEW_CONTOUR 1
#define POINT_TYPE_CURVE 2
#define POINT_TYPE_LINE 4

////////////////////////////////////////////��Щ�ṹ��.ttc�ļ���.otf/.ttf�ļ� ���ļ��ṹʾ��,��û��ʵ���õ�/////////////////////////
//����ǰ�����������TTC�ļ�,��ô���������������е�ĳ��
//���������ṹ����Ե��� .ttc��ʽ���ļ��ṹ.
struct TTCHeaderv1
{
	TAG 	TTCTag; // Font Collection ID string : 'ttcf' (used for both CFF and TrueType, for backward compatibility)
	FIXED 	Version; // 	Version of the TTC Header(1.0), 0x00010000
	ULONG 	numFonts; // 	Number of fonts in TTC
	ULONG 	OffsetTable[1]; // 	Array of offsets to the OffsetTable for each font from the beginning of the file
};

struct TTCHeaderv2
{
	TAG 	TTCTag; //TrueType Collection ID string : 'ttcf'
	FIXED 	Version; // 	Version of the TTC Header(2.0), 0x00020000
	ULONG 	numFonts; // 	Number of fonts in TTC
	ULONG 	offsetTable[0x7fffffff / 4]; // Array of offsets to the OffsetTable for each font from the beginning of the file
	ULONG 	ulDsigTag; // 	Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature)
	ULONG 	ulDsigLength; // 	The length(in bytes) of the DSIG table(null if no signature)
	ULONG 	ulDsigOffset; // 	The offset(in bytes) of the DSIG table from the beginning of the TTC file(null if no signature)
};


//���ֻ��һ������,��ô�ļ���ͷ����OffsetTable
//�����TrueType collection,��ôÿ�������ļ��Ŀ�ʼָ������ TTCHeader ��ʶ(indidated)
struct OffsetTable
{
	FIXED 	sfnt_version; //0x00010000 for version 1.0.�����������TTF����,��ô1.0�ǰ汾��.�����������CFF����,��ô��'OTTO'
	USHORT 	numTables;// 	Number of tables.
	USHORT 	searchRange;// (Maximum power of 2 <= numTables) x 16.
	USHORT 	entrySelector;// 	Log2(maximum power of 2 <= numTables).
	USHORT 	rangeShift;// 	NumTables x 16 - searchRange.
};
//������OffsetTable�ľ���TableRecord��Ŀ,����tag�����
struct TableRecord
{
	ULONG 	tag; //	4 - byte identifier.����4�ֽڵ�����,�������4�ֽ�,���油0
	ULONG 	checkSum; // 	CheckSum for this table.
	ULONG 	offset; // 	Offset from beginning of TrueType font file.
	ULONG 	length; // 	Length of this table.
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//cmap : �ַ������͵�����ӳ���
//��ΪΪ��֧�ֲ�ֹ1���ַ�����ṹ,�������в�ֹ1���ӱ�(subtable)
//https://www.microsoft.com/typography/otspec/cmap.htm
struct cmap_header
{
	USHORT 	version;//	Table version number(0).
	USHORT 	numTables;//	Number of encoding tables that follow.
};

struct cmap_subtable
{
	USHORT 	platformID;//	Platform ID.
	USHORT 	encodingID;// 	Platform - specific encoding ID.
	ULONG 	offset; //Byte offset from beginning of table to the subtable for this encoding.
};
//ÿ��cmap�ӱ������7�ָ�ʽ֮һ��
//format 0, format 2, format 4, format 6, format 8.0, format 10.0, and format 12.0 ��

//Byte encoding table �����ڱ����glyph�����ڵ��ֽڷ�Χ�ڵ��ַ������Ǳ�׼��ƻ���ַ���glyph��ӳ�䷽ʽ��
struct cmap_subtable_format0
{
	USHORT 	format;//	 	Format number is set to 0.
	USHORT 	length;//		This is the length in bytes of the subtable.
	USHORT 	language;//	 	Please see ��Note on the language field in 'cmap' subtables�� in this document.
	BYTE 	glyphIdArray[256];//	 	An array that maps character codes to glyph index values.
};
//��cjk���������.ʹ����8/16bit��ϱ���,��ʹ����ĳ��ȷ��ֵ���ֽڱ�������һ��2byte�ַ��ĵ�һ���ֽ�.
//����,��������2-byte�ַ�,��������(glyph)������ӳ����Ȼ�������ڵ�һ���ֽ�.
struct subHeaders_of_format2
{
	USHORT 	firstCode;//	 	 	First valid low byte for this subHeader.
	USHORT 	entryCount;//	 	 	Number of valid low bytes for this subHeader.
	SHORT 	idDelta;//	 	 	See text below.
	USHORT 	idRangeOffset;//	 	 	See text below.
};
struct cmap_subtable_format2
{
	USHORT 	format;//	 	Format number is set to 2.
	USHORT 	length;//		This is the length in bytes of the subtable.
	USHORT 	language;//	 	Please see ��Note on the language field in 'cmap' subtables�� in this document.
	USHORT 	subHeaderKeys[256];//	 	Array that maps high bytes to subHeaders : value is subHeader index * 8.
	subHeaders_of_format2 subHeaders[0xFFFF];//	 	Variable - length array of subHeader structures.
	USHORT 	glyphIndexArray[0xFFFF];//	 	Variable - length array containing subarrays used for mapping the low byte of 2 - byte characters.
};

//Format 4 ��˫�ֽڱ��롣���ָ�ʽ���������ļ��еı���ֲ��뼸������������������֮�������һЩ�����Ŀհס�
//����һЩ������ܲ������������е�glyph��Ӧ��
struct cmap_subtable_format4
{
#define  segCount 10000
#define  arbitrary_length 10000
	USHORT 	format;//	 	 	Format number is set to 4.
	USHORT 	length;//	 		This is the length in bytes of the subtable.
	USHORT 	language;//	 	 	Please see ��Note on the language field in 'cmap' subtables�� in this document.
	USHORT 	segCountX2;//	 	 	2 x segCount.
	USHORT 	searchRange;//	 		2 x(2 * *floor(log2(segCount)))
	USHORT 	entrySelector;//	 		log2(searchRange / 2)
	USHORT 	rangeShift;//	 		2 x segCount - searchRange
	USHORT 	endCount[segCount];//	 	 	End characterCode for each segment, last = 0xFFFF.
	USHORT 	reservedPad;//	 	 	Set to 0.
	USHORT 	startCount[segCount];//	 	 	Start character code for each segment.
	SHORT 	idDelta[segCount];//	 	 	Delta for all character codes in segment.
	USHORT 	idRangeOffset[segCount];//	 	 	Offsets into glyphIdArray or 0
	USHORT 	glyphIdArray[arbitrary_length];//		Glyph index array(arbitrary length)
#undef segCount
#undef arbitrary_length 10000
};

//�������õ��������ݽṹ
struct fontinfo
{
	void			* userdata;
	const BYTE			* data;              // pointer to .ttf file
	int              fontstart;         // offset of start of font

	int numGlyphs;                     // number of glyphs, needed for range checking

	int cmap,loca, head, glyf, hhea, hmtx, kern; // table locations as offset from start of .ttf
	int index_map;                     // a cmap mapping for our chosen character encoding
	int indexToLocFormat;              // format needed to map from glyph index to glyph
};

//big endian to small endian
USHORT b2s16(USHORT b)
{
	USHORT s = ((b >> 8) & 0xFF) | ((b & 0xFF) << 8);
	return s;
}

UINT b2s32(UINT b)
{
	UINT s = ((b & 0xFF) << 24) | (((b >> 8) & 0xFF) << 16) | (((b >> 16) & 0xFF) << 8) | ((b >> 24) & 0xFF);
	return s;
}

//���תΪС��
static USHORT ttUSHORT(const BYTE *p) 
{ 
	return p[0] * 256 + p[1]; 
}
static SHORT ttSHORT(const BYTE *p)   
{ 
	return p[0] * 256 + p[1]; 
}
static UINT ttULONG(const BYTE *p)  
{ 
	return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; 
}
static INT ttLONG(const BYTE *p)   
{ 
	return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; 
}

INT check_tag4(const BYTE *pc, BYTE c0, BYTE c1, BYTE c2, BYTE c3)
{
	if (pc[0] == c0 && pc[1] == c1 && pc[2] == c2 && pc[3] == c3)
	{
		printf("%c%c%c%c\n", pc[0], pc[1], pc[2], pc[3]);
		return 1;
	}
	return 0;
}

int check_tag(const BYTE *pc, const BYTE *tag)
{
	return check_tag4(pc, tag[0], tag[1], tag[2], tag[3]);
}


static int check_is_font(const BYTE *font)
{
	// check the version number
	if (check_tag4(font, '1', 0, 0, 0))  return 1;  // TrueType 1
	if (check_tag(font, (BYTE*)"typ1"))   return 1; // TrueType with type 1 font -- we don't support this!
	if (check_tag(font, (BYTE*)"OTTO"))   return 1; // OpenType with CFF(postscript outlines)
	if (check_tag4(font, 0, 1, 0, 0)) return 1;     // OpenType 1.0

	//if (check_tag(font, (BYTE*)"ttcf")) return 1;   //.TTC��ʽ�Ŀ�
	return 0;
}

static int check_is_ttc(const BYTE *font)
{
	if (check_tag(font, (BYTE*)"ttcf")) return 1;   //.TTC��ʽ�Ŀ�
	return 0;
}

static int find_table(const BYTE *fontdata, int font_start,const char *table_name)
{
	int OffsetTable_start = font_start;
	USHORT numTables = ttUSHORT(fontdata + OffsetTable_start + 4);
	int TableRecord_start = font_start + 12;
	for (int i = 0; i < numTables;i++)
	{
		int some_TableRecord_start = TableRecord_start + 16 * i;
		if (check_tag(fontdata+some_TableRecord_start,(const BYTE*)table_name))
		{
			ULONG TableRecord_offset = ttULONG(fontdata + some_TableRecord_start + 8);
			return TableRecord_offset;
		}
	}
	return 0;
}


//��.ttc����.ttf����.otf��ȡ�������ļ��е�ƫ����(byte)
int get_font_offset_from_index(const BYTE *font_collection, int index)
{
	// if it's just a font, there's only one valid index
	//if (stbtt__isfont(font_collection))
	//	return index == 0 ? 0 : -1;

	// ����Ƕ��������ɵ�.ttc�ļ�
	if (check_is_ttc(font_collection)) 
	{
		// version 1?
		if (ttULONG(font_collection + 4) == 0x00010000 || ttULONG(font_collection + 4) == 0x00020000) {
			INT numFonts = ttLONG(font_collection + 8);
			if (index >= numFonts)
				return -1;
			unsigned long offset = ttULONG(font_collection + 12 + index * 4);//������ȡTTCHeader(v1��v2)�е�offsetTable�е�ƫ��ֵ
			return offset;
		}
	}
	//����ǵ���otf����ttf�����ļ�,ƫ��ֵ���Ǵ�0��ʼ
	if (check_is_font(font_collection))
	{
		return index == 0 ? 0 : -1;
	}
	return -1;
}

int get_fontinfo_from_font_offset(fontinfo *info, const BYTE *font_collection, int font_offset,int platform_id,int encoding_id)
{
	//memset(info, 0, sizeof(*info));
	info->data = font_collection;
	info->fontstart = font_offset;

	info->cmap = find_table(info->data, info->fontstart, "cmap");       // required
	info->loca = find_table(info->data, info->fontstart, "loca"); // required
	info->head = find_table(info->data, info->fontstart, "head"); // required
	info->glyf = find_table(info->data, info->fontstart, "glyf"); // required
	info->hhea = find_table(info->data, info->fontstart, "hhea"); // required
	info->hmtx = find_table(info->data, info->fontstart, "hmtx"); // required
	info->kern = find_table(info->data, info->fontstart, "kern"); // not required

	INT t = find_table(info->data, info->fontstart, "maxp");
	if (t)
		info->numGlyphs = ttUSHORT(info->data + t + 4);
	else
		info->numGlyphs = 0xffff;
	//cmap ��ͷ
	/*
	{
		UInt16	version;	Version number (Set to zero)
		UInt16	numberSubtables;	Number of encoding subtables
	}
	*/
	//�� cmap �ӱ������7����ʽ:format 0, format 2, format 4, format 6,format 8.0, format 10.0, and format 12.0
	//����ƽ̨id�ͺ�ƽ̨�ض�id��������
	/*
	 {
	 UInt16	 platformID	; Platform identifier
	 UInt16	 platformSpecificID	 ;Platform-specific encoding identifier
	 UInt32	 offset	 ;Offset of the mapping table
	 }
	*/
	int numTables = ttUSHORT(info->data + info->cmap + 2);//2 == sizeof(version)
	for (int i = 0; i < numTables; ++i) {
		UINT subtable_record = info->cmap + 4 + 8 * i;//4==sizeof(cmap��ͷ),8==sizeof(�ӱ�ͷ)
		// find an encoding we understand:
		int platformid = ttUSHORT(info->data + subtable_record);
		int encodingid = ttUSHORT(info->data + subtable_record + 2);
		printf("platformid:%d,encoding_id:%d\n", platformid, encodingid);
		//Ѱ����Ҫ��ƽ̨�ϵı���id��Ӧ��cmap�ӱ��ƫ��(cmap�ӱ������format 0, format 2, format 4, format 6,format 8.0, format 10.0, and format 12.0�е�ĳһ��)
		if (platformid == platform_id && encodingid == encoding_id)
		{
			int subtable_offset = info->cmap + ttULONG(info->data + subtable_record + 4);
			info->index_map = subtable_offset;
			break;
		}
		//switch (platformid)
		//{
		//case CMAP_PLATFORM_ID_WIN:
		//	switch (encodingid)
		//	{
		//	case CMAP_WIN_ENCODING_ID_Unicode_BMP_UCS_2:
		//	case CMAP_WIN_ENCODING_ID_Unicode_UCS_4:
		//		// MS/Unicode
		//		info->index_map = info->cmap + ttULONG(info->data + encoding_record + 4); // 4 == sizeof(platformID) + sizeof(platformSpecificID),����Ҫ���ӱ��offset
		//		break;
		//	}
		//	break;
		//case CMAP_PLATFORM_ID_UNICODE:
		//	// Mac/iOS has these
		//	// all the encodingIDs are unicode, so we don't bother to check it
		//	info->index_map = info->cmap + ttULONG(info->data + encoding_record + 4);
		//	break;
		//}
	}

	info->indexToLocFormat = ttUSHORT(info->data + info->head + 50);
	return 0;
}

//https://www.microsoft.com/typography/otspec/cmap.htm
int get_glyph_index_from_codepoint(fontinfo *info, UINT unicode_codepoint)
{
	const BYTE * data = info->data;
	INT index_map = info->index_map;//index_mapָ�����ĳ��ƽ̨id��encodingid��cmap���ӱ�,�м���format,��Ҫ���ݸ�ʽ���ж�,���в�ͬ�Ĳ���

	USHORT format = ttUSHORT(data + index_map + 0);
	if (format == 0)
	{
		return -1;
	}
	else if (format == 2)
	{
		return -1;
	}
	else if (format == 4)
	{
		if (unicode_codepoint > 0xffff)
			return 0;
		USHORT segcount = ttUSHORT(data + index_map + 6) >> 1;
		USHORT searchRange = ttUSHORT(data + index_map + 8) >> 1;
		USHORT entrySelector = ttUSHORT(data + index_map + 10);
		USHORT rangeShift = ttUSHORT(data + index_map + 12) >> 1;

		INT endCount = index_map + 14;

		int search_index = 0;
		if (unicode_codepoint > ttUSHORT(data + endCount + (rangeShift - 1) * 2)) //���rangeShift���һ��Ԫ�ز�������ڵ���codepoint,ֱ������searchRange�ĵ�һ��,�±���rangeShift
		{
			search_index = search_index + rangeShift;
		}
		int origin_index = search_index;
		for (int seg = 0; seg < segcount; seg++)
		{
			printf("��%d����startCode:%d,endCode: %d \n", seg, ttUSHORT(data + index_map + 14 + segcount * 2 + 2 + 2 * seg), ttUSHORT(data + endCount + 2 * seg));
		}
		int c;
		int next_index;
		while (entrySelector > 0)
		{
			next_index = search_index + (searchRange >> 1);
			 c = ttUSHORT(data + endCount + next_index * 2);
			if (unicode_codepoint > c)
			{
				search_index = next_index;
			}
			searchRange = searchRange >> 1;
			entrySelector--;
		}
		if (search_index == origin_index)//search_indexһ�ζ�û�ı��,˵��search_index�ұ�(������������)û�б�unicode_codepointС��,��Ҫ�����һ���Ƿ�������ڵ�����
		{
			if (ttUSHORT(data + endCount + (search_index) * 2) >= unicode_codepoint)
			{
				search_index = search_index + 0;
			}
			else if (ttUSHORT(data + endCount + (search_index + 1) * 2) >= unicode_codepoint)
			{
				search_index = search_index + 1;
			}
		}
		else//��������ѭ��֮��,search_index��С��unicode_point�ı߽�,��ô���ڵ��ڵ��ں���,��Ҫ+1
		{
			search_index++;
		}

		{
			USHORT offset, start;
			USHORT item = (USHORT)search_index;

			//printf("��0����startCode:%d,endCode: %d \n", ttUSHORT(data + index_map + 14 + segcount * 2 + 2), ttUSHORT(data + endCount));
			//printf("%d,%d \n", ttUSHORT(data + endCount),ttUSHORT(data + endCount + 2 * item));
			//printf("��1����startCode:%d,endCode: %d \n", ttUSHORT(data + index_map + 14 + segcount * 2 + 2 + 2 * 1), ttUSHORT(data + endCount + 2 * 1));
			assert(unicode_codepoint <= ttUSHORT(data + endCount + 2 * item));
			
			start = ttUSHORT(data + index_map + 14 + segcount * 2 + 2 + 2 * item);
			if (unicode_codepoint < start)
				return 0;

			//item = 898;
			offset = ttUSHORT(data + index_map + 14 + segcount * 6 + 2 + 2 * item);//idRangeOffset����λ��
			if (offset == 0)
				return (USHORT)(unicode_codepoint + ttSHORT(data + index_map + 14 + segcount * 4 + 2 + 2 * item));//��item��idDelta,ֱ�Ӽӵ�codepoint�ϼ���
			return ttUSHORT(data + index_map + 14 + segcount * 6 + 2 + 2 * item + offset + (unicode_codepoint - start) * 2 );//����,
		}
	}
	else
	{
		return -1;
	}

}

//������У���
ULONG
CalcTableChecksum(ULONG *Table, ULONG Length)
{
	ULONG Sum = 0L;
	ULONG *Endptr = Table + ((Length + 3) & ~3) / sizeof(ULONG);//��ζ�����еı���4�ֽڶ����,����4�ֽڵĺ��油0,�����ǲ���ȷ��.
	while (Table < Endptr)
		Sum += *Table++;
	return Sum;
}


struct TTF_POINT
{
	UINT x;
	UINT y;
};
INT is_monotonic(TTF_POINT p1, TTF_POINT p2, TTF_POINT p3)
{
	if (p1.y == p2.y && p2.y == p3.y)
	{
		return 0;
	}
	else if (p1.y <= p2.y && p2.y <= p3.y)
	{
		return 1;
	}
	else if (p1.y >= p2.y && p2.y >= p3.y)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

typedef struct
{
	SHORT x, y, cx, cy;
	unsigned char type, padding;
} stbtt_vertex;

stbtt_vertex *g_stbtt_vertex = NULL;
INT g_stbtt_vertex_size = 0;
int load_ttf_file(const char * filename, int font_index, UINT c)
{
	//FILE *file = fopen(filename ? filename : "c:/windows/fonts/arialbd.ttf", "rb");
	//FILE *file = fopen(filename ? filename : "C:/Users/Robin/Desktop/fonts/NotoSansHans-Light.otf", "rb");//OTTO(OpenType with CFF(PostScript)
	//FILE *file = fopen(filename ? filename : "C:/Users/Robin/Desktop/fonts/roman.fon", "rb");//��Ч����(��Ӧ��ǰ�Ĵ��������˵)
	//FILE *file = fopen(filename ? filename : "C:/Users/Robin/Desktop/fonts/simsun.ttf", "rb");//OpenType 1.0
	FILE *file = fopen(filename ? filename : "C:/Users/Robin/Desktop/fonts/SimSun18030.ttc", "rb");//ttcf
	if (!file)
	{
		return -1;
	}
	font_index = font_index ? font_index : 0;
	fseek(file, 0, SEEK_END);
	unsigned int filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	BYTE *ttf_buffer = (BYTE*)malloc(filesize);
	BYTE *ttf_buffer_origin = ttf_buffer;
	fread(ttf_buffer, 1, filesize,file);
	fclose(file);

	//���ļ��������ȡ��font_index�������ڻ�����ľ���
	//>=0 �ɹ�
	//<0  ʧ��
	UINT font_offset = get_font_offset_from_index(ttf_buffer, font_index);
	if (font_offset<0)
	{
		return -1;
	}

	fontinfo fi;

	get_fontinfo_from_font_offset(&fi, ttf_buffer, font_offset,3,1);

	
	//���glyph������
	INT glyph_index = get_glyph_index_from_codepoint(&fi, c);


	//https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6loca.html
	//�ж�glyph_index��Ӧ��glyph�ĳ���(���ĳ���������һ������ȥ��ǰ�ٳ��Գ���(indexToLocFormat==0������2,==1������4)���õ���.
	//���������0,˵��û�ж�Ӧ��ͼ��,�á�������ʾ.
	int g_curr, g_next;
	if (glyph_index >= fi.numGlyphs) return -1; // glyph index out of range
	if (fi.indexToLocFormat >= 2)    return -1; // unknown index->glyph map format

	if (fi.indexToLocFormat == 0) {
		g_curr = fi.glyf + ttUSHORT(fi.data + fi.loca + glyph_index * 2) * 2;
		g_next = fi.glyf + ttUSHORT(fi.data + fi.loca + glyph_index * 2 + 2) * 2;
	}
	else {
		g_curr = fi.glyf + ttULONG(fi.data + fi.loca + glyph_index * 4);
		g_next = fi.glyf + ttULONG(fi.data + fi.loca + glyph_index * 4 + 4);
	}

	INT glyph_offset = g_curr == g_next ? -1 : g_curr; // if length is 0, return -1
	if (glyph_offset<0)
	{
		return 0;
	}
	SHORT numberOfContours = ttSHORT(fi.data + glyph_offset);
	stbtt_vertex *vertices = NULL;
	if (numberOfContours > 0)
	{
		INT endPtsOfContours = glyph_offset + 10;
		INT ins_len = ttUSHORT(fi.data + glyph_offset + 10 + numberOfContours * 2);
		INT flags_offset = glyph_offset + 10 + numberOfContours * 2 + 2 + ins_len;//flag����ƫ��
		//contoursʹ�õĵ����ʵ����,ʹ�����һ��contour�յ������+1�õ�
		INT real_number_of_point = 1 + ttUSHORT(fi.data + endPtsOfContours + numberOfContours * 2 - 2);
		vertices = (stbtt_vertex *)malloc(real_number_of_point * sizeof(vertices[0]));
		g_stbtt_vertex_size = real_number_of_point;
		if (vertices == 0)
			return 0;

		////////////////flag���ֱ��///////////////////////////
		BYTE F_On_Curve = 1;
		BYTE F_X_Short_Vector = 2;//1:��Ӧx������1�ֽڴ�С,0:��2�ֽ�
		BYTE F_Y_Short_Vector = 4;//..
		BYTE F_Repeat = 8;//��ǰflag���ظ���,����������flag��Ӧ��������ʾ��ǰflag�ظ��Ĵ���

		//F_X_Short_Vector ��1: F_X_Multi_meaning Ϊ1��ʾx������+,0��ʾx������-.
		//F_X_Short_Vector ��0: F_X_Multi_meaning Ϊ1��ʾ��ǰx����ֵ��ǰһ����һ����.F_X_Multi_meaning Ϊ0��ʾ��ǰx������һ��16bit�Ĳ�ֵvector(delta vector)
		BYTE F_X_Multi_meaning = 16;
		//��F_X_Multi_meaning����
		BYTE F_Y_Multi_meaning = 32;
		///////////////////////////////////////////////////////
		
		
		INT point_index = 0;//�������

		INT curr_flag_index = 0;//flags��������(��Ϊ��ѹ������,�����ڱ���֮ǰ����֪��)
		BYTE curr_flag = 0;
		INT curr_flag_repeat_num = 0;//��ǰflag�ظ�����
		//flags������ѹ������(����С��real_number_of_point),���Բ���ֱ������flags����
		for (point_index = 0; point_index<real_number_of_point; point_index++)
	    {
			if (curr_flag_repeat_num>0)
			{
				
				curr_flag_repeat_num--;
			}
			else
			{
				curr_flag = fi.data[flags_offset + curr_flag_index++];
				if (curr_flag & F_Repeat)//��һ���ֽڱ�ʾ��ǰflag�ظ��˶��ٴ�
				{
					curr_flag_repeat_num = fi.data[flags_offset + curr_flag_index++];
				}
			}
			vertices[point_index].type = curr_flag;
			printf("point_index: %d -> curr_flag: %d \n", point_index, curr_flag);
	    }
		//����ѭ������,curr_flag_index�������������ĳ���
		INT x_offset = flags_offset + curr_flag_index * sizeof(BYTE);
		INT curr_x_index = 0;
		//����x����,��flagsһ��,x��������Ҳ��ѹ������
		SHORT x = 0;
		for (point_index = 0; point_index < real_number_of_point; ++point_index) 
		{
			BYTE flags = vertices[point_index].type;

			//��1�ֽڵĳ���
			if (flags & F_X_Short_Vector) 
			{
				SHORT dx = fi.data[x_offset + curr_x_index++ ];
				if (flags & F_X_Multi_meaning)//F_X_Short_Vector ��1: F_X_Multi_meaning Ϊ1��ʾx������+
				{
					x += dx;
				}
				else
				{
					x += -dx;
				}
				
			}
			else //2�ֽڳ���
			{
				if (!(flags & F_X_Multi_meaning)) //F_X_Short_Vector ��0: F_X_Multi_meaning Ϊ1��ʾ��ǰx����ֵ��ǰһ����һ����
				{
					SHORT hi = fi.data[x_offset + curr_x_index];
					SHORT lo = fi.data[x_offset + curr_x_index + 1];
					x = x + SHORT(hi * 256 + lo);
					curr_x_index += 2;
				}
				else
				{

				}
			}
			vertices[point_index].x = x;
			printf("x-> %d: %d \n", point_index, x);
		}
		
		INT y_offset = x_offset + curr_x_index*sizeof(BYTE);
		INT curr_y_index = 0;
		//����x����,��flagsһ��,x��������Ҳ��ѹ������
		SHORT y = 0;
		for (point_index = 0; point_index < real_number_of_point; ++point_index)
		{
			BYTE flags = vertices[point_index].type;

			//��1�ֽڵĳ���
			if (flags & F_Y_Short_Vector)
			{
				SHORT dy = fi.data[y_offset + curr_y_index++];
				if (flags & F_Y_Multi_meaning)//F_X_Short_Vector ��1: F_X_Multi_meaning Ϊ1��ʾx������+
				{
					y += dy;
				}
				else
				{
					y += -dy;
				}

			}
			else //2�ֽڳ���
			{
				if (!(flags & F_Y_Multi_meaning)) //F_X_Short_Vector ��0: F_X_Multi_meaning Ϊ1��ʾ��ǰx����ֵ��ǰһ����һ����
				{
					SHORT hi = fi.data[y_offset + curr_y_index];
					SHORT lo = fi.data[y_offset + curr_y_index + 1];
					y += (hi * 256 + lo);
					curr_y_index += 2;
				}
				else
				{

				}
			}
			vertices[point_index].y = y;
			printf("y-> %d: %d \n", point_index, y);
			g_stbtt_vertex = vertices;
		}
		//��ӡx,y���Ƿ���������
		for (int i = 0; i < real_number_of_point;i++)
		{
			BYTE flags = vertices[i].type;
			INT x = vertices[i].x;
			INT y = vertices[i].y;
			printf("%d: %d , %d,%s \n", i, x, y,(flags&F_On_Curve) ? "on" : "off");
		}

		//��ֵ��ĵ���,���������Ŀ϶�����
		stbtt_vertex * interpolated_vertices = (stbtt_vertex *)malloc( 2 * real_number_of_point * sizeof(vertices[0]));

		//������off�ĵ�,��Ҫ���м��ֵ,��ôpoint���������б仯.
		INT num_after_interpolate = 0;

		//�±ʻ��ĵ������
		INT next_new_contour_point_index = 0;

		 
		//��ǰ�����ڼ����ʻ�
		INT curr_process_contour = 0;
		
		//�������е��x,y�����Ѿ�����,��������Ҫ (1)�������Ĳ��������ϵĵ���зָ�,(2)���ֲ�ͬ��contour
		for (point_index = 0; point_index < real_number_of_point;)
		{
			stbtt_vertex v = vertices[point_index];
			BYTE flags = v.type;
			SHORT x = v.x;
			SHORT y = v.y;

			stbtt_vertex next_v = vertices[point_index+1];
			BYTE next_flags = next_v.type;
			SHORT next_x = next_v.x;
			SHORT next_y = next_v.y;

			//ÿ��ѭ������¼һ�����ߵ�ǰ2�������һ��ֱ�ߵĵ�1����,����Ҫ��ֵ�ĵ���ֱ�Ӽ�¼��ȥ��.
			//�������������,ÿ��ѭ����ʼ�ĵ�p��Ӧ����on curve.���p��off curve,��˵�����ϴβ�ֵ��һ���ĵ�p1;(p1,p,p_next)
			//�����˱���������,p_next
			//�±ʻ�
			if (next_new_contour_point_index == point_index)
			{
				if (!(flags & 1))//�ʻ�����ʼ�������on curve
					return -1;


				if (next_flags & 1)//��һ��Ҳ��on curve,��ô��ֱ�߿�ͷ
				{
					interpolated_vertices[num_after_interpolate] = v;
					interpolated_vertices[num_after_interpolate].type = POINT_TYPE_NEW_CONTOUR | POINT_TYPE_LINE;

					//interpolated_vertices[num_after_interpolate + 1] = next_v;

					num_after_interpolate+=1;
					//last_on_off = 1;
					point_index +=1;

				}
				else//��һ������off curve,��ô�����߿�ͷ
				{
					interpolated_vertices[num_after_interpolate] = v;
					interpolated_vertices[num_after_interpolate].type = POINT_TYPE_NEW_CONTOUR | POINT_TYPE_CURVE;


					interpolated_vertices[num_after_interpolate + 1] = next_v;
					interpolated_vertices[num_after_interpolate + 1].type = 0;

					//��Ҫ�жϽ�����һ���ǲ���off curve,�����off curve,��Ҫ��ֵ.���ҰѲ�ֵ���¼����,��ô�����´�ѭ���ĵ����off curve
					stbtt_vertex next_next_v = vertices[point_index + 1 + 1];
					BYTE next_next_flags = next_next_v.type;
					SHORT next_next_x = next_next_v.x;
					SHORT next_next_y = next_next_v.y;
					if (!(next_next_flags & 1))//next next ��off curve ,��ֵ,��ֵ����������������ϵ�,��������һ��bezier���ߵ���ʼ��
					{
						interpolated_vertices[num_after_interpolate + 1 + 1].x = (next_v.x + next_next_v.x) >> 1;
						interpolated_vertices[num_after_interpolate + 1 + 1].y = (next_v.y + next_next_v.y) >> 1;
						interpolated_vertices[num_after_interpolate + 1 + 1].type = POINT_TYPE_CURVE;
						num_after_interpolate +=3;
						//last_on_off = 1;
						point_index += 2;
					}
					else//next next �� on curve,������,�ȴ��´�ѭ�����д���
					{
						num_after_interpolate += 2;
						point_index += 2;
					}
				}
				//������һ�γ����±ʻ��ĵ������
				next_new_contour_point_index = 1 + ttUSHORT(fi.data + endPtsOfContours + curr_process_contour * 2);
				curr_process_contour++;
			}
			else//�����±ʻ�
			{
				if (flags & 1)//��ǰ�� on curve
				{
					if (next_flags & 1)//��һ��Ҳ��on curve,��ô��ֱ�߿�ͷ
					{
						interpolated_vertices[num_after_interpolate] = v;
						interpolated_vertices[num_after_interpolate].type = POINT_TYPE_LINE;
						num_after_interpolate += 1;
						//last_on_off = 1;
						point_index +=1;

					}
					else//��һ������off curve,��ô�����߿�ͷ
					{
						interpolated_vertices[num_after_interpolate] = v;
						interpolated_vertices[num_after_interpolate].type = POINT_TYPE_CURVE;


						interpolated_vertices[num_after_interpolate + 1] = next_v;
						interpolated_vertices[num_after_interpolate + 1].type = 0;//���Ƶ�,����Ҫ����

						//��Ҫ�жϽ�����һ���ǲ���off curve,�����,��Ҫ��ֵ.
						stbtt_vertex next_next_v = vertices[point_index + 1 + 1];
						BYTE next_next_flags = next_next_v.type;
						SHORT next_next_x = next_next_v.x;
						SHORT next_next_y = next_next_v.y;
						if (!(next_next_flags & 1))//next next ��off curve ,��ֵ,��ֵ����������������ϵ�,��������һ��bezier���ߵ���ʼ��
						{
							interpolated_vertices[num_after_interpolate + 1 + 1].x = (next_v.x + next_next_v.x) >> 1;
							interpolated_vertices[num_after_interpolate + 1 + 1].y = (next_v.y + next_next_v.y) >> 1;
							interpolated_vertices[num_after_interpolate + 1 + 1].type = POINT_TYPE_CURVE;
							num_after_interpolate += 3;
							//last_on_off = 1;
							point_index += 2;
						}
						else//next next �� on curve,������,�ȴ��´�ѭ�����д���
						{
							num_after_interpolate += 2;
							point_index += 2;
						}
					}
				}
				else//��ǰ����off curve,�϶����ǲ�ֵ֮�����һ����,��ֵ��ĵ����������,��ô��ǰ��Ҳ�������ߵĿ��Ƶ�
				{
					//(1) ��ʼ���Ѿ�����һ�����߻�ֱ�ߵĴ����������ӽ�ȥ��
					interpolated_vertices[num_after_interpolate] = v;//(2)��ӿ��Ƶ�
					interpolated_vertices[num_after_interpolate].type = 0;//���Ƶ�϶�������ʼ����,Ҳ�Ͳ���Ҫ����

					if ((next_flags&1))
					{
						num_after_interpolate += 1;
						point_index += 1;
					}
					else//������ֵ,��ֵ�ĵ������߿�ͷ
					{
						interpolated_vertices[num_after_interpolate  + 1].x = (next_v.x + x) >> 1;
						interpolated_vertices[num_after_interpolate  + 1].y = (next_v.y + y) >> 1;
						interpolated_vertices[num_after_interpolate  + 1].type = POINT_TYPE_CURVE;
						num_after_interpolate += 2;
						point_index += 1;
					}
					
				}
			}	
		}

		//
		for (int i = 0; i < num_after_interpolate; i++)
		{
			BYTE flags = interpolated_vertices[i].type;
			INT x = interpolated_vertices[i].x;
			INT y = interpolated_vertices[i].y;
			
			printf("%d: %d , %d",i,x,y);
			if (flags & POINT_TYPE_NEW_CONTOUR)
			{
				printf("%s", "��ͷ");
			}
			if (flags & POINT_TYPE_LINE)
			{
				printf("%s", "ֱ��");
			}
			if (flags & POINT_TYPE_CURVE)
			{
				printf("%s", "����");
			}
			if (flags == 0)//control point
			{
				printf("%s", "���Ƶ�");
			}
			printf("\n");
		}

		g_stbtt_vertex = interpolated_vertices;
		g_stbtt_vertex_size = num_after_interpolate;

	}
	else if (numberOfContours == -1)
	{

	}
	OffsetTable ot;
	memcpy(&ot, ttf_buffer, sizeof(ot));
	return 0;
}

///////////////////////////////////////һ���ǲ����ļ�

#include <stdio.h>
#include<stdlib.h>

#include"glut.h"
#include <windows.h>


int x_pixel_per_grid = 40;
int y_pixel_per_grid = 40;
void gl_draw_grid(int x, int y)
{
	glBegin(GL_POINTS);
	for (int i = x_pixel_per_grid * x; i < x_pixel_per_grid * (x + 1); i++)
	for (int j = y_pixel_per_grid * y; j < y_pixel_per_grid * (y + 1); j++)
	{
		glVertex2f(i, j);
	}

	glEnd();
}

int robin_test()
{

	load_ttf_file(0, 0, 'O');


	return 0;
}



//����Rendering context
void setupRenderingContext() {
	//����clear colorΪ��ɫ
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#if !USE_THEIRS_TEST
	robin_test();
#endif
	//���û�ͼ��ɫ
	glColor3f(1.0f, 1.0f, 0.0f);
}

//���ڴ�С�ı�Ļص�����
void onChangeSize(GLint w, GLint h){
	//����
	GLfloat ratio;

	//��������ϵx(-100.0f,100.0f),y(-100.0f,100.0f),z(-100.0f,100.0f)
	GLfloat coordinateSize = 2000.0f;

	//�����ӿںʹ��ڴ�Сһ��
	glViewport(0, 0, w, h);

	//��ͶӰ����Ӧ�����ľ������
	glMatrixMode(GL_PROJECTION);

	//���õ�ǰָ���ľ���Ϊ��λ����
	glLoadIdentity();
	ratio = (GLfloat)w / (GLfloat)h;

	//����ͶӰ
	// left right bottom top
	if (w < h)
		//glOrtho(-coordinateSize, coordinateSize, -coordinateSize / ratio, coordinateSize / ratio, -coordinateSize, coordinateSize);
		glOrtho(0, 2 * coordinateSize, 0, 2 * coordinateSize / ratio, -coordinateSize, coordinateSize);
	else
		//glOrtho(-coordinateSize * ratio, coordinateSize * ratio, -coordinateSize, coordinateSize, -coordinateSize, coordinateSize);
		glOrtho(0, 2 * coordinateSize * ratio, 0, 2 * coordinateSize, -coordinateSize, coordinateSize);

	//��ģ����ͼ�����ջӦ�����ľ������
	glMatrixMode(GL_MODELVIEW);
	//���õ�ǰָ������Ϊ��λ����
	glLoadIdentity();
}


#define MACRO_SWAP(a,b,type) do{type c = (a);a=(b);b=c;}while(0)


float g_scale = 0.5;


void bresenham_drawline(INT x0, INT y0, INT x1, INT y1, INT color)
{

	x0 *= g_scale;
	y0 *= g_scale;
	x1 *= g_scale;
	y1 *= g_scale;

	//printf("x0:%d,y0:%d,x1:%d,y1:%d\n",x0,y0,x1,y1);
	INT steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)//���б�ʾ���ֵ����1������x��y�����б�ʾ���ֵС��1��
	{
		MACRO_SWAP(x0, y0, INT);
		MACRO_SWAP(x1, y1, INT);
	}
	if (x0 > x1)//��������򣨴����������£�,���������յ�
	{
		MACRO_SWAP(x0, x1, INT);
		MACRO_SWAP(y0, y1, INT);
	}

	//printf("x0:%d,y0:%d,x1:%d,y1:%d\n",x0,y0,x1,y1);
	INT deltax = x1 - x0;
	INT deltay = y1 - y0;
	INT error = 0;
	INT deltaerr = abs(deltay);
	INT ystep;
	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	INT y = y0;
	INT x;
	for (x = x0; x <= x1; x++)
	{
		if (steep)//x �� y �����ǵ������ˣ����Ƶ�ʱ��Ҫ�ٵ�����ȥ
			gl_draw_grid(y, x);
		else
			gl_draw_grid(x, y);
		error = error + deltaerr;
		if (2 * error >= deltax)
		{
			y = y + ystep;
			error = error - deltax;
		}
	}
}

void drawline(INT x1, INT y1, INT x2, INT y2)
{
	x1 *= g_scale;
	y1 *= g_scale;
	x2 *= g_scale;
	y2 *= g_scale;

	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
	glFlush();
}


void drawbezier(INT x1, INT y1, INT x2, INT y2, INT x3, INT y3)
{
	double t = 0;

	INT x_pre = x1;
	INT y_pre = y1;

	for (int i = 1; i <= 1000; i++)
	{
		t = i * 0.001;
		double A = (1 - t)*(1 - t);
		double B = 2 * t*(1 - t);
		double C = t*t;

		INT x = A *x1 + B * x2 + C * x3;
		INT y = A *y1 + B * y2 + C * y3;

		bresenham_drawline(x_pre, y_pre, x, y, 0);
		x_pre = x;
		y_pre = y;

	}

}

int pre_tick;
int char_show = 'A';
//��Ⱦ
void onRender(){
	//����
	glClear(GL_COLOR_BUFFER_BIT);

	int tick = ::GetTickCount();
	if (tick - pre_tick > 1000)
	{

		//load_ttf_file(0, 0, char_show++);

		pre_tick = tick;

	}
	g_stbtt_vertex;
	g_stbtt_vertex_size;

	int contour_start_x, contour_start_y;
	//ע��ÿ���ʻ�contour�����һ�������ƻ���ʼ���!
	int loop = 0;
	for (int i = 0; i < g_stbtt_vertex_size;)
	{
		/*typedef struct
		{
		USHORT x, y, cx, cy;
		unsigned char type, padding;
		} stbtt_vertex;*/
		stbtt_vertex v = g_stbtt_vertex[i];

		if (v.type & POINT_TYPE_NEW_CONTOUR)
		{
			contour_start_x = v.x;
			contour_start_y = v.y;
		}

		if (v.type & POINT_TYPE_CURVE)
		{
			//��һ�����±ʻ�
			if ((g_stbtt_vertex[i + 2].type & POINT_TYPE_NEW_CONTOUR) | (i + 2) == g_stbtt_vertex_size)
			{
				drawbezier(
					g_stbtt_vertex[i].x, g_stbtt_vertex[i].y,
					g_stbtt_vertex[i + 1].x, g_stbtt_vertex[i + 1].y,
					contour_start_x, contour_start_y);
			}
			else
			{
				drawbezier(
					g_stbtt_vertex[i].x, g_stbtt_vertex[i].y,
					g_stbtt_vertex[i + 1].x, g_stbtt_vertex[i + 1].y,
					g_stbtt_vertex[i + 2].x, g_stbtt_vertex[i + 2].y);
			}
			i += 2;
		}
		else if (v.type & POINT_TYPE_LINE)
		{
			if ((g_stbtt_vertex[i + 1].type & POINT_TYPE_NEW_CONTOUR) | (i + 1) == g_stbtt_vertex_size)
			{
				bresenham_drawline(
					g_stbtt_vertex[i].x, g_stbtt_vertex[i].y,
					contour_start_x, contour_start_y, 0);
			}
			else
			{
				bresenham_drawline(
					g_stbtt_vertex[i].x, g_stbtt_vertex[i].y,
					g_stbtt_vertex[i + 1].x, g_stbtt_vertex[i + 1].y, 0);
			}

			i += 1;
		}
		else
		{
			printf(" loop %d index %d error!!!!%d,%d,%d \n", loop, i, v.x, v.y, v.type);
			i++;
		}
		loop++;
	}

#endif
	glEnd();
	//glBitmap(512, 512, 0, 0, 0, 0, temp_bitmap);

	//��������������
	glutSwapBuffers();
}

int main(int argv, char *argc[])
{
	pre_tick = GetTickCount();
	//��ʼ��glut
	glutInit(&argv, argc);
	//��ʼ����ʾģʽ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//���ô��ڴ�С
	glutInitWindowSize(800, 600);
	//��������
	glutCreateWindow("Point Demo");
	//ע��ص�����
	glutReshapeFunc(onChangeSize);
	glutDisplayFunc(onRender);

	setupRenderingContext();

	glutMainLoop();

	return 0;
}



#endif