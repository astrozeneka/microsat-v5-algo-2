#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEQ_RECORD_MEMORY_CHUNK 40
#define SEQ_SEQUENCE_MEMORY_CHUNK 4096
#define SEQ_MICROSATELLITE_MEMORY_CHUNK 64
// Sequence stucture

typedef struct {
    char *array;
    size_t used;
    size_t size;
} sequence;

void initSequence(sequence *a, size_t initialSize){
    // Structure memory allocation will be outside
    a->array = malloc(initialSize * sizeof(char) +1);
    a->used = 0;
    a->size = initialSize;
    a->array[a->used+1] == 0;
}

void insertSequence(sequence *a, char element){
    if (a->used+1 == a->size){
        a->size += SEQ_SEQUENCE_MEMORY_CHUNK;
        a->array = realloc(a->array, a->size * sizeof(char));
    }
    a->array[a->used++] = element;
}

void freeSequence(sequence *a){
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

typedef struct {
    char *name;
    char *description;
    sequence *sequence;
} record;

typedef struct {
    char *sequence;
    char *motif;
    int period;
    int repeat;
    int start;
    int end;
    int length;
} microsatellite;

typedef struct {
    microsatellite *array;
    size_t used;
    size_t size;
} microsatelliteArray;

void initMicrosatelliteArray(microsatelliteArray *a, size_t initialSize){
    a->array = malloc(initialSize * sizeof(microsatellite));
    a->used = 0;
    a->size = initialSize;
}

void insertMicrosatelliteArray(microsatelliteArray *a, microsatellite *element){
    if (a->used == a->size){
        a->size += SEQ_MICROSATELLITE_MEMORY_CHUNK;
        a->array = realloc(a->array, a->size*sizeof(microsatellite));
    }
    a->array[a->used++] = *element;
}

void readConfig(int *output, FILE *f){
    char x[8];
    int cursor=0;
    while (fscanf(f, "%1023s", x) == 1){
        if(cursor%2==1)
            output[cursor/2]=atoi(x);
        cursor++;
    }
    int a = 0;
}

void readFastaFile(record *output, FILE *f){
    // IMPORTANT, should initialize structure after creating
    output->name=NULL;
    output->description=NULL;
    output->sequence=malloc(sizeof(sequence)); // NULL
    initSequence(output->sequence, SEQ_RECORD_MEMORY_CHUNK); // THIS SHOULD BE THE BUG FIX

    char buffer[4096];
    char *line = NULL;
    size_t len = 0;
    while(fgets(buffer, 4096, f)) {


        line=malloc(sizeof(buffer));
        len=strlen(buffer);
        strncpy(line, buffer, len);
        line[len] = (char)0;

        if(strlen(line) == 0)
            continue;
        strtok(line, "\n");

        if(line[0] == '>'){
            char *x = malloc(64);
            sscanf(line, "%63s", x);
            output->name = malloc(64*sizeof(char));
            output->description = malloc(1024*sizeof(char));
            strcpy(output->name, x);
            output->name[strlen(x)] = 0;
            strcpy(output->description, line+strlen(x));
        }else{
            // Fasta reads
            int i;
            for(i = 0; line[i] != 0; i++)
                insertSequence(output->sequence, line[i]);
        }
    }
}

void search_perfect_microsatellites(microsatelliteArray *output, record *record, int *minRepeats){
    char *seq = record->sequence->array;
    size_t len;
    int start;
    int length;
    int repeat;
    int i;
    int j;
    char motif[7] = "\0";

    len = record->sequence->used;
    for (i=0; i<len; i++){
        if (seq[i] == 78)
            continue;
        if (seq[i] != 'A' && seq[i] != 'T' && seq[i] != 'C' && seq[i] != 'G') {
            char *n = malloc(1000000);
            strncpy(n, seq+i, 100);
            continue;
        }
        for (j=1; j<=6; j++){
            start = i;
            length=j;
            while(start+length<len && seq[i]==seq[i+j] && seq[i]!=78){
                i++;
                length++;
            }
            repeat=length/j;
            if(repeat >= minRepeats[j - 1]){
                microsatellite *m = malloc(sizeof(microsatellite));

                m->motif = malloc(16*sizeof(char));
                strncpy(m->motif, seq+start, j);
                m->motif[j] = 0;

                m->sequence=malloc(64*sizeof(char));
                strcpy(m->sequence, record->name);

                m->period=j;
                m->repeat=repeat;
                m->start=start+1;
                m->end=start+length;
                m->length=repeat*j;

                insertMicrosatelliteArray(output, m);
                free(m);

                i=start+length;
                j=0;
            }else{
                i=start;
            }
        }
    }
}

int main(int argc, char **argv){
    // Parse input
    const char *infile = NULL;
    const char *outfile = NULL;
    const char *configfile = NULL;
    int i;
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i-1], "-i") == 0)
            infile = argv[i];
        if(strcmp(argv[i-1], "-o") == 0)
            outfile = argv[i];
        if(strcmp(argv[i-1], "-c") == 0)
            configfile = argv[i];
    }

    // Open the configuration file
    int *minRepeats;
    if(configfile != NULL){
        FILE *fptr;
        fptr = fopen(configfile, "r");
        minRepeats = malloc(6*sizeof(int));
        readConfig(minRepeats,  fptr);
    }

    microsatelliteArray *microsatellites;
    if(infile != NULL){
        FILE *fptr;
        fptr = fopen(infile, "r");

        record *record=malloc(sizeof(record));
        readFastaFile(record, fptr);

        microsatellites=malloc(sizeof(microsatelliteArray));
        initMicrosatelliteArray(microsatellites, SEQ_MICROSATELLITE_MEMORY_CHUNK);
        search_perfect_microsatellites(microsatellites, record, minRepeats);
    }

    if(outfile != NULL){
        FILE *fptr;
        fptr = fopen(outfile, "w");

        int i;
        for(i = 0; i < microsatellites->used; i++){
            fprintf(fptr, "%s\t%s\t%d\t%d\t%d\t%d\t%d\n",
                    microsatellites->array[i].sequence,
                    microsatellites->array[i].motif,
                    microsatellites->array[i].period,
                    microsatellites->array[i].repeat,
                    microsatellites->array[i].start,
                    microsatellites->array[i].end,
                    microsatellites->array[i].length);
        }
        fclose(fptr);
        printf("File written\n");
    }
    return 0;
}