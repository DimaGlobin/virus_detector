#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

void XOR_cipher(const char* file_path, const char* key) {

    printf("file_path: %s\n", file_path);
    FILE* fp = fopen(file_path, "rb+");
    assert(fp != NULL);

    int key_len = strlen(key);
    int i = 0;
    int ch = 0;

    while ((ch = fgetc(fp)) != EOF) {   // Read every byte in file

        ch = ch ^ key[i % key_len];     // XOR the byte with the key
        fseek(fp, -1, SEEK_CUR);        // Move the file pointer back one byte
        fputc(ch, fp);                  // Write the encrypted byte to the file
        i++;
    }

    fclose(fp);
}

void walk(const char* path, const char* key) {

    DIR* dir_ptr = NULL;                     // Initialize necessary structures
    struct dirent* direntp;
    char *npath = NULL;

    if (!path)                               // Error check
        return;

    if( (dir_ptr = opendir(path)) == NULL ) 
        return;

    while( (direntp = readdir(dir_ptr))) {

        if (strcmp(direntp -> d_name, ".") == 0 || strcmp(direntp -> d_name, "..") == 0)  // Skip directories "." and ".."
            continue;

        npath = (char* )malloc(strlen(path) + strlen(direntp -> d_name) + 2);
        sprintf(npath, "%s/%s", path, direntp -> d_name);

        switch (direntp -> d_type) {                                             // Check if it regular file or directory 
                                                                                 //If it is a directory recursively move on 
            case DT_REG:

                printf("d_name: %s\n", direntp -> d_name);

                XOR_cipher(npath, key);

                break;

            case DT_DIR:  

                
                walk(npath, key);

                free(npath);

                break;
        }
    }
}

int main() {

    const char* key = "cyberprotect";

    walk("simple_test", key);

    return 0;
}