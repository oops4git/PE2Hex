#include <stdio.h>
#include <stdlib.h>
#include <shlwapi.h>


int main(int argc, char* argv[]) {
    FILE* fexe, * fout;

    if (argc < 2) {
        printf("Usage: %s <path/to/your/executable.exe>\n", argv[0]);
        return 1;
    }

    const char* exePath = argv[1]; 


    const char* filename = strrchr(exePath, '\\');
    if (!filename) { 
        filename = exePath;
    }
    else {
        filename++; 
    }

    char outputPath[260]; 
    snprintf(outputPath, sizeof(outputPath), "pe_%s.txt", filename); 


    fexe = fopen(exePath, "rb");
    if (!fexe) {
        perror("Failed to open executable");
        return 1;
    }


    fseek(fexe, 0, SEEK_END);
    long fileSize = ftell(fexe);
    rewind(fexe); 


    unsigned char* buf = (unsigned char*)malloc(fileSize);
    if (!buf) {
        perror("Memory allocation failed");
        fclose(fexe);
        return 1;
    }


    fread(buf, 1, fileSize, fexe);
    fclose(fexe); 


    fout = fopen(outputPath, "w");
    if (!fout) {
        perror("Failed to open output file");
        free(buf);
        return 1;
    }

    
    fprintf(fout, "unsigned char pBuffer[] = \n");

    for (long i = 0; i < fileSize; i++) {
        if (i % 16 == 0) { 
            if (i != 0) { 
                fprintf(fout, "\"\n");
            }
            fprintf(fout, "\""); 
        }
        fprintf(fout, "\\x%02x", buf[i]); 
        if (i == fileSize - 1) { 
            fprintf(fout, "\""); 
        }
    }

    if (fileSize % 16 != 0) { 
        fprintf(fout, "\"");
    }
    fprintf(fout, ";\n"); 


    fclose(fout);
    free(buf);

    printf("File has been successfully exported to %s\n", outputPath);
    return 0;
}
