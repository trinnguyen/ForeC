/*
 *  Program Title: lzss Text File Archiver
 *  Author's Name: Simon Yuan
 *  Student ID: 3415420
 *  Date written: 09/05/2004
 *  Description: This program compresses and extracts text files using
 *               Lempel-Ziv Storer Szymanski (LZSS) algorithm.
 *  Operating condition: This is a Command Line Interface based program, must be
 *                       run under a command shell.
 *  Bugs discovered: None.
 */

#include "stdio.h"

// Hardware counter connected via a direct FSL bus.
#include "counter_dfsl.h"
Counter counter;
#define FSL_COUNTER_ID			2

typedef struct _Fifo {
    char *data;
    unsigned size;
    unsigned written_size;
    int begin;
    int end;
} Fifo;

void Fifo_init(Fifo*, unsigned, char*);
void Fifo_destroy(Fifo*);
int Fifo_push_front(Fifo*, char);
int Fifo_push_back(Fifo*, char);
char Fifo_pop_front(Fifo*);
char Fifo_pop_back(Fifo*);
char Fifo_at(const Fifo *, unsigned);
void Fifo_print(const Fifo *);

int mgetc(char **stream);
int mputc(char c, char **stream);
int mwrite(const void *ptr, int size, int nmemb, char **stream);

//******************************************
//* Start of global variable declarations. *
//******************************************

int FILE_SIZE = 816;
int NUM_THREADS = 4;
int WINLENGTH = 256;
int LKLENGTH = 16;

char archive[816];

