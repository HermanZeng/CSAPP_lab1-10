/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "5140219405",
    /* First member's full name */
    "zengheming",
    /* First member's email address */
    "zhming2014@sjtu.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


static char *heap_listp;

/* Basic constants and macros */
#define WSIZE 4 /* word size (bytes) */
#define DSIZE 8 /* double word size (bytes) */
#define CHUNKSIZE ( 1 << 12 ) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
//#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))
//#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))
//#define GET(p) (*(unsigned int *)(p))
//#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
//#define GET_SIZE(p) (GET(p) & ?0x7)
//#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
//#define HDRP(bp) ((char *)(bp) - WSIZE)
//#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define PREV_ALLOC(bp) (GET(bp) & 0x2)
#define lc(bp) ((char *) (bp))
#define rc(bp) ((char *) (bp) + WSIZE)
#define LC(bp) (GET(lc(bp)))
#define RC(bp) (GET(rc(bp)))
#define alter1(bp) ( (char *)bp - Tnode)
#define alter2(bp) ((Tnode + bp))
#define check(bp, temp) (GET_SIZE(HDRP(bp)) < GET_SIZE(HDRP(temp))) || ((GET_SIZE(HDRP(bp)) == GET_SIZE(HDRP(temp))) && alter1(bp) < alter1(temp))


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void *find_min_block();
static void insert_min(void *bp);
static void place(void *bp, size_t asize);
static void move_list(void *bp);
static char *listp4;
static char *listp2;
static unsigned int root;
static char *Tnode;
static char *Last = NULL;
static int judge = 0;
static void insnode(void *bp);
static void delnode(void *bp);
static void rotate(void *bp);


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	//printf("initial\n");
    if ((void*)(Tnode = mem_sbrk(4 * WSIZE)) < (void *)0)
		return -1;
	Tnode += WSIZE;
	PUT(HDRP(Tnode), PACK(4 * WSIZE, 1));
	PUT(FTRP(Tnode), PACK(4 * WSIZE, 1));

	if ((void *)(listp2 = mem_sbrk(2 * WSIZE)) < (void *)0) return -1;
	PUT(listp2, 0);
	PUT(listp2 + WSIZE, 0);
	if ((void *)(listp4 = mem_sbrk(2 * WSIZE)) < (void *)0) return -1;
	PUT(listp4, 0);
	PUT(listp4 + WSIZE, 0);
	if ((void *)(heap_listp = mem_sbrk(4 * WSIZE)) < (void *)0) return -1;
	root = NULL;
	Last = NULL;
	judge = 0;
	/* create the initial empty heap */
    /*if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *) -1)
        return -1;*/
    PUT(heap_listp, 0); 			         /* alignment padding */
    PUT(heap_listp+(1*WSIZE), PACK(DSIZE, 1));  /* prologue header */
    PUT(heap_listp+(2*WSIZE), PACK(DSIZE, 1));  /* prologue footer */
    PUT(heap_listp+(3*WSIZE), PACK(0, 1)); 	         /* epilogue header */
    heap_listp += (2*WSIZE);

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    /*if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;*/
    return 0;

}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    /*int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }*/
    //printf("malloc\n");
    size_t asize; /* adjusted block size */
 	size_t extendsize; /* amount to extend heap if no fit */
 	char *bp;
     
 	/* Ignore spurious requests */
	if (size <= 0)
 		return NULL;

 	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= WSIZE)
 		asize = 2 * DSIZE;
 	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
 	/* Search the free list for a fit */
 	if ((bp = find_fit(asize)) != NULL) {
 		place (bp, asize);
 		return bp;
     }

	if (judge == 0) extendsize = max(asize, CHUNKSIZE);
	else extendsize = asize - GET_SIZE(Last);
	if (judge == 1 && extendsize == 0)
	{
		judge = 0;
		bp = Last;
		place(bp, asize);
	}
	else
	{
		if ((bp = extend_heap(extendsize / WSIZE)) == NULL) return NULL;
		place(bp, asize);
	}

 	/* No fit found. Get more memory and place the block */
	/*extendsize = MAX (asize, CHUNKSIZE) ;
	if ((bp = extend_heap (extendsize/WSIZE)) == NULL)
    	return NULL;
    place (bp, asize);*/
 	return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
 void mm_free(void *bp)
 {
	 //printf("free\n");
     if (bp == NULL) return;
	 size_t size=GET_SIZE(HDRP(bp));
     PUT(HDRP(bp), PACK(size, PREV_ALLOC(HDRP(bp))));
	 PUT(FTRP(bp), PACK(size, PREV_ALLOC(HDRP(bp))));
     coalesce(bp);
  }
