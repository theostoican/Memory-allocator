//Alocator de memorie - Theodor Stoican, 313CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tgmath.h>

typedef unsigned char arena_t;
arena_t *p;
int N;

void INITIALIZE ()
{
	int i;
	p = (arena_t*)calloc (N, sizeof (arena_t));
	for (i = 0; i <= 3 ;i++)
	{
		*(p+i) = 0;
	}
}
void FINALIZE ()
{
	free (p);
}

void DUMP()
{
	int counter = 0;
	while (counter <= N-1)
	{
		if (counter % 16 == 0)
		{	
			if(counter ==0) printf ("%08X\t", counter);
				else printf ("\n%08X\t", counter);
		}
		if (counter % 8 == 0 && counter && counter %16 !=0) printf (" ");
		printf ("%02X ", *(p + counter));
		counter++;
	}
	printf("\n%08X \n", counter );
}

int ALLOC (int SIZE)
{
	int *q, *nextblock, *change;
	int rneighbor, jump, lneighbor, current, DIM, back;

	q = (int*)p;
	rneighbor = *q;
	//abordez cazurile in care exista blocuri in arena
	//initial studiez cazul in care exista un singur bloc
	if (rneighbor - 4 >= SIZE + 12)
	{
		change = (int*)(p + rneighbor);
		*(change + 1) = 4;
		*(q + 1) = rneighbor;
		*q = 4;
		*(q + 2) = 0;
		*(q + 3) = SIZE + 12;
		return 16;
	}
	//studiez cazul in care exista mai multe blocuri
	if (rneighbor)
	{
		while (rneighbor)
		{
			q = (int*)(p + rneighbor);
			DIM = rneighbor + *(q+2);
			rneighbor = *q;
			if (rneighbor - DIM >= SIZE + 12)
			{
				*q = DIM;
				back = *(q + 1);
				q = (int*)(p + back);
				back = *q;
				q = (int*)(p + DIM);
				*q = rneighbor;
				*(q + 1) = back;
				*(q + 2) = 12 + SIZE;
				q = (int*)(p + rneighbor);
				*(q + 1) = DIM;
				return DIM + 12;
			}
		}
		jump = *(q + 2);
		lneighbor = *(q + 1);
		q = (int*)(p + lneighbor);
		current = *q;
		if (current + jump + 12 + SIZE <= N)
		{
			nextblock = (int*)(p + current + jump) ;
			*(nextblock) = 0;
			*(nextblock + 2) = 12 + SIZE;
			*(nextblock + 1) = current;
			q = (int*)(p + current);
			*q = current + jump;
			return current + jump + 12;
		}
		else return 0;
	}
	//cazul in care arena este goala
	else if (4 + SIZE + 12 <= N)
	{
		*q = 4;
		*(q + 1) = 0;
		*(q + 2) = 0;
		*(q + 3) = 12 + SIZE;
		return 16;
	}
	else return 0;
}
void FREE (int INDEX)
{
	int next, previous;
	int *q, *movef, *moveb;
	q = (int*)(p + INDEX - 12);
	previous = *(q + 1);
	next = *q;
	if (next)
	{
		movef = (int*)(p+next);
		*(movef + 1) = previous;
	}
	moveb = (int*)(p + previous);
	*(moveb) = next;
}
void FILL (int INDEX, int SIZE, int VALUE)
{
	int i;
	for (i=INDEX; i <= INDEX + SIZE -1; i++)
		*(p + i) = VALUE;
}

//Functiile de tipul "SHOW" respecta toate structura functiei ALLOC.
//Se studiaza mai intai cazurile in care arena are un bloc sau mai multe 
//blocuri, iar apoi cand nu exista blocuri

