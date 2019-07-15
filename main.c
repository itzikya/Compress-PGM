#include "func.h"

void main()
{
	int option, segmentCounter = 0;
	char fileName[MAX_LEN_OF_TXT_NAME], compressedFileName[MAX_LEN_OF_TXT_NAME];
	unsigned char threshold, maxGray;
	bool printMenu = true;
	imgPosList *segmentList = NULL;
	grayImage *img = NULL, *colouredImg = NULL;

	printf("Please choose option from the menu:\n1. Read an image file in PGM format\n2. Find all segments\n3. Color the segments\n4. Save the colored segment in a compressed format\n5. Compress and save the original image in a compressed format\n6. Convert a compressed image to PGM format\n7. exit\n");
	scanf("%d", &option);

	if (!(option <= 7) || !(option >= 1))
	{
		printMenu = false;
		printf("Illegal option.\n");
		printf("Press 1 to return to main menu, any other button to exit:\n");
		scanf("%d", &option);
		if (option != 1)
		{
			printMenu = false;
			exit(1);
		}
	}
	while (printMenu == true)
	{
		switch (option)
		{
		case 1:
			printf("Please enter a PGM file name (e.g.: ______.txt):\n");
			scanf("%s", &fileName);
			img = readPGM(fileName);
			break;
		case 2:
			if (img == NULL)
			{
				printf("Please insert an image file name:\n");
				scanf("%s", &fileName);
				img = readPGM(fileName);
			}
			printf("Insert threshold value:\n");
			scanf("%hhu", &threshold);
			segmentCounter = findAllSegments(img, threshold, &segmentList);
			break;

		case 3:
			if (segmentList == NULL)
			{
				if (img == NULL)
				{
					printf("Please insert file name:\n");
					scanf("%s", &fileName);
					img = readPGM(fileName);
				}
				printf("Insert threshold value:\n");
				scanf("%hhu", &threshold);
				segmentCounter = findAllSegments(img, threshold, &segmentList);
			}
			colouredImg = colorSegments(segmentList, segmentCounter);

			for (int i = 0; i < segmentCounter; i++)
				freeList(&segmentList[i]);

			break;
		case 4:
			if (colouredImg == NULL)
			{
				if (segmentList == NULL)
				{
					if (img == NULL)
					{
						printf("Please insert an image file name:\n");
						scanf("%s", &fileName);
						img = readPGM(fileName);
					}
					printf("Insert threshold value:\n");
					scanf("%hhu", &threshold);
					segmentCounter = findAllSegments(img, threshold, &segmentList);
				}
				colouredImg = colorSegments(segmentList, segmentCounter);

				for (int i = 0; i < segmentCounter; i++)
					freeList(&segmentList[i]);

			}
			printf("Please insert a file name for compressed image (e.g.: ______.bin):\n");
			scanf("%s", &compressedFileName);
			printf("Insert max gray level:\n");
			scanf("%hhu", &maxGray);
			saveCompressed(compressedFileName, colouredImg, maxGray);
			break;

		case 5:
			printf("Please insert an image file name\n");
			scanf("%s", &fileName);
			img = readPGM(fileName);
			printf("Insert max gray level:\n");
			scanf("%hhu", &maxGray);
			printf("Please insert a file name for compressed image (e.g.: ______.bin):\n");
			scanf("%s", &compressedFileName);
			saveCompressed(compressedFileName, img, maxGray);
			break;

		case 6:
			printf("Please insert a compressed image file name (e.g.: ______.bin):\n");
			scanf("%s", &compressedFileName);
			printf("Please insert a requested PGM file name (e.g.: ______.txt):\n");
			scanf("%s", &fileName);
			convertCompressedImageToPGM(compressedFileName, fileName);
			break;

		case 7:
			exit(1);
			break;
		}
		printf("Press 1 to return to main menu, any other button to exit:");
		scanf("%d", &option);
		if (option != 1)
		{
			printMenu = false;
			exit(1);
		}
		printf("Please choose option from the menu:\n1. Read an image file in PGM format\n2. Find all segments\n3. Color the segments\n4. Save the colored image in a compressed format\n5. Compress and save the original image in a compressed format\n6. Convert a compressed image to PGM format\n7. exit\n");
		scanf("%d", &option);
	}
}