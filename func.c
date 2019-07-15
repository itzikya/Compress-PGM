#include "func.h"
grayImage *readPGM(char *fname)
{
	char* type;
	grayImage *res;
	FILE* f;
	int i, j, maxDigits;
	unsigned short rows, cols, maxGrey;

	res = (grayImage*)malloc(sizeof(grayImage));
	checkMemoryAllocation(res);

	f = fopen(fname, "r");
	checkMemoryAllocation(f);

	//scan values from file
	type = (char*)calloc(3, sizeof(char));
	fscanf(f, "%s", type);
	fscanf(f, "%hu", &cols);
	fscanf(f, "%hu", &rows);
	fscanf(f, "%hu", &maxGrey);
	maxDigits = howManyDigits(maxGrey);

	res->pixels = (unsigned char**)malloc((rows + 1) * sizeof(unsigned char*));
	checkMemoryAllocation(res->pixels);
	for (i = 0; i < rows; i++)
	{
		res->pixels[i] = (unsigned char*)malloc(((maxDigits + 1)*cols + 1) * sizeof(unsigned char));
		checkMemoryAllocation(res->pixels[i]);
		for (j = 0; j < cols; j++)
		{
			fscanf(f, "%hhu", &(res->pixels[i][j]));
		}
	}
	fclose(f);
	res->rows = rows;
	res->cols = cols;
	return (res);
}


Segment findSingleSegment(grayImage* img, imgPos start, unsigned char threshold)
{
	Segment res;
	unsigned short rows, cols;
	unsigned char leftRange, rightRange;
	bool** statusArr;

	rows = img->rows;
	cols = img->cols;

	statusArr = (bool**)calloc(rows, sizeof(bool*));
	checkMemoryAllocation(statusArr);

	for (int i = 0; i < rows; i++)
	{
		statusArr[i] = (bool*)calloc(cols, sizeof(bool));
		checkMemoryAllocation(statusArr[i]);
	}

	//find requested range
	buildRange(img->pixels[start[ROW]][start[COL]], threshold, &leftRange, &rightRange);
	//build tree in recursion
	res.root = findSingleSegmentHelper(img, start[ROW], start[COL], leftRange, rightRange, statusArr);

	for (int i = 0; i < rows; i++)
	{
		free(statusArr[i]);
	}
	free(statusArr);
	return res;
}

treeNode* findSingleSegmentHelper(grayImage* img, unsigned short x, unsigned short y, unsigned char leftRange, unsigned char rightRange, bool** statusArr)
{
	treeNode* root;
	treeNodeListCell* curr;
	//link relevant neighbours to root
	root = checkNeighbours(img, x, y, leftRange, rightRange, statusArr);
	curr = root->next_possible_positions;

	if (curr == NULL) //stopping condition - there is no next possible positions
	{
		return;
	}
	else
	{
		while (curr != NULL)
		{
			//link every relevant neighbour in recursion
			curr->node = findSingleSegmentHelper(img, curr->node->position[ROW], curr->node->position[COL], leftRange, rightRange, statusArr);
			curr = curr->next;
		}
	}
	return (root);
}

treeNode* checkNeighbours(grayImage* img, unsigned short x, unsigned short y, unsigned char leftRange, unsigned char rightRange, bool** statusArr)
{
	treeNode* res;
	treeNodeListCell *saver = NULL;

	//flag relevant roots in boolean array
	statusArr[x][y] = true;

	//link relevant neighbours for root
	res = createNewTreeNode(x, y, NULL);
	for (int i = x - 1; i <= x + 1; i++)
	{
		for (int j = y - 1; j <= y + 1; j++)
		{
			if (i >= 0 && i < img->rows && j >= 0 && j < img->cols)
			{
				if (statusArr[i][j] == false && img->pixels[i][j] >= leftRange && img->pixels[i][j] <= rightRange)
				{
					//if neighbour is relevant - add it to next possible positions
					statusArr[i][j] = true;
					if ((res->next_possible_positions) == NULL) //make new next possible positions list
					{
						res->next_possible_positions = createNewTreeNodeListCell(i, j, NULL);
						saver = res->next_possible_positions;
					}
					else //add to next possible positions list
					{
						saver->next = createNewTreeNodeListCell(i, j, NULL);
						saver = saver->next;
					}
				}
			}
		}
	}
	return (res);
}