void SHOW_FREE ()
{
	int *q;
	int reg = 0, byte=0, next, current, dim, rest, interbyte;
	q = (int*)p;
	next = *q;
	if (next == 0)
	{
		byte += N-4;
		reg++;
	}
	else
	{
		byte += next - 4;
		if (byte) reg++;
		current = next;
		q = (int*)(p + current);
		next = *q;
		while (next)
		{
			dim = *(q+2);
			interbyte = next - current - dim;
			if (interbyte)
			{
				reg++;
				byte += next - current - dim;
			}
			current = next;
			q = (int*)(p + current);
			next = *q;
		}
		rest = *(q + 2);
		interbyte = N - current - rest;
		if (interbyte)
		{
			byte += N - current - rest;
			reg++;
		}
	}
	printf ("%d blocks ", reg);
	printf("(%d bytes) free", byte);
}
void SHOW_USAGE ()
{
	int *q;
	int used = 0, block, noofblocks = 0, findlast;
	float eff, fragm = 0;
//noofblocks provine de la "number of blocks", unde memorez numarul
//blocurilor alocate

	q = (int*)p;
	block = *q;
	if ((block - 4) > 0) fragm++;
	while (block)
	{	
		noofblocks++;
		q = (int*)(p + block);
	
		if (*q)
			if (*q - *(q + 2) - block != 0) fragm ++;
		used += *(q + 2) - 12;
		block = *q;
	}
	if (noofblocks)
	{ 
		block = *(q + 1); 
		q = (int*)(p + block);
		block = *q;
		q = (int*)(p + block);
		findlast = block + *(q+2);
		if (N - findlast != 0) fragm ++;
		if (fragm)
			fragm = (float)(fragm - 1) / (float)noofblocks;
	}
	eff = (float)(used) / (float)(noofblocks * 12 + used + 4);
	eff *= 100;
	eff = floorf(eff);
	fragm *= 100;
	fragm = floorf(fragm);
	printf ("%d blocks (%d bytes) used\n", noofblocks, used);
	printf ("%0.f%% efficiency\n", eff);
	printf ("%0.f%% fragmentation", fragm);
}
void SHOW_ALLOCATIONS()
{
	int *q;
	int MOVE, DIMFREE, BLENGTH;
	q = (int*)p;
	MOVE = *q;
	if (MOVE)
	{
		printf ("OCCUPIED 4 bytes");
		DIMFREE = MOVE - 4;
		if (DIMFREE)
			printf ("\nFREE %d bytes", DIMFREE);
		while (1)
		{
			q = (int*)(p + MOVE);
			BLENGTH = *(q+2);
			printf ("\nOCCUPIED %d bytes", BLENGTH);
			if (*q)
			{
				if (*q - MOVE - BLENGTH)
					printf ("\nFREE %d bytes", *q - MOVE - BLENGTH);
			}
			else break;
			MOVE = *q;
		}
		if (N - MOVE - BLENGTH)
			printf("\nFREE %d bytes", N - MOVE - BLENGTH);
	}
	else
	{
		printf ("OCCUPIED 4 bytes\n");
		printf ("FREE %d bytes\n", N - 4);
	}
}
int main()
{
	char s[60], *adr, s2[20], s1[20];
	int ok, i, s3[4], isnumber, j;
	
	//folosesc strtok si atoi pentru prelucrarea sirurilor citite si, in 
	//functiede caz, depozitez static in diferiti vectori numerele sau 
	//caracterele care alcatuiesc comenzile
	
	while (1)
	{
		i = 0;
		fgets (s, 60, stdin);
		ok = 0;
		adr = (char*)strtok (s, " ");
		while (adr != NULL)
		{
			isnumber = 1;
			for (j = 0; j < strlen(adr); j++)
			{
				if(adr[j] < 48 || adr[j] > 57)
				{
					isnumber = 0;
					break;
				}
				else
				{
				//pun in vector numerele(cazul comenzii FILL)
					s3[i] = atoi(adr);
					i++;
					break;
				}
			}
			if (isnumber == 0)
			{
			//folosesc doua siruri pentru comenzile de tipul
			//SHOW
				if (ok) 
					strcpy (s2, adr);
				else strcpy (s1, adr);
				ok++;
			}
			adr = strtok (NULL, " ");
		}
		//adr imi va returna intotdeauna un sir cu enter la final in 
		//momentul citirii, asa ca il elimin
		if (s1[strlen(s1)-1]=='\n') s1[strlen(s1)-1] = s1[strlen(s1)];
		if (s2[strlen(s2)-1]=='\n') s2[strlen(s2)-1] = s2[strlen(s2)];
		if (strcmp (s1, "INITIALIZE") == 0) 
				{
					N = s3[0];
					INITIALIZE ();
				}
		if (strcmp (s1, "FINALIZE") == 0) 
		{
			FINALIZE();
			exit(1);
		}
		if (strcmp (s1, "DUMP") == 0) DUMP();
		if (strcmp (s1, "ALLOC") == 0) 
		{
			printf("%d\n",ALLOC (s3[0]));
		}
		if (strcmp (s1, "FREE") == 0) FREE(s3[0]);
		if (strcmp (s1, "FILL") == 0) FILL(s3[0],s3[1],s3[2]);
		if (strcmp (s1, "SHOW") == 0)
		{	
			if (strcmp (s2, "FREE") == 0)
				SHOW_FREE();
			if (strcmp (s2, "USAGE") == 0)
				SHOW_USAGE();
			if (strcmp (s2, "ALLOCATIONS") == 0)
				SHOW_ALLOCATIONS();
			printf("\n");
		}
	}
	return 0;
}
