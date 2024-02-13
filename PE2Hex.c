#include <stdio.h>
#include <stdlib.h>
#include <shlwapi.h>


int main(int argc, char* argv[]) {
    FILE* fexe, * fout;

    if (argc < 2) {
        printf("Usage: %s <path/to/your/executable.exe>\n", argv[0]);
        return 1;
    }

    const char* exePath = argv[1]; // 从命令行参数接收EXE文件路径

    // 提取文件名（不包含路径）
    const char* filename = strrchr(exePath, '\\');
    if (!filename) { // 处理不包含路径分隔符的情况
        filename = exePath;
    }
    else {
        filename++; // 移过路径分隔符
    }

    char outputPath[260]; // 存储输出路径，假设路径长度不超过259字符
    snprintf(outputPath, sizeof(outputPath), "pe_%s.txt", filename); // 构建输出文件路径

    // 以二进制读取模式打开EXE文件
    fexe = fopen(exePath, "rb");
    if (!fexe) {
        perror("Failed to open executable");
        return 1;
    }

    // 移动到文件末尾以确定文件大小
    fseek(fexe, 0, SEEK_END);
    long fileSize = ftell(fexe);
    rewind(fexe); // 回到文件开头

    // 分配内存以存储文件内容
    unsigned char* buf = (unsigned char*)malloc(fileSize);
    if (!buf) {
        perror("Memory allocation failed");
        fclose(fexe);
        return 1;
    }

    // 读取文件内容到缓冲区
    fread(buf, 1, fileSize, fexe);
    fclose(fexe); // 关闭EXE文件

    // 打开输出文件
    fout = fopen(outputPath, "w");
    if (!fout) {
        perror("Failed to open output file");
        free(buf);
        return 1;
    }

    
    fprintf(fout, "unsigned char pBuffer[] = \n");
    // 将缓冲区内容输出到文本文件，每个字节前加\x，每16个字节换行，行首行尾加双引号
    for (long i = 0; i < fileSize; i++) {
        if (i % 16 == 0) { // 每16个字节或文件开始时
            if (i != 0) { // 如果不是文件的第一个字节，先结束前一行
                fprintf(fout, "\"\n");
            }
            fprintf(fout, "\""); // 行开始加双引号
        }
        fprintf(fout, "\\x%02x", buf[i]); // 以十六进制形式输出，每个字节前加\x
        if (i == fileSize - 1) { // 文件结束时
            fprintf(fout, "\""); // 行尾加双引号
        }
    }

    if (fileSize % 16 != 0) { // 如果最后一行不足16个字节，也需要在行尾加上双引号
        fprintf(fout, "\"");
    }
    fprintf(fout, ";\n"); // 文件输出完毕后换行

    // 清理资源
    fclose(fout);
    free(buf);

    printf("File has been successfully exported to %s\n", outputPath);
    return 0;
}