treeNodeListCell* createNewTreeNodeListCell(unsigned short x, unsigned short y, treeNodeListCell* next)
{
	treeNodeListCell* res;
	res = (treeNodeListCell*)malloc(sizeof(treeNodeListCell));
	checkMemoryAllocation(res);
	res->node = createNewTreeNode(x, y, NULL);
	res->next = next;
}

treeNode* createNewTreeNode(unsigned short x, unsigned short y, treeNodeListCell* nextPossiblePositions)
{
	treeNode* res;

	res = (treeNode*)malloc(sizeof(treeNode));
	checkMemoryAllocation(res);
	res->position[ROW] = x;
	res->position[COL] = y;
	res->next_possible_positions = nextPossiblePositions;

	return (res);
}

void buildRange(unsigned char startValue, unsigned char threshold, unsigned char *leftRange, unsigned char *rightRange)
{
	int start = (int)startValue;
	int range = (int)threshold;

	if (start + range > MAX_VALUE_IN_BYTE)
	{
		*rightRange = MAX_VALUE_IN_BYTE;
	}
	else
	{
		*rightRange = start + range;
	}
	if (start - range < 0)
	{
		*leftRange = 0;
	}
	else
	{
		*leftRange = start - range;
	}
}

int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments)
{
	int counter = 0;
	unsigned short rows, cols, i, j;
	unsigned char leftRange, rightRange;
	bool** statusArr;
	treeNode* rootRes;
	imgPosList* res;

	rows = img->rows;
	cols = img->cols;

	statusArr = (bool**)calloc(rows, sizeof(bool*));
	checkMemoryAllocation(statusArr);

	for (i = 0; i < rows; i++)
	{
		statusArr[i] = (bool*)calloc(cols, sizeof(bool));
		checkMemoryAllocation(statusArr[i]);
	}

	res = (imgPosList*)malloc((sizeof(imgPosList)*cols*rows));
	checkMemoryAllocation(res);

	//search for unsegmented values, and link a segment for them
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			if (statusArr[i][j] != true)
			{
				//build segment
				res[counter].head = res[counter].tail = NULL;
				buildRange(img->pixels[i][j], threshold, &leftRange, &rightRange);
				rootRes = createNewTreeNode(i, j, NULL);
				//build tree for root
				rootRes = findSingleSegmentHelper(img, i, j, leftRange, rightRange, statusArr);
				//convert from one segment structure to a list structure
				convertFromTreeToList(rootRes, &res[counter]);
				counter++;
				freeTree(rootRes);
			}
		}
	}

	res = (imgPosList*)realloc(res, counter * (sizeof(imgPosList)));

	for (int i = 0; i < rows; i++)
	{
		free(statusArr[i]);
	}
	free(statusArr);

	*segments = res;

	return (counter);
}

void convertFromTreeToList(treeNode * root, imgPosList * lst)
{
	treeNodeListCell * curr;
	//insert roots' data to list
	insertDataToEndList(lst, root->position[ROW], root->position[COL]);

	if (root->next_possible_positions == NULL) //stopping condition - no next possible positions
	{
		return;
	}

	else
	{
		//add relevant neighbours to list in recursion
		curr = root->next_possible_positions;
		while (curr != NULL)
		{
			convertFromTreeToList(curr->node, lst);
			curr = curr->next;
		}
	}
}

void insertDataToEndList(imgPosList * lst, unsigned short x, unsigned short y)
{
	imgPosCell * newTail;
	newTail = createNewImgPosCell(x, y, NULL, NULL);
	insertImgPosCellToEndList(lst, newTail);
}

imgPosCell* createNewImgPosCell(unsigned short x, unsigned short y, imgPosCell * next, imgPosCell * prev)
{
	imgPosCell * res;
	res = (imgPosCell*)malloc(sizeof(imgPosCell));
	checkMemoryAllocation(res);

	res->position[ROW] = x;
	res->position[COL] = y;
	res->next = next;
	res->prev = prev;

	return (res);
}