/*{
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    //printf("realloc\n");
    /*void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    void *bp = ptr;
    copySize = GET_SIZE(HDRP(bp));
    /*PUT(HDRP(bp), PACK(csize, 0));
    PUT(FTRP(bp), PACK(csize, 0));
    newptr = coalesce(bp);
    if (GET_SIZE(newptr) < size)
        newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    //copySize = *(size_t *)((char *)oldptr - WSIZE) & ~0x7;//*(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;*/
    
    void *oldptr = ptr;
    void *newptr;
    size_t copysize;
    void *bp = ptr;
    size_t asize; /* adjusted block size */
 	/* Ignore spurious requests */
	if (size <= 0)
 		return NULL;

 	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= WSIZE)
 		asize = 2 * DSIZE;
 	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    copysize = GET_SIZE(HDRP(bp));
    if (asize == copysize) return oldptr;
    if (asize < copysize)
    {
        //printf("hit\n");
        if ((copysize - asize) < 2 * DSIZE) return oldptr;
        else
        {
            size_t real_size = copysize - asize;
            size_t allocate = GET(HDRP(oldptr)) & 0x7;
            PUT(HDRP(oldptr), PACK(asize, allocate));
            PUT(FTRP(oldptr), PACK(asize, 0x1));
            PUT(HDRP(NEXT_BLKP(oldptr)), PACK(real_size, 0x2));
            PUT(FTRP(NEXT_BLKP(oldptr)), PACK(real_size, 0x0));
            insnode(NEXT_BLKP(oldptr));
            return oldptr;
        }
    }
    if (asize > copysize)
    {
        newptr = mm_malloc(asize);
        //place(newptr, asize);
        memcpy(newptr, oldptr, copysize);
        mm_free(oldptr);
        return newptr;
    }
        /*newptr = find_fit(asize);
        delnode(newptr);
        size_t real_size = GET_SIZE(HDRP(newptr)) - copysize;
        if (GET_SIZE(HDRP(newptr)) - copysize > 2 * DSIZE)
        {
            memcpy(newptr, oldptr, copysize);
            PUT(HDRP(newptr), PACK(copysize, 0x3));
            PUT(FTRP(newptr), PACK(GET_SIZE(HDRP(newptr)) - copysize, 0x1));
            PUT(HDRP(NEXT_BLKP(oldptr)), PACK(real_size, 0x2));
            PUT(FTRP(NEXT_BLKP(oldptr)), PACK(real_size, 0x0));
        }
        else
        {
            memcpy(newptr, oldptr, copysize);
            PUT(HDRP(newptr), PACK(copysize, 0x3));
            PUT(FTRP(newptr), PACK(copysize, 0x1));
        }
        
        return newptr;
    }*/
}


static void *extend_heap(size_t words)
{
	//printf("extend\n");
    char *bp;
	size_t size;

	/* Allocate an even number of words to maintain alignment */
	size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
	if ((long)(bp = mem_sbrk(size))  == -1)
    	return NULL;

	if (judge == 0)
	{
		PUT(HDRP(bp), PACK(size, 2));
		PUT(FTRP(bp), PACK(size, 2));
	}
	else
	{
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 2));
	}
	
	Last = FTRP(bp);
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
	return coalesce(bp);

	/* Initialize free block header/footer and the epilogue header */
	//PUT(HDRP(bp), PACK(size, 0)); 		/* free block header */
	//PUT(FTRP(bp), PACK(size, 0)); 		/* free block footer */
	//PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

	/* Coalesce if the previous block was free */
	//return coalesce(bp);
}


