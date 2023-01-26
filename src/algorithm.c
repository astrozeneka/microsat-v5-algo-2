#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEQ_RECORD_MEMORY_CHUNK 40
#define SEQ_SEQUENCE_MEMORY_CHUNK 409600
#define SEQ_MICROSATELLITE_MEMORY_CHUNK 6400

// Sequence stucture

typedef struct {
    char *array;
    size_t used;
    size_t size;
} sequence;

void initSequence(sequence *a, size_t initialSize){
    // Structure memory allocation will be outside
    a->array = malloc(initialSize * sizeof(char));
    a->used = 0;
    a->size = 0;
}

void insertSequence(sequence *a, char element){
    if (a->used == a->size){
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

void readFastaFile(FILE *f){
    record *output = malloc(sizeof(record));
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
            int b = 0;
        }else{
            int a = 0;
        }
    }
}

int main(int argc, char **argv){
    // Parse input
    const char *infile = NULL;
    const char *outfile = NULL;
    const char *configfile = NULL;

    for(int i = 1; i < argc; i++){
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

    if(infile != NULL){
        FILE *fptr;
        fptr = fopen(infile, "r");

        readFastaFile(fptr);

    }

}