void insertImgPosCellToEndList(imgPosList * lst, imgPosCell * tail)
{
	if (lst->head == NULL)
	{
		lst->head = lst->tail = tail;
		tail->next = tail->prev = NULL;
	}

	else
	{
		tail->next = NULL;
		tail->prev = lst->tail;
		lst->tail->next = tail;
		lst->tail = tail;
	}
}

grayImage* colorSegments(imgPosList* segments, unsigned int size)
{
	int maxRow = 0, maxCol = 0;
	grayImage* res;
	imgPosCell* curr = segments[ROW].head;
	unsigned int i;

	//break if there is only one segment
	if (size == 1)
	{
		printf("Only one segment! No option for colouring\n");
		main();
	}

	//find max values for rows and cols, and create matrix for pixels
	findMaxes(segments, size, &maxRow, &maxCol);
	res = (grayImage*)malloc(sizeof(grayImage));
	res->pixels = createMatrix(maxRow, maxCol);

	//paint the pixels
	for (i = 0; i < size; i++)
	{
		curr = segments[i].head;
		while (curr != NULL)
		{
			res->pixels[curr->position[ROW]][curr->position[COL]] = (i * MAX_VALUE_IN_BYTE / (size - 1));
			curr = curr->next;
		}
	}

	res->cols = maxCol;
	res->rows = maxRow;

	return (res);
}

void findMaxes(imgPosList* segments, unsigned int size, int* maxRow, int* maxCol)
{
	int maxR, maxC;
	unsigned int i;
	imgPosCell* curr;

	maxR = segments[ROW].head->position[ROW];
	maxC = segments[ROW].head->position[COL];

	for (i = 0; i < size; i++)
	{
		curr = segments[i].head;
		while (curr != NULL)
		{
			if (maxR < curr->position[ROW])
			{
				maxR = curr->position[ROW];
			}
			if (maxC < curr->position[COL])
			{
				maxC = curr->position[COL];

			}
			curr = curr->next;
		}
	}

	*maxRow = maxR + 1;
	*maxCol = maxC + 1;
}

void saveCompressed(char *file_name, grayImage *image, unsigned char max_gray_level)
{
	FILE* f;
	int i, j, counter = 0, rest = 0, numOfBits;
	unsigned short rows, cols;
	unsigned char temp, writer = 0;

	rows = image->rows;
	cols = image->cols;
	numOfBits = howManyBits((int)max_gray_level);

	f = fopen(file_name, "wb");
	checkMemoryAllocation(f);

	//write values to bin file
	fwrite(&rows, sizeof(unsigned short), 1, f);
	fwrite(&cols, sizeof(unsigned short), 1, f);

	fwrite(&max_gray_level, sizeof(unsigned char), 1, f);
	//compress and write values to bin file
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			temp = (image->pixels[i][j] * max_gray_level) / 255;
			if (8 - rest - (counter + 1)*numOfBits > 0)
			{
				temp <<= 8 - rest - (counter + 1)*numOfBits;
				if (i + 1 >= rows&&j + 1 >= cols)
				{
					printf("%hhu ", temp);
					fwrite(&temp, sizeof(unsigned char), 1, f);
				}
				writer |= temp;
				counter++;
			}
			else if (8 - rest - (counter + 1)*numOfBits == 0)
			{
				writer |= temp;
				printf("%hhu ", writer);
				fwrite(&writer, sizeof(unsigned char), 1, f);
				counter = 0;
				rest = 0;
			}
			else
			{
				temp >>= numOfBits - (8 - rest - (counter*numOfBits));
				writer |= temp;
				printf("%hhu ", writer);
				fwrite(&writer, sizeof(unsigned char), 1, f);
				temp = (image->pixels[i][j] * max_gray_level) / 255;
				rest = -1 * (8 - rest - (counter + 1)*numOfBits);
				temp <<= 8 - rest;
				writer = temp;
				counter = 0;
			}
		}
	}
	fclose(f);
}