static void *coalesce(void *bp)
{
	//printf("coalesce\n");
    size_t prev_alloc = PREV_ALLOC(HDRP(bp));
	//size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
 	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
 	size_t size = GET_SIZE(HDRP(bp));

	if (prev_alloc && next_alloc) { /* Case 1 */
		insnode(bp);
		PUT(HDRP(NEXT_BLKP(bp)), (GET(HDRP(NEXT_BLKP(bp))) & (~0x2)));
		return bp;
 	}
	else if (prev_alloc && !next_alloc) { /* Case 2 */
 	          size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
			  delnode(NEXT_BLKP(bp));
 	          PUT(HDRP(bp), PACK(size, PREV_ALLOC(HDRP(bp))));
			  PUT(FTRP(bp), PACK(size, PREV_ALLOC(HDRP(bp))));
 	          //return(bp);
    }
    else if (!prev_alloc && next_alloc) { /* Case 3 */
 	          size += GET_SIZE(HDRP(PREV_BLKP(bp)));
			  delnode(PREV_BLKP(bp));
			  PUT(FTRP(bp), PACK(size, PREV_ALLOC(HDRP(PREV_BLKP(bp)))));
			  PUT(HDRP(PREV_BLKP(bp)), PACK(size, PREV_ALLOC(HDRP(PREV_BLKP(bp)))));
			  bp = PREV_BLKP(bp);
 			  //return(PREV_BLKP(bp));
 	}

 	else { /* Case 4 */
 			size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
			delnode(PREV_BLKP(bp));
			delnode(NEXT_BLKP(bp));
			PUT(HDRP(PREV_BLKP(bp)), PACK(size, PREV_ALLOC(HDRP(PREV_BLKP(bp)))));
			PUT(FTRP(NEXT_BLKP(bp)), PACK(size, PREV_ALLOC(HDRP(PREV_BLKP(bp)))));
			bp = PREV_BLKP(bp);
 			//return(PREV_BLKP(bp));
 		}
	insnode(bp);
	PUT(HDRP(NEXT_BLKP(bp)), (GET(HDRP(NEXT_BLKP(bp))) & (~0x2)));
	return bp;
}

static void *find_fit(size_t asize)
{
     //printf("find_fit\n");
     void *bp = NULL;
 	
	 if (asize == DSIZE)
	 {
		 unsigned int temp = RC(listp2);
		 if (temp != 0) return alter2(temp);
	 }
	 if (asize < 4 * WSIZE)
	 {
		 bp = find_min_block();
		 if (bp != NULL) return bp;
	 }
	 unsigned int tp = root;
	 while (tp != NULL)
	 {
		 void* t = alter2(tp);
		 if (GET_SIZE(HDRP(t)) >= asize)
		 {
			 bp = t;
			 tp = LC(t);
		 }
		 else tp = RC(t);
	 }
	 if (bp != NULL) rotate(bp);
	 return bp;
	 /* first fit search */
     /*for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0 ; bp = NEXT_BLKP(bp) ) {
 	       if (!GET_ALLOC(HDRP(bp)) && (asize<=GET_SIZE(HDRP(bp)))) {
   	            return bp;
 	        }
      }
       return NULL;*/  /*no fit */
 } 

static void place(void *bp, size_t asize)
{
    //printf("place\n");
    size_t csize = GET_SIZE(HDRP(bp));
	delnode(bp);
    if((csize - asize) >= DSIZE){
        PUT(HDRP(bp), PACK(asize, 1 + PREV_ALLOC(HDRP(bp))));
		PUT(FTRP(bp), PACK(asize, 1 + PREV_ALLOC(HDRP(bp))));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 2));
        PUT(FTRP(bp), PACK(csize - asize, 2));
		insnode(bp);
    }
    else{
		PUT(HDRP(bp), PACK(csize, 1 + PREV_ALLOC(HDRP(bp))));
		PUT(FTRP(bp), PACK(csize, 1 + PREV_ALLOC(HDRP(bp))));
		PUT(HDRP(NEXT_BLKP(bp)), (GET(HDRP(NEXT_BLKP(bp))) | (0x2)));
    }
  	/*if ( (csize �Casize) >= (DSIZE + DSIZE) ) {
 		PUT(HDRP(bp), PACK(asize, 1)) ;
 		PUT(FTRP(bp), PACK(asize, 1)) ;
 		bp = NEXT_BLKP(bp) ;
 		PUT(HDRP(bp), PACK(csize-asize, 0) ;
 		PUT(FTRP(bp), PACK(csize-asize, 0) ;
 	} else {
 		PUT(HDRP(bp), PACK(csize, 1) ;
 		PUT(FTRP(bp), PACK(csize, 1) ;
 	}*/
}


