#ifndef __FUNC_H
#define __FUNC_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_VALUE_IN_BYTE 255
#define MAX_LEN_OF_TXT_NAME 250
#define ROW 0
#define COL 1
#define BITS_IN_BYTE 8


typedef struct _grayimage {
	unsigned short rows, cols;
	unsigned char **pixels;
} grayImage;

typedef unsigned short imgPos[2];

typedef struct _treeNodeListCell;

typedef struct _treeNode {
	imgPos position;
	struct _treeNodeListCell *next_possible_positions;
} treeNode;

typedef struct _treeNodeListCell {
	treeNode *node;
	struct _treeNodeListCell *next;
} treeNodeListCell;

typedef struct _segment {
	treeNode *root;
} Segment;

typedef struct _imgPosCell {
	imgPos position;
	struct _imgPosCell *next, *prev;
}imgPosCell;

typedef struct _imgPosList {
	imgPosCell *head, *tail;
}imgPosList;


grayImage *readPGM(char *fname);
int howManyDigits(int num);

Segment findSingleSegment(grayImage* img, imgPos start, unsigned char threshold);
treeNode* findSingleSegmentHelper(grayImage* img, unsigned short x, unsigned short y, unsigned char leftRange, unsigned char rightRange, bool** boolArr);
treeNode* createNewTreeNode(unsigned short x, unsigned short y, treeNodeListCell* nextPossiblePositions);
treeNodeListCell* createNewTreeNodeListCell(unsigned short x, unsigned short y, treeNodeListCell* next);
treeNode* checkNeighbours(grayImage* img, unsigned short x, unsigned short y, unsigned char leftRange, unsigned char rightRange, bool** boolArr);
int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments);
void buildRange(unsigned char startValue, unsigned char threshold, unsigned char *leftRange, unsigned char *rightRange);

int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments);
void convertFromTreeToList(treeNode * root, imgPosList * lst);
void insertDataToEndList(imgPosList * lst, unsigned short x, unsigned short y);
imgPosCell * createNewImgPosCell(unsigned short x, unsigned short y, imgPosCell * next, imgPosCell * prev);
void insertImgPosCellToEndList(imgPosList * lst, imgPosCell * tail);

grayImage* colorSegments(imgPosList* segments, unsigned int size);
void findMaxes(imgPosList* segments, unsigned int size, int* maxRow, int* maxCol);

void saveCompressed(char *file_name, grayImage *image, unsigned char max_gray_level);

void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name);

int howManyDigits(int num);
int howManyBits(int num);

unsigned char** createMatrix(int maxRow, int maxCol);
void freeList(imgPosList* lst);
void freeTree(treeNode* node);

void checkMemoryAllocation(void* ptr);

#endif