char embedded[] = {
    35, 105, 110, 99, 108, 117, 100, 101, 32, 60, 105, 111, 115, 116, 114, 101,
    97, 109, 62, 10, 35, 105, 110, 99, 108, 117, 100, 101, 32, 60, 102, 115,
    116, 114, 101, 97, 109, 62, 10, 10, 117, 115, 105, 110, 103, 32, 110, 97,
    109, 101, 115, 112, 97, 99, 101, 32, 115, 116, 100, 59, 10, 10, 105, 110,
    116, 32, 109, 97, 105, 110, 40, 105, 110, 116, 32, 97, 114, 103, 99, 44,
    32, 99, 104, 97, 114, 32, 42, 97, 114, 103, 118, 91, 93, 41, 10, 123,
    10, 32, 32, 32, 32, 105,  102, 115, 116, 114, 101, 97, 109, 32, 105, 110,
    40, 97, 114, 103, 118, 91, 49, 93, 41, 59, 10, 10, 32, 32, 32, 32,
    105, 102, 32, 40, 33, 105, 110, 41, 10, 32, 32, 32, 32, 123, 10, 32,
    32, 32, 32, 32, 32, 32, 32, 99, 101, 114, 114, 32, 60, 60, 32, 34,
    67, 97, 110, 110, 111, 116, 32, 111, 112, 101, 110, 32, 105, 110, 112, 117,
    116, 32, 102, 105, 108, 101, 58, 32, 34, 32, 60, 60, 32, 97, 114, 103,
    118, 91, 50, 93, 32, 60, 60, 32, 101, 110, 100, 108, 59, 10, 32, 32,
    32, 32, 32, 32, 32, 32, 114, 101, 116, 117, 114, 110, 32, 49, 59, 10,
    32, 32, 32, 32, 125, 10, 10, 32, 32, 32, 32, 111, 102, 115, 116, 114,
    101, 97, 109, 32, 111, 117, 116, 40, 34, 101, 109,  98, 101, 100, 46, 104,
    34, 41, 59, 10, 10, 32, 32, 32, 32, 105, 102, 32, 40, 33, 111, 117,
    116, 41, 10, 32, 32, 32, 32, 123, 10, 32, 32, 32, 32, 32, 32, 32,
    32, 99, 101, 114, 114, 32, 60, 60, 32, 34, 67, 97, 110, 110, 111, 116,
    32, 111, 112, 101, 110, 32, 111, 117, 116, 112, 117, 116, 32, 102, 105, 108,
    101, 58, 32, 101, 109, 98, 101, 100, 46, 104, 34, 32, 60, 60, 32, 101,
    110, 100, 108, 59, 10, 32, 32, 32, 32, 32, 32, 32, 32, 114, 101, 116,
    117, 114, 110, 32, 50, 59, 10, 32, 32, 32, 32, 125, 10, 10, 32, 32,
    32, 32, 108, 111, 110, 103, 32, 98, 101, 103, 105, 110, 44, 32, 101, 110,
    100, 59, 10, 32, 32, 32, 32, 98, 101, 103, 105, 110, 32, 61, 32, 105,
    110, 46, 116, 101, 108, 108, 103, 40, 41, 59, 10, 32, 32, 32, 32, 105,
    110, 46, 115, 101, 101, 107, 103, 40, 48, 44, 32, 105, 111, 115, 58, 58,
    101, 110, 100, 41, 59, 10, 32, 32, 32, 32, 101, 110, 100, 32, 61, 32,
    105, 110, 46, 116, 101, 108, 108, 103, 40, 41, 59, 10, 32, 32, 32, 32,
    105, 110, 46, 115, 101, 101, 107, 103, 40, 48, 44, 32, 105, 111, 115, 58,
    58, 98, 101, 103, 41, 59, 10, 10, 32, 32, 32, 32, 117, 110, 115, 105,
    103, 110, 101, 100, 32, 105, 110, 116, 32, 99, 32, 61, 32, 105, 110, 46,
    103, 101, 116, 40, 41, 59, 10, 32, 32, 32, 32, 111, 117, 116, 32, 60,
    60, 32, 34, 35, 100, 101, 102, 105, 110, 101, 32, 70, 73, 76, 69, 95,
    83, 73, 90, 69, 32, 34, 32, 60, 60, 32, 40, 101, 110, 100, 32, 45,
    32, 98, 101, 103, 105, 110, 41, 32, 60, 60, 32, 34, 92, 110, 92, 110,
    34, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 34, 99, 104,
    97, 114, 32, 97, 114, 99, 104, 105, 118, 101, 91, 34, 32, 60, 60, 32,
    40, 101, 110, 100, 32, 45, 32, 98, 101, 103, 105, 110, 41, 32, 60, 60,
    32, 34, 93, 59, 92, 110, 92, 110, 34, 10, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 34, 99, 104, 97, 114, 32, 101, 109, 98, 101, 100, 100,
    101, 100, 91, 93, 32, 61, 32, 123, 92, 110, 34, 10, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 34, 32, 32, 32, 32, 34, 32, 60, 60,
    32, 99, 59,  10, 10, 32, 32, 32, 32, 119, 104, 105, 108, 101, 32, 40,
    99, 32, 61, 32, 105, 110, 46, 103, 101, 116, 40, 41, 44, 32, 33, 105,
    110, 46, 101, 111, 102, 40, 41, 41, 32, 123, 10, 32, 32, 32, 32, 32,
    32, 32, 32, 111, 117, 116, 32, 60, 60, 32, 34, 44, 92, 110, 34, 10, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 60, 32, 34,
    32, 32, 32, 32, 34, 32, 60, 60, 32, 99, 59, 10, 32, 32, 32, 32,
    125, 10, 32, 32, 32, 32, 10, 32, 32, 32, 32, 111, 117, 116, 32, 60,
    60, 32, 34, 92, 110, 125, 59, 92, 110, 34, 59, 10, 125, 10
};

int offset;
int matches = 0;

int *first(int **copy1, int **copy2) {
	return *copy1;
}

int offset_thread[4];
int matches_thread[4];

struct fileStats {
    char *p_file;
    char *fileaddr;
    int filesize;
    int totalSize;
    int bytesProcessed;
} fileInfo, archiveInfo;

char window_buf[256];
char lookAhead_buf[16];