static void rotate(void *bp)
{
	//printf("rotate\n");
    if (root == NULL) return;
	char *left = NULL;
	char *right = NULL;
	char *yr = NULL;
	char *node = Tnode;
	PUT(lc(node), NULL);
	PUT(rc(node), NULL);
	left = right = node;
	void *temp = alter2(root);
	while (1)
	{
		if (alter1(bp) == alter1(temp)) break;
		if (check(bp, temp))
		{
			if (LC(temp) != NULL && (check(bp, alter2(LC(temp)))))
			{
				yr = alter2(LC(temp));
				PUT(lc(temp), RC(yr));
				PUT(rc(yr), alter1(temp));
				temp = yr;
			}
			if (LC(temp) == NULL) break;
			PUT(lc(right), alter1(temp));
			right = temp;
			temp = alter2(LC(temp));
		}
		else
		{
			if (RC(temp) != NULL && (check(alter2(RC(temp)), bp)))
			{
				yr = alter2(RC(temp));
				PUT(rc(temp), LC(yr));
				PUT(lc(yr), alter1(temp));
				temp = yr;
			}
			if (RC(temp) == NULL) break;
			PUT(rc(left), alter1(temp));
			left = temp;
			temp = alter2(RC(temp));
		}
	}
	PUT(rc(left), LC(temp));
	PUT(lc(right), RC(temp));
	PUT(lc(temp), RC(node));
	PUT(rc(temp), LC(node));
	root = alter1(temp);
	return;
}

static void *find_min_block()
{
	//printf("find_min\n");
    unsigned int temp = LC(listp4);
	if (temp == 0) return NULL;
	return alter2(temp);
}

static void insert_min(void *bp)
{
	//printf("insert_min\n");
    unsigned int temp = GET(rc(listp4));
	PUT(listp4, alter1(bp));
	PUT(lc(bp), alter1(listp4));
	PUT(rc(bp), temp);
	if (temp != 0) PUT(lc(alter2(temp)), alter1(bp));
	else PUT(lc(listp4), alter1(bp));
}

static void insnode(void *bp)
{
	//printf("insnode\n");
    if (FTRP(bp) == Last) judge = 1;
	if (GET_SIZE(HDRP(bp)) <= DSIZE)
	{
		PUT(rc(listp2), alter1(bp));
		return;
	}
	if (GET_SIZE(HDRP(bp)) == 2 * DSIZE)
	{
		insert_min(bp);
		return;
	}
	PUT(lc(bp), NULL);
	PUT(rc(bp), NULL);
	if (root == NULL)
	{
		root = alter1(bp);
		return;
	}
	unsigned int temp = root;
	void *last = NULL;
	while (temp != NULL)
	{
		last = alter2(temp);
		if (check(bp, last)) temp = LC(last);
		else temp = RC(last);
	}
    //printf("dead\n");
	if (check(bp, last)) PUT(lc(last), alter1(bp));
	else PUT(rc(last), alter1(bp));
	rotate(bp);
	return;
}

static void move_list(void *bp)
{
	//printf("move_list\n");
    unsigned int pre = GET(lc(bp));
	unsigned int suc = GET(rc(bp));
	if (pre != 0) PUT(rc(alter2(pre)), suc);
	if (suc != 0) PUT(lc(alter2(suc)), pre);
	else PUT(lc(listp4), suc);
}


static void delnode(void *bp)
{
	//printf("delnode\n");
    if (FTRP(bp) == Last) judge = 0;
	if (GET_SIZE(HDRP(bp)) <= DSIZE)
	{
		PUT(rc(listp2), 0);
		return;
	}
	if (GET_SIZE(HDRP(bp)) <= 2 * DSIZE)
	{
		move_list(bp);
		return;
	}
	rotate(bp);
	unsigned int T = LC(bp);
	void *last = NULL;
	if (T == NULL) root = RC(bp);
	else
	{
		while (T != NULL)
		{
			last = alter2(T);
			T = RC(last);
		}
		void *temp = alter2(root);
		root = LC(temp);
		rotate(last);
		PUT(rc(last), RC(temp));
	}
}
