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

//平台id
#define CMAP_PLATFORM_ID_CUSTOM		4
#define CMAP_PLATFORM_ID_WIN		3
#define CMAP_PLATFORM_ID_MAC		1
#define CMAP_PLATFORM_ID_UNICODE	0

//编码id
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

////////////////////////////////////////////这些结构是.ttc文件和.otf/.ttf文件 的文件结构示意,并没有实际用到/////////////////////////
//如果是包含多个字体的TTC文件,那么可能是下面两种中的某个
//下面两个结构体针对的是 .ttc格式的文件结构.
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


//如果只有一个字体,那么文件开头就是OffsetTable
//如果是TrueType collection,那么每个字体文件的开始指针是由 TTCHeader 标识(indidated)
struct OffsetTable
{
	FIXED 	sfnt_version; //0x00010000 for version 1.0.如果包含的是TTF数据,那么1.0是版本号.如果包含的是CFF数据,那么是'OTTO'
	USHORT 	numTables;// 	Number of tables.
	USHORT 	searchRange;// (Maximum power of 2 <= numTables) x 16.
	USHORT 	entrySelector;// 	Log2(maximum power of 2 <= numTables).
	USHORT 	rangeShift;// 	NumTables x 16 - searchRange.
};
//紧接着OffsetTable的就是TableRecord条目,按照tag排序的
struct TableRecord
{
	ULONG 	tag; //	4 - byte identifier.都是4字节的名字,如果不够4字节,后面补0
	ULONG 	checkSum; // 	CheckSum for this table.
	ULONG 	offset; // 	Offset from beginning of TrueType font file.
	ULONG 	length; // 	Length of this table.
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//cmap : 字符到字型的索引映射表
//因为为了支持不止1种字符编码结构,它可能有不止1个子表(subtable)
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
//每个cmap子表可能是7种格式之一：
//format 0, format 2, format 4, format 6, format 8.0, format 10.0, and format 12.0 。

//Byte encoding table 适用于编码和glyph索引在单字节范围内的字符。这是标准的苹果字符到glyph的映射方式。
struct cmap_subtable_format0
{
	USHORT 	format;//	 	Format number is set to 0.
	USHORT 	length;//		This is the length in bytes of the subtable.
	USHORT 	language;//	 	Please see “Note on the language field in 'cmap' subtables“ in this document.
	BYTE 	glyphIdArray[256];//	 	An array that maps character codes to glyph index values.
};
//对cjk字体很有用.使用了8/16bit混合编码,它使用了某个确定值的字节表明这是一个2byte字符的第一个字节.
//另外,甚至对于2-byte字符,它到字型(glyph)索引的映射仍然很依赖于第一个字节.
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
	USHORT 	language;//	 	Please see “Note on the language field in 'cmap' subtables“ in this document.
	USHORT 	subHeaderKeys[256];//	 	Array that maps high bytes to subHeaders : value is subHeader index * 8.
	subHeaders_of_format2 subHeaders[0xFFFF];//	 	Variable - length array of subHeader structures.
	USHORT 	glyphIndexArray[0xFFFF];//	 	Variable - length array containing subarrays used for mapping the low byte of 2 - byte characters.
};

//Format 4 是双字节编码。这种格式用于字体文件中的编码分布与几个连续的区域，在区域之间可能有一些保留的空白。
//而有一些编码可能并不会与字体中的glyph对应。
struct cmap_subtable_format4
{
#define  segCount 10000
#define  arbitrary_length 10000
	USHORT 	format;//	 	 	Format number is set to 4.
	USHORT 	length;//	 		This is the length in bytes of the subtable.
	USHORT 	language;//	 	 	Please see “Note on the language field in 'cmap' subtables“ in this document.
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

//真正有用的字体数据结构
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

//大端转为小端
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

	//if (check_tag(font, (BYTE*)"ttcf")) return 1;   //.TTC格式的库
	return 0;
}