Fifo window;
Fifo lookAhead;

//*************************************
//* Functions required for archiving. *
//*************************************
int getFileSize(void);
int getBytesProcessed(void);
void openFile(void);
void createDescriptor(struct fileStats*, struct fileStats*);
void storeFileInfo(struct fileStats*, struct fileStats*);
void longestMatch(const Fifo, const Fifo, int, int *, int *);
void writeToArchive(const Fifo *, int*, int*, struct fileStats*, int *, int *, Fifo *, Fifo *);
void printResult(struct fileStats*, struct fileStats*);

//*************************************
//* Functions required for unpacking. *
//*************************************
void unpackArchive(char *filename, int *, int *, struct fileStats*, struct fileStats*);
void processDescriptor(struct fileStats*, struct fileStats*);
void decompressFile(int*, int*, struct fileStats*, struct fileStats*);
void decode(Fifo*, int*, int*, int*, struct fileStats*, struct fileStats*);

//********************************************************
//* Functions required both for archiving and unpacking. *
//********************************************************
void obtainFileInfo(struct fileStats *);

//******************************
//* Start of the lzss program. *
//******************************
int main(int argc, char **argv) {
	// Initialise and start counting the elapsed cycles for each reaction.
	counterInitialise(counter);
	counterStart(counter, FSL_COUNTER_ID);
	
	openFile();

	Fifo_init(&window, WINLENGTH, window_buf);
	Fifo_push_back(&window, mgetc(&fileInfo.p_file)); // No point to search the window when there's nothing.
	writeToArchive(&window, &offset, &matches, &archiveInfo, offset_thread, matches_thread, &window, &lookAhead);  // Same reason here, the first character should always get copied to the archive.
	fileInfo.p_file = fileInfo.fileaddr;
	Fifo_pop_back(&window);
	Fifo_init(&lookAhead, LKLENGTH, lookAhead_buf);
	
	// Fill the look ahead buffer up.
	while (lookAhead.written_size < LKLENGTH) {
		Fifo_push_back(&lookAhead, mgetc(&fileInfo.p_file));
	}
				
	while (getFileSize() != getBytesProcessed()) {
		int count = 1;
		do {
			// This loop will transfer the first node from look ahead buffer to window for 'matches' times.
			// The node from look ahead buffer will be transfered to window at least once.
			if (window.written_size >= WINLENGTH) {
				Fifo_pop_front(&window);
			}
			
			// Transfer the first character from look ahead buffer to end of window.
			Fifo_push_back(&window, Fifo_pop_front(&lookAhead));
			
			// Fill back up the look ahead buffer.
			Fifo_push_back(&lookAhead, mgetc(&fileInfo.p_file));
		} while (count++ < matches);

		longestMatch(window, lookAhead, 0, offset_thread, matches_thread);
		longestMatch(window, lookAhead, 1, offset_thread, matches_thread);
		longestMatch(window, lookAhead, 2, offset_thread, matches_thread);
		longestMatch(window, lookAhead, 3, offset_thread, matches_thread);

		writeToArchive(&lookAhead, &offset, &matches, &archiveInfo, offset_thread, matches_thread, &window, &lookAhead);
	}
	
	// Stop counting the elapsed cycles for the current reaction.
	counterStop(FSL_COUNTER_ID);
	counterRead(counter, FSL_COUNTER_ID);
	counterDifference(counter);
	counterMaximum(counter);
	counterMinimum(counter);
	counterAccumulate(counter);
	counterAverage(counter);
	xil_printf("Total time: %d cycles\r\n", counter.total);
	xil_printf("Program termination\r\n");
	
	asm volatile (".long 0x80000001\r\n");
	return 0;
}

void openFile(void) {
    archiveInfo.fileaddr = archive;
    fileInfo.totalSize = 0;
    fileInfo.bytesProcessed = 0;
    fileInfo.fileaddr = embedded;
    fileInfo.p_file = embedded;
    obtainFileInfo(&fileInfo);
    createDescriptor(&fileInfo, &archiveInfo);
}

