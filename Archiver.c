#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct
{
    int file_name_size;
    int source_file_size;
}file_size;

void add_to_arc(FILE*, char*);
void extract_from_arc(char*);
void extract_file(FILE*);
void list_arc(char*);
void list_file(FILE*);

void add_to_arc(FILE* arc, char* file_name)
{
    FILE* source_file = fopen(file_name, "rb");
    if (source_file == NULL)
    {
        printf("Error of opening file\n");
        exit(1);
    }
    file_size sizes;
    sizes.file_name_size = strlen(file_name) + 1;
    sizes.source_file_size = 0;
    char byte;
    fread(&byte, 1, 1, source_file);
    while (!feof(source_file))
    {
        ++sizes.source_file_size;
        fread(&byte, 1, 1, source_file);
    }
    fwrite(&sizes.file_name_size, 4, 1, arc);
    fwrite(&sizes.source_file_size, 4, 1, arc);
    fwrite(file_name, sizes.file_name_size, 1, arc);
    rewind(source_file);
    fread(&byte, 1, 1, source_file);
    while (!feof(source_file))
    {
        fwrite(&byte, 1, 1, arc);
        fread(&byte, 1, 1, source_file);
    }
    fclose(source_file);
    remove(file_name);
}

void extract_from_arc(char* arc_name)
{
    FILE* arc = fopen(arc_name, "rb");
    if (arc == NULL)
    {
        printf("Error of opening file\n");
        exit(1);
    }
    int number_of_files;
    fread(&number_of_files, 4, 1, arc);
    for (int i = 0; i < number_of_files; ++i)
    {
        extract_file(arc);
    }
    fclose(arc);
    remove(arc_name);
}

void extract_file(FILE* arc)
{
    file_size sizes;
    fread(&sizes, 4, 2, arc);
    char* file_name = (char*)malloc(sizes.file_name_size);
    fread(file_name, sizes.file_name_size, 1, arc);
    FILE* source_file = fopen(file_name, "wb+");
    if (source_file == NULL)
    {
        printf("Error of opening file\n");
        exit(1);
    }
    char byte;
    for (int i = 0; i < sizes.source_file_size; ++i)
    {
        fread(&byte, 1, 1, arc);
        fwrite(&byte, 1, 1, source_file);
    }
    fclose(source_file);
    free(file_name);
}

void list_arc(char* arc_name)
{
    FILE* arc = fopen(arc_name, "rb");
    if (arc == NULL)
    {
        printf("Error of opening file\n");
        exit(1);
    }
    int number_of_files;
    fread(&number_of_files, 4, 1, arc);
    printf("%d\n", number_of_files);
    for (int i = 0; i < number_of_files; ++i)
    {
        list_file(arc);
    }
    fclose(arc);
}

void list_file(FILE* arc)
{
    file_size sizes;
    fread(&sizes, 4, 2, arc);
    char* file_name = (char*)malloc(sizes.file_name_size);
    fread(file_name, sizes.file_name_size, 1, arc);
    printf("%s\n", file_name);
    free(file_name);
    fseek(arc, sizes.source_file_size, SEEK_CUR);
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Error\n");
        return 1;
    }
    if (strcmp(argv[1], "--file") == 0)
    {
        if (strcmp(argv[3], "--create") == 0)
        {
            FILE* arc = fopen(argv[2], "wb+");
            if (arc == NULL)
            {
                printf("Error of opening file\n");
                return 1;
            }
            int number_of_files;
            number_of_files = argc - 4;
            fwrite(&number_of_files, 4, 1, arc);
            for (int i = 0; i < number_of_files; ++i)
            {
                add_to_arc(arc, argv[i + 4]);
            }
            fclose(arc);
            return 0;
        }
        if (strcmp(argv[3], "--extract") == 0)
        {
            extract_from_arc(argv[2]);
            return 0;
        }
        if (strcmp(argv[3], "--list") == 0)
        {
            list_arc(argv[2]);
            return 0;
        }
        printf("Error\n");
        return 1;
    }
    else
    {
        printf("Error\n");
        return 1;
    }
}