void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name)
{
	FILE* fRead, *fWrite;
	unsigned short rows, cols;
	unsigned char compressedMaxGrayLevel, maxGrayLevel, tempByte, currentPixel, helper, mask = MAX_VALUE_IN_BYTE;
	int numOfBits, leftoverBits = BITS_IN_BYTE, counter = 0, countInByte = 0;

	fRead = fopen(compressed_file_name, "rb");
	checkMemoryAllocation(fRead);
	fWrite = fopen(pgm_file_name, "w+");
	checkMemoryAllocation(fWrite);

	fread(&cols, sizeof(unsigned short), 1, fRead);
	fread(&rows, sizeof(unsigned short), 1, fRead);
	fread(&compressedMaxGrayLevel, sizeof(unsigned char), 1, fRead);
	maxGrayLevel = compressedMaxGrayLevel << 1;
	numOfBits = howManyBits((int)compressedMaxGrayLevel);
	mask <<= BITS_IN_BYTE - numOfBits;
	fprintf(fWrite, "P2\n");
	printf("P2\n");
	fprintf(fWrite, "%hu %hu\n", cols, rows);
	printf("%hu %hu\n", rows, cols);
	fprintf(fWrite, "%hhu\n", maxGrayLevel);
	printf("%hhu\n", maxGrayLevel);

	fread(&tempByte, sizeof(unsigned char), 1, fRead);
	while (!feof(fRead))
	{
		if (leftoverBits >= numOfBits)
		{
			currentPixel = tempByte;
			currentPixel <<= BITS_IN_BYTE - leftoverBits;
			currentPixel &= mask;
			counter++;
			countInByte++;
			leftoverBits -= numOfBits;
			fprintf(fWrite, "%hhu ", currentPixel);
			printf("%hhu ", currentPixel);
			if (counter == cols)
			{
				counter = 0;
				printf("\n");
				fprintf(fWrite, "\n");
			}
			if (leftoverBits == 0)
			{
				fread(&tempByte, sizeof(unsigned char), 1, fRead);
				countInByte = 0;
			}
		}
		else
		{
			currentPixel = tempByte;
			currentPixel <<= BITS_IN_BYTE - leftoverBits;
			fread(&tempByte, sizeof(unsigned char), 1, fRead);
			countInByte = 0;
			helper = tempByte >> leftoverBits;
			leftoverBits = BITS_IN_BYTE - (numOfBits - leftoverBits);
			currentPixel |= helper;
			currentPixel &= mask;
			counter++;
			countInByte++;
			fprintf(fWrite, "%hhu ", currentPixel);
			printf("%hhu ", currentPixel);
			if (counter == cols)
			{
				counter = 0;
				printf("\n");
				fprintf(fWrite, "\n");
			}
		}
	}
	fclose(fWrite);
	fclose(fRead);
}

int howManyDigits(int num)
{
	int res = 0;
	while (num > 0)
	{
		res++;
		num /= 10;
	}
	return res;
}

int howManyBits(int num)
{
	int res = 0;
	while (num > 0)
	{
		res++;
		num /= 2;
	}
	return res;
}

unsigned char** createMatrix(int maxRow, int maxCol)
{
	unsigned char** res;
	int i;

	res = (unsigned char**)malloc(maxRow * sizeof(unsigned char*));
	checkMemoryAllocation(res);

	for (i = 0; i < maxRow; i++)
	{
		res[i] = (unsigned char*)malloc(maxCol * sizeof(unsigned char));
		checkMemoryAllocation(res[i]);
	}

	return (res);
}

void freeList(imgPosList* lst)
{
	imgPosCell* curr = lst->head;
	imgPosCell* temp;
	while (curr != NULL)
	{
		temp = curr;
		curr = temp->next;
		free(temp);
	}
	lst->head = NULL;
	lst->tail = NULL;
}

void freeTree(treeNode* node)
{
	if (node->next_possible_positions == NULL)
	{
		free(node);
		return;
	}
	else
	{
		freeTree(node->next_possible_positions->node);
		free(node->next_possible_positions);
		free(node);
	}
}


void checkMemoryAllocation(void* ptr)
{
	if (ptr == NULL)
	{
		printf("Failed to open requested file.\n");
		main();
	}
}