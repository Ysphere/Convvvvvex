/*
Name: Yuhang Xu
Prof: FangJu Wang
Date: Feb.13.2017
CIS*3490
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>

#define MAX_SIZE 30005
#define BOOL short
#define TRUE 1
#define FALSE 0

typedef struct PPoint 
{
	float x;
	float y;
} Point;

typedef struct LList
{
	Point points[MAX_SIZE];
	int length;
} List;

// distance beween two points
float dist(Point p1, Point p2)
{
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// initialize the list
void initList(List * list)
{
	list->length = 0;
}

// add a point to the list
BOOL addPoint(List * list, Point p)
{
	BOOL isExist = FALSE;
	for (int i = 0; i < list->length; ++i)
	{
		if (dist(p, list->points[i]) < 1e-3) // this point is already in the list
		{
			isExist = TRUE;
			break;
		}
	}
	if (!isExist)
	{
		list->points[list->length] = p;
		list->length++;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// display the points in the list
void displayList(List * list)
{
	for (int i = 0; i < list->length; i++) 
	{
		printf("(%f, %f)\n", list->points[i].x, list->points[i].y);
	}
}

void bruteForce(Point points[], int amount)
{
	List * list = (List*)malloc(sizeof(List));
	initList(list);
	int i, j, k;
	//parameter of the line
	float a, b, c;
	BOOL flag, flag1 = TRUE, flag2 = TRUE;
	for (i = 0; i<amount - 1; i++)
	{
		for (j = i + 1; j<amount; j++)
		{
			a = points[j].y - points[i].y;
			b = points[i].x - points[j].x;
			c = points[i].x * points[j].y - points[i].y * points[j].x;
			//check if other points are on the same side of the line, if not then flag2 is flase
			for (k = 0; k<amount; k++)
			{
				if ((k == i) || (k == j))
					continue;
				if (flag1)
				{
					if (a*points[k].x + b*points[k].y >= c)
					{
						flag = TRUE;
					}
					else
					{
						flag = FALSE;
					}
					flag1 = FALSE;
					continue;
				}
				if (flag == TRUE)
				{
					if (!(a*points[k].x + b*points[k].y >= c))
					{
						flag2 = FALSE;
						break;
					}
				}
				else
				{
					if (!(a*points[k].x + b*points[k].y < c))
					{
						flag2 = FALSE;
						break;
					}
				}
			}

			if (flag2)
			{
				if (addPoint(list, points[i]))
				{
					printf("(%f, %f)\n", points[i].x, points[i].y);
				}
				if (addPoint(list, points[j]))
				{
					printf("(%f, %f)\n", points[j].x, points[j].y);
				}
			}
			flag1 = TRUE, flag2 = TRUE;
		}
	}

	free(list);
	list = NULL;
}


/**
* list: store the nodes on convex hull
* start: starting index of the search
* end: ending index of the search
* isUP: if need to search the points on the up side of the edge
*/
void fun(Point points[], List *list, int start, int end, BOOL isUp) {
	if (start >= end) {
		return;
	}
	float max = 0, min = 0;    //the max and min distance to the edge
	int maxTag = -1, minTag = -1;
	float result;
	for (int i = start + 1; i < end; i++) {
		// calculate the distance between current point to the edge{start, end}
		result
			= points[start].x * points[end].y
			+ points[i].x * points[start].y
			+ points[end].x * points[i].y
			- points[i].x * points[end].y
			- points[end].x * points[start].y
			- points[start].x * points[i].y;
		if (result < min && !isUp) {
			min = result;
			minTag = i;
		}
		if (result > max && isUp) {
			max = result;
			maxTag = i;
		}
	}
	if (minTag > 0 && !isUp) {
		addPoint(list, points[minTag]);
		// divide, and search for each sub-set
		fun(points, list, start, minTag, isUp);
		fun(points, list, minTag, end, isUp);
	}

	if (maxTag > 0 && isUp) {
		addPoint(list, points[maxTag]);
		fun(points, list, start, maxTag, isUp);
		fun(points, list, maxTag, end, isUp);
	}
}

//quick sort for the points
void quickSort(Point points[], int start, int end) {
	if (start >= end) {
		return;
	}
	int lo = start;
	int hi = end;
	Point key = points[lo];
	while (lo < hi) {
		while (lo < hi && points[hi].x >= key.x) {
			hi--;
		}
		points[lo] = points[hi];
		while (lo < hi && points[lo].x <= key.x) {
			lo++;
		}
		points[hi] = points[lo];
	}
	points[lo] = key;
	quickSort(points, start, lo - 1);
	quickSort(points, lo + 1, end);
}

void divideAndConquer(Point points[], int amount)
{
	List * list = (List*)malloc(sizeof(List));
	initList(list);
	quickSort(points, 0, amount - 1);
	if (amount > 0) {
		//add the first point
		addPoint(list, points[0]);
	}
	//divide the point set, then search the upper sub-set and lower sub-set
	fun(points, list, 0, amount - 1, TRUE);
	fun(points, list, 0, amount - 1, FALSE);
	if (amount > 1) {
		//add the last point
		addPoint(list, points[amount - 1]);
	}
	displayList(list);

	free(list);
	list = NULL;
}

int main()
{
	Point points[MAX_SIZE];
	int amount = 0;
	struct timeb t1, t2;
	long t;

	FILE * fp = fopen("data2.txt", "r+");
	while (fscanf(fp, "%f %f", &(points[amount].x), &(points[amount].y)) != EOF)
	{
		amount++;
	}
	fclose(fp);

	printf("Divide and conquer\n");
	ftime(&t1); /*starting time*/
	divideAndConquer(points, amount);
	ftime(&t2); /* finish time*/
	t = (t2.time - t1.time) * 1000 + (t2.millitm - t1.millitm);
	printf("Divide and conquer, total time used  %ld msec\n",  t);

	printf("\nBrute force\n");
	ftime(&t1); /*starting time*/
	bruteForce(points, amount);
	ftime(&t2); /* finish time*/
	t = (t2.time - t1.time) * 1000 + (t2.millitm - t1.millitm);
	printf("Brute Force, total time used  %ld msec\n", t);
}