int getFileSize(void) {
    return fileInfo.totalSize;
}

int getBytesProcessed(void) {
    if (fileInfo.bytesProcessed < fileInfo.totalSize) {
        return fileInfo.bytesProcessed;
    } else {
        return fileInfo.totalSize;
    }
}

//********************************************************
//* Start of function definitions for compressing files. *
//********************************************************

void createDescriptor(struct fileStats *fileInfo, struct fileStats *archiveInfo) {
    // obtainFileInfo() has been moved into createArchive() since obtainFileInfo() now obtains useful information
    // about the file requested, it is useful to provide extra information to createArchive() such as whether the
    // file is a directory.
    storeFileInfo(fileInfo, archiveInfo);
}

void obtainFileInfo(struct fileStats *fileInfo) {
    fileInfo->filesize = FILE_SIZE;
    fileInfo->totalSize = fileInfo->filesize;
}

void storeFileInfo(struct fileStats *fileInfo, struct fileStats *archiveInfo) {
    // Length of string plus 1 to take account of the null character.
    mwrite(&(fileInfo->filesize), 4, 1, &archiveInfo->p_file);
}

void longestMatch(const Fifo win, const Fifo lk, int begin, int *offset_thread, int *matches_thread) {
	int NUM_THREADS = 4;

    int _matches = 0;
    matches_thread[begin] = 0;
    int i, j, k, _offset;
    for (i = begin; i < win.written_size && Fifo_at(&win, i); ) {
        j = 0;

        _offset = i;
        for (k = i; j < LKLENGTH && Fifo_at(&win, k) == Fifo_at(&lk, j); ) {
            _matches++;
            j++;
            k++;
            
            if (k == win.written_size) {
            	k = 0;
            }
        }

        if (_matches > matches_thread[begin]) {
            matches_thread[begin] = _matches;
            offset_thread[begin] = _offset;
        }

        i += NUM_THREADS;
        _matches = 0;
    }
}

void writeToArchive(const Fifo *fifo, int *offset, int *matches, struct fileStats *archiveInfo, int *offset_thread, int *matches_thread, Fifo *window, Fifo *lookAhead) {
	int NUM_THREADS = 4;
	#undef EOF
	int EOF = -1;
	
	*matches = 0;
	int i;
	for (i = 0; i < NUM_THREADS; i++) {
		if (matches_thread[i] > *matches) {
			*matches = matches_thread[i];
			*offset = offset_thread[i];
		}
	}

    int encode=0x800000;
    encode += (*offset << 5) + (*matches - 1); // 0 matches will actually mean 1 match, matches range from 0~15, 4 bits.

    if (*matches > 2) {
    	fileInfo.bytesProcessed += *matches;
        mputc(encode >> 16, &archiveInfo->p_file); // Write the MSByte first so the MSbit can be read easily
        mputc(encode >> 8, &archiveInfo->p_file);
        mputc(encode, &archiveInfo->p_file);
    } else if (Fifo_at(fifo, 0) != EOF) {
    	fileInfo.bytesProcessed++;
        mputc(Fifo_at(fifo, 0), &archiveInfo->p_file);
        if (*matches == 2 && Fifo_at(fifo, 1) != EOF) {
        	fileInfo.bytesProcessed++;
            mputc(Fifo_at(fifo, 1), &archiveInfo->p_file);
        }
    }
    
    if (fileInfo.bytesProcessed >= fileInfo.totalSize) {
		int count = 1;
		do {
			// This loop will transfer the first node from look ahead buffer to window for 'matches' times.
			// The node from look ahead buffer will be transfered to window at least once.
			if (window->written_size >= WINLENGTH) {
				Fifo_pop_front(window);
			}
			
			// Transfer the first character from look ahead buffer to end of window.
			Fifo_push_back(window, Fifo_pop_front(lookAhead));
			
			// Fill back up the look ahead buffer.
			Fifo_push_back(lookAhead, mgetc(&fileInfo.p_file));
		} while (count++ < *matches);

        *matches = 0;
        for (count = 0; count < LKLENGTH - 1 && Fifo_at(lookAhead, 0) != EOF; count++) {
            mputc(Fifo_at(lookAhead, 0), &archiveInfo->p_file);
            Fifo_pop_front(lookAhead);
        }
    }
}