static int check_is_ttc(const BYTE *font)
{
	if (check_tag(font, (BYTE*)"ttcf")) return 1;   //.TTC格式的库
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


//从.ttc或者.ttf或者.otf获取字体在文件中的偏移量(byte)
int get_font_offset_from_index(const BYTE *font_collection, int index)
{
	// if it's just a font, there's only one valid index
	//if (stbtt__isfont(font_collection))
	//	return index == 0 ? 0 : -1;

	// 如果是多个字体组成的.ttc文件
	if (check_is_ttc(font_collection)) 
	{
		// version 1?
		if (ttULONG(font_collection + 4) == 0x00010000 || ttULONG(font_collection + 4) == 0x00020000) {
			INT numFonts = ttLONG(font_collection + 8);
			if (index >= numFonts)
				return -1;
			unsigned long offset = ttULONG(font_collection + 12 + index * 4);//这是提取TTCHeader(v1或v2)中的offsetTable中的偏移值
			return offset;
		}
	}
	//如果是单个otf或者ttf字体文件,偏移值都是从0开始
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
	//cmap 表头
	/*
	{
		UInt16	version;	Version number (Set to zero)
		UInt16	numberSubtables;	Number of encoding subtables
	}
	*/
	//而 cmap 子表可能有7种形式:format 0, format 2, format 4, format 6,format 8.0, format 10.0, and format 12.0
	//按照平台id和和平台特定id升序排列
	/*
	 {
	 UInt16	 platformID	; Platform identifier
	 UInt16	 platformSpecificID	 ;Platform-specific encoding identifier
	 UInt32	 offset	 ;Offset of the mapping table
	 }
	*/
	int numTables = ttUSHORT(info->data + info->cmap + 2);//2 == sizeof(version)
	for (int i = 0; i < numTables; ++i) {
		UINT subtable_record = info->cmap + 4 + 8 * i;//4==sizeof(cmap表头),8==sizeof(子表头)
		// find an encoding we understand:
		int platformid = ttUSHORT(info->data + subtable_record);
		int encodingid = ttUSHORT(info->data + subtable_record + 2);
		printf("platformid:%d,encoding_id:%d\n", platformid, encodingid);
		//寻找需要的平台上的编码id对应的cmap子表的偏移(cmap子表可能是format 0, format 2, format 4, format 6,format 8.0, format 10.0, and format 12.0中的某一种)
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
		//		info->index_map = info->cmap + ttULONG(info->data + encoding_record + 4); // 4 == sizeof(platformID) + sizeof(platformSpecificID),是需要的子表的offset
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
	INT index_map = info->index_map;//index_map指向的是某个平台id和encodingid的cmap的子表,有几种format,需要根据格式来判断,进行不同的查找

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
		if (unicode_codepoint > ttUSHORT(data + endCount + (rangeShift - 1) * 2)) //如果rangeShift最后一个元素不满足大于等于codepoint,直接跳到searchRange的第一个,下标是rangeShift
		{
			search_index = search_index + rangeShift;
		}
		int origin_index = search_index;
		for (int seg = 0; seg < segcount; seg++)
		{
			printf("第%d个的startCode:%d,endCode: %d \n", seg, ttUSHORT(data + index_map + 14 + segcount * 2 + 2 + 2 * seg), ttUSHORT(data + endCount + 2 * seg));
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
		if (search_index == origin_index)//search_index一次都没改变过,说明search_index右边(不包括它本身)没有比unicode_codepoint小的,需要检查下一个是否满足大于等于它
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
		else//经过上面循环之后,search_index是小于unicode_point的边界,那么大于等于的在后面,需要+1
		{
			search_index++;
		}

		{
			USHORT offset, start;
			USHORT item = (USHORT)search_index;

			//printf("第0个的startCode:%d,endCode: %d \n", ttUSHORT(data + index_map + 14 + segcount * 2 + 2), ttUSHORT(data + endCount));
			//printf("%d,%d \n", ttUSHORT(data + endCount),ttUSHORT(data + endCount + 2 * item));
			//printf("第1个的startCode:%d,endCode: %d \n", ttUSHORT(data + index_map + 14 + segcount * 2 + 2 + 2 * 1), ttUSHORT(data + endCount + 2 * 1));
			assert(unicode_codepoint <= ttUSHORT(data + endCount + 2 * item));
			
			start = ttUSHORT(data + index_map + 14 + segcount * 2 + 2 + 2 * item);
			if (unicode_codepoint < start)
				return 0;

			//item = 898;
			offset = ttUSHORT(data + index_map + 14 + segcount * 6 + 2 + 2 * item);//idRangeOffset数组位置
			if (offset == 0)
				return (USHORT)(unicode_codepoint + ttSHORT(data + index_map + 14 + segcount * 4 + 2 + 2 * item));//第item个idDelta,直接加到codepoint上即可
			return ttUSHORT(data + index_map + 14 + segcount * 6 + 2 + 2 * item + offset + (unicode_codepoint - start) * 2 );//否则,
		}
	}
	else
	{
		return -1;
	}

}

//计算表的校验和
ULONG
CalcTableChecksum(ULONG *Table, ULONG Length)
{
	ULONG Sum = 0L;
	ULONG *Endptr = Table + ((Length + 3) & ~3) / sizeof(ULONG);//意味着所有的表都是4字节对齐的,不够4字节的后面补0,否则是不正确的.
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
	//FILE *file = fopen(filename ? filename : "C:/Users/Robin/Desktop/fonts/roman.fon", "rb");//无效字体(对应当前的处理程序来说)
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

	//从文件缓存里读取第font_index个字体在缓冲里的距离
	//>=0 成功
	//<0  失败
	UINT font_offset = get_font_offset_from_index(ttf_buffer, font_index);
	if (font_offset<0)
	{
		return -1;
	}

	fontinfo fi;

	get_fontinfo_from_font_offset(&fi, ttf_buffer, font_offset,3,1);

	
	//获得glyph的索引
	INT glyph_index = get_glyph_index_from_codepoint(&fi, c);


	//https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6loca.html
	//判断glyph_index对应的glyph的长度(它的长度是用下一个来减去当前再乘以长度(indexToLocFormat==0长度是2,==1长度是4)来得到的.
	//如果长度是0,说明没有对应的图形,用□等来显示.
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
		INT flags_offset = glyph_offset + 10 + numberOfContours * 2 + 2 + ins_len;//flag数组偏移
		//contours使用的点的真实数量,使用最后一个contour终点的索引+1得到
		INT real_number_of_point = 1 + ttUSHORT(fi.data + endPtsOfContours + numberOfContours * 2 - 2);
		vertices = (stbtt_vertex *)malloc(real_number_of_point * sizeof(vertices[0]));
		g_stbtt_vertex_size = real_number_of_point;
		if (vertices == 0)
			return 0;

		////////////////flag各种标记///////////////////////////
		BYTE F_On_Curve = 1;
		BYTE F_X_Short_Vector = 2;//1:对应x坐标是1字节大小,0:是2字节
		BYTE F_Y_Short_Vector = 4;//..
		BYTE F_Repeat = 8;//当前flag是重复的,而接下来的flag对应的整数表示当前flag重复的次数

		//F_X_Short_Vector 是1: F_X_Multi_meaning 为1表示x坐标是+,0表示x坐标是-.
		//F_X_Short_Vector 是0: F_X_Multi_meaning 为1表示当前x坐标值和前一个是一样的.F_X_Multi_meaning 为0表示当前x坐标是一个16bit的差值vector(delta vector)
		BYTE F_X_Multi_meaning = 16;
		//和F_X_Multi_meaning类似
		BYTE F_Y_Multi_meaning = 32;
		///////////////////////////////////////////////////////
		
		
		INT point_index = 0;//点的索引

		INT curr_flag_index = 0;//flags数组索引(因为是压缩过的,所以在遍历之前并不知道)
		BYTE curr_flag = 0;
		INT curr_flag_repeat_num = 0;//当前flag重复几次
		//flags可能是压缩过的(数量小于real_number_of_point),所以不能直接索引flags数组
		for (point_index = 0; point_index<real_number_of_point; point_index++)
	    {
			if (curr_flag_repeat_num>0)
			{
				
				curr_flag_repeat_num--;
			}
			else
			{
				curr_flag = fi.data[flags_offset + curr_flag_index++];
				if (curr_flag & F_Repeat)//下一个字节表示当前flag重复了多少次
				{
					curr_flag_repeat_num = fi.data[flags_offset + curr_flag_index++];
				}
			}
			vertices[point_index].type = curr_flag;
			printf("point_index: %d -> curr_flag: %d \n", point_index, curr_flag);
	    }
		//上面循环过后,curr_flag_index就是数组真正的长度
		INT x_offset = flags_offset + curr_flag_index * sizeof(BYTE);
		INT curr_x_index = 0;
		//加载x坐标,和flags一样,x坐标数组也是压缩过的
		SHORT x = 0;
		for (point_index = 0; point_index < real_number_of_point; ++point_index) 
		{
			BYTE flags = vertices[point_index].type;

			//是1字节的长度
			if (flags & F_X_Short_Vector) 
			{
				SHORT dx = fi.data[x_offset + curr_x_index++ ];
				if (flags & F_X_Multi_meaning)//F_X_Short_Vector 是1: F_X_Multi_meaning 为1表示x坐标是+
				{
					x += dx;
				}
				else
				{
					x += -dx;
				}
				
			}
			else //2字节长度
			{
				if (!(flags & F_X_Multi_meaning)) //F_X_Short_Vector 是0: F_X_Multi_meaning 为1表示当前x坐标值和前一个是一样的
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
		//加载x坐标,和flags一样,x坐标数组也是压缩过的
		SHORT y = 0;
		for (point_index = 0; point_index < real_number_of_point; ++point_index)
		{
			BYTE flags = vertices[point_index].type;

			//是1字节的长度
			if (flags & F_Y_Short_Vector)
			{
				SHORT dy = fi.data[y_offset + curr_y_index++];
				if (flags & F_Y_Multi_meaning)//F_X_Short_Vector 是1: F_X_Multi_meaning 为1表示x坐标是+
				{
					y += dy;
				}
				else
				{
					y += -dy;
				}

			}
			else //2字节长度
			{
				if (!(flags & F_Y_Multi_meaning)) //F_X_Short_Vector 是0: F_X_Multi_meaning 为1表示当前x坐标值和前一个是一样的
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
		//打印x,y和是否在曲线上
		for (int i = 0; i < real_number_of_point;i++)
		{
			BYTE flags = vertices[i].type;
			INT x = vertices[i].x;
			INT y = vertices[i].y;
			printf("%d: %d , %d,%s \n", i, x, y,(flags&F_On_Curve) ? "on" : "off");
		}

		//插值后的点数,分配两倍的肯定够了
		stbtt_vertex * interpolated_vertices = (stbtt_vertex *)malloc( 2 * real_number_of_point * sizeof(vertices[0]));

		//连续的off的点,需要在中间插值,那么point的数量就有变化.
		INT num_after_interpolate = 0;

		//新笔画的点的索引
		INT next_new_contour_point_index = 0;

		 
		//当前处理到第几个笔画
		INT curr_process_contour = 0;
		
		//上面所有点的x,y坐标已经有了,接下来需要 (1)把连续的不在曲线上的点进行分割,(2)区分不同的contour
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

			//每次循环都记录一个曲线的前2个点或者一个直线的第1个点,而需要插值的点是直接记录进去的.
			//所以正常情况下,每次循环开始的点p都应该是on curve.如果p是off curve,则说明是上次插值了一个的点p1;(p1,p,p_next)
			//构成了贝塞尔曲线,p_next
			//新笔画
			if (next_new_contour_point_index == point_index)
			{
				if (!(flags & 1))//笔画的起始点必须是on curve
					return -1;


				if (next_flags & 1)//下一个也是on curve,那么是直线开头
				{
					interpolated_vertices[num_after_interpolate] = v;
					interpolated_vertices[num_after_interpolate].type = POINT_TYPE_NEW_CONTOUR | POINT_TYPE_LINE;

					//interpolated_vertices[num_after_interpolate + 1] = next_v;

					num_after_interpolate+=1;
					//last_on_off = 1;
					point_index +=1;

				}
				else//下一个点是off curve,那么是曲线开头
				{
					interpolated_vertices[num_after_interpolate] = v;
					interpolated_vertices[num_after_interpolate].type = POINT_TYPE_NEW_CONTOUR | POINT_TYPE_CURVE;


					interpolated_vertices[num_after_interpolate + 1] = next_v;
					interpolated_vertices[num_after_interpolate + 1].type = 0;

					//还要判断接下下一个是不是off curve,如果是off curve,需要插值.并且把插值点记录下来,那么导致下次循环的点就是off curve
					stbtt_vertex next_next_v = vertices[point_index + 1 + 1];
					BYTE next_next_flags = next_next_v.type;
					SHORT next_next_x = next_next_v.x;
					SHORT next_next_y = next_next_v.y;
					if (!(next_next_flags & 1))//next next 是off curve ,插值,插值的这个点是在曲线上的,并且是下一个bezier曲线的起始点
					{
						interpolated_vertices[num_after_interpolate + 1 + 1].x = (next_v.x + next_next_v.x) >> 1;
						interpolated_vertices[num_after_interpolate + 1 + 1].y = (next_v.y + next_next_v.y) >> 1;
						interpolated_vertices[num_after_interpolate + 1 + 1].type = POINT_TYPE_CURVE;
						num_after_interpolate +=3;
						//last_on_off = 1;
						point_index += 2;
					}
					else//next next 是 on curve,不处理,等待下次循环进行处理
					{
						num_after_interpolate += 2;
						point_index += 2;
					}
				}
				//更新下一次出现新笔画的点的索引
				next_new_contour_point_index = 1 + ttUSHORT(fi.data + endPtsOfContours + curr_process_contour * 2);
				curr_process_contour++;
			}
			else//不是新笔画
			{
				if (flags & 1)//当前点 on curve
				{
					if (next_flags & 1)//下一个也是on curve,那么是直线开头
					{
						interpolated_vertices[num_after_interpolate] = v;
						interpolated_vertices[num_after_interpolate].type = POINT_TYPE_LINE;
						num_after_interpolate += 1;
						//last_on_off = 1;
						point_index +=1;

					}
					else//下一个点是off curve,那么是曲线开头
					{
						interpolated_vertices[num_after_interpolate] = v;
						interpolated_vertices[num_after_interpolate].type = POINT_TYPE_CURVE;


						interpolated_vertices[num_after_interpolate + 1] = next_v;
						interpolated_vertices[num_after_interpolate + 1].type = 0;//控制点,不需要类型

						//还要判断接下下一个是不是off curve,如果是,需要插值.
						stbtt_vertex next_next_v = vertices[point_index + 1 + 1];
						BYTE next_next_flags = next_next_v.type;
						SHORT next_next_x = next_next_v.x;
						SHORT next_next_y = next_next_v.y;
						if (!(next_next_flags & 1))//next next 是off curve ,插值,插值的这个点是在曲线上的,并且是下一个bezier曲线的起始点
						{
							interpolated_vertices[num_after_interpolate + 1 + 1].x = (next_v.x + next_next_v.x) >> 1;
							interpolated_vertices[num_after_interpolate + 1 + 1].y = (next_v.y + next_next_v.y) >> 1;
							interpolated_vertices[num_after_interpolate + 1 + 1].type = POINT_TYPE_CURVE;
							num_after_interpolate += 3;
							//last_on_off = 1;
							point_index += 2;
						}
						else//next next 是 on curve,不处理,等待下次循环进行处理
						{
							num_after_interpolate += 2;
							point_index += 2;
						}
					}
				}
				else//当前点是off curve,肯定就是插值之后的下一个点,插值后的点是曲线起点,那么当前点也就是曲线的控制点
				{
					//(1) 起始点已经在上一个曲线或直线的处理过程中添加进去了
					interpolated_vertices[num_after_interpolate] = v;//(2)添加控制点
					interpolated_vertices[num_after_interpolate].type = 0;//控制点肯定不是起始点了,也就不需要类型

					if ((next_flags&1))
					{
						num_after_interpolate += 1;
						point_index += 1;
					}
					else//继续插值,插值的点是曲线开头
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
				printf("%s", "开头");
			}
			if (flags & POINT_TYPE_LINE)
			{
				printf("%s", "直线");
			}
			if (flags & POINT_TYPE_CURVE)
			{
				printf("%s", "曲线");
			}
			if (flags == 0)//control point
			{
				printf("%s", "控制点");
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

///////////////////////////////////////一下是测试文件

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



//设置Rendering context
void setupRenderingContext() {
	//设置clear color为黑色
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#if !USE_THEIRS_TEST
	robin_test();
#endif
	//设置绘图颜色
	glColor3f(1.0f, 1.0f, 0.0f);
}

//窗口大小改变的回调函数
void onChangeSize(GLint w, GLint h){
	//横宽比
	GLfloat ratio;

	//设置坐标系x(-100.0f,100.0f),y(-100.0f,100.0f),z(-100.0f,100.0f)
	GLfloat coordinateSize = 2000.0f;

	//设置视口和窗口大小一样
	glViewport(0, 0, w, h);

	//对投影矩阵应用随后的矩阵操作
	glMatrixMode(GL_PROJECTION);

	//重置当前指定的矩阵为单位矩阵
	glLoadIdentity();
	ratio = (GLfloat)w / (GLfloat)h;

	//正交投影
	// left right bottom top
	if (w < h)
		//glOrtho(-coordinateSize, coordinateSize, -coordinateSize / ratio, coordinateSize / ratio, -coordinateSize, coordinateSize);
		glOrtho(0, 2 * coordinateSize, 0, 2 * coordinateSize / ratio, -coordinateSize, coordinateSize);
	else
		//glOrtho(-coordinateSize * ratio, coordinateSize * ratio, -coordinateSize, coordinateSize, -coordinateSize, coordinateSize);
		glOrtho(0, 2 * coordinateSize * ratio, 0, 2 * coordinateSize, -coordinateSize, coordinateSize);

	//对模型视图矩阵堆栈应用随后的矩阵操作
	glMatrixMode(GL_MODELVIEW);
	//重置当前指定矩阵为单位矩阵
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
	if (steep)//如果斜率绝对值大于1，交换x和y，变成斜率绝对值小于1的
	{
		MACRO_SWAP(x0, y0, INT);
		MACRO_SWAP(x1, y1, INT);
	}
	if (x0 > x1)//如果反方向（从右上至左下）,交换起点和终点
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
		if (steep)//x 和 y 坐标是调换过了，绘制的时候要再调换回去
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
//渲染
void onRender(){
	//清理
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
	//注意每个笔画contour的最后一个点是绕回起始点的!
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
			//下一个是新笔画
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

	//交换两个缓冲区
	glutSwapBuffers();
}

int main(int argv, char *argc[])
{
	pre_tick = GetTickCount();
	//初始化glut
	glutInit(&argv, argc);
	//初始化显示模式
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//设置窗口大小
	glutInitWindowSize(800, 600);
	//创建窗口
	glutCreateWindow("Point Demo");
	//注册回调函数
	glutReshapeFunc(onChangeSize);
	glutDisplayFunc(onRender);

	setupRenderingContext();

	glutMainLoop();

	return 0;
}



#endif