void printResult(struct fileStats *fileInfo, struct fileStats *archiveInfo) {
    obtainFileInfo(archiveInfo);
}


//*******************************************************
//* Start of function definitions for unpacking files.  *
//*******************************************************
void unpackArchive(char *filename, int *offset, int *matches, struct fileStats *fileInfo, struct fileStats *archiveInfo) {

}

void processDescriptor(struct fileStats *fileInfo, struct fileStats *archiveInfo) {

}


//****************************************************
//* Start of function definitions for decompression. *
//****************************************************
void decompressFile(int *offset, int *matches, struct fileStats *fileInfo, struct fileStats *archiveInfo) {
}

void decode(Fifo *win, int *sizeCount, int *offset, int *matches, struct fileStats *fileInfo, struct fileStats *archiveInfo) {

}

//**********************************


void Fifo_init(Fifo *fifo, unsigned size, char *buf) {
    fifo->begin = 0;
    fifo->end = 0;
    fifo->size = size;
    fifo->written_size = 0;
    fifo->data = buf;
}

void Fifo_destroy(Fifo *fifo) {

}

int Fifo_push_front(Fifo *fifo, char c) {
    if (fifo->written_size == 0) {
        Fifo_push_back(fifo, c);
    } else if (fifo->written_size >= fifo->size) {
        return -1;
    }
	
    fifo->begin--;
    if (fifo->begin < 0) {
        fifo->begin = fifo->size - 1;
    }
    fifo->data[fifo->begin] = c;
    fifo->written_size++;
	
	return 0;
}

int Fifo_push_back(Fifo *fifo, char c) {
    if (fifo->written_size >= fifo->size) {
        return -1;
    }
	
    fifo->written_size++;
    fifo->data[fifo->end] = c;
    fifo->end++;
    if (fifo->end == fifo->size) {
        fifo->end = 0;
	}
	
	return 0;
}

char Fifo_pop_front(Fifo *fifo) {
    if (fifo->written_size == 0) {
        return -1;
    }
	
    char c = fifo->data[fifo->begin];
    fifo->written_size--;
    fifo->begin++;
    if (fifo->begin == fifo->size) {
        fifo->begin = 0;
    }
    
    return c;
}

char Fifo_pop_back(Fifo *fifo) {
    if (fifo->written_size == 0) {
        return -1;
    }
	
    fifo->written_size--;
    fifo->end--;
    if (fifo->end < 0) {
        fifo->end = fifo->size - 1;
    }
    return fifo->data[fifo->end];
}

char Fifo_at(const Fifo *fifo, unsigned i) {
    if (i < fifo->written_size) {
        return fifo->data[(fifo->begin + i) % fifo->size];
    } else {
        return -1;
    }
}

void Fifo_print(const Fifo *fifo) {
    int i, j;
    for (i = fifo->begin, j = 0; j < fifo->written_size; i++, j++) {
        if (i == fifo->size) {
            i = 0;
        }
    }
}

//**********************************

int mgetc(char **stream)
{
    char *c = *stream++;
    return *c;
}

int mputc(char c, char **stream)
{
    **stream = c;
    stream++;
    return c;
}

int mwrite(const void *ptr, int size, int nmemb, char **stream)
{
    int n, s, ret = 0;

    for (n = 0; n < nmemb; n++) {
        for (s = 0; s < size; s++) {
            **stream = *((char*) ptr);
            stream++;
            ret++;
        }
    }

    return ret;
}



