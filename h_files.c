#include "habits.h"

static const unsigned int BUFSIZE = 4000 * sizeof (char);

char* alloccat(char* home, char* name) {
    char* realname = (char*) malloc(sizeof (*realname) * (strlen(home)+strlen(name) + 1) );
    strcpy(realname, home);
    strcat(realname, name);
    return realname;
}

/*int file_delete_line(char* path, char* name) {*/
/*    int file_to_read = open(path, O_RDONLY);*/
/*    int found = 0;*/
/*    if (file_to_read == -1) {*/
/*        puts("File could not be opened!");*/
/*        found = -1;*/
/*    } else {*/
/**/
/*        uint total_chars = lseek(file_to_read, 0, SEEK_END);*/
/*        uint number_of_chunks = total_chars / BUFSIZE;*/
/*        uint remaining_chars = total_chars % BUFSIZE;*/
/*        lseek(file_to_read, 0, SEEK_SET);*/
/**/
/*        char *tempfilepath = alloccat(path, "___temp___");*/
/*        int tempfileptr = open(tempfilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);*/
/**/
/*        uint i;*/
/*        char *searchable_name = alloccat(name, "\n");*/
/*        int nbytes = BUFSIZE;*/
/*        char* filebuffer;*/
/**/
/*        for (i = 1; i <= number_of_chunks+1; i++) {*/
/*            if (i == number_of_chunks+1) nbytes = remaining_chars;*/
/**/
/*            filebuffer = malloc(nbytes * sizeof filebuffer);*/
/*            read(file_to_read, filebuffer, nbytes);*/
/**/
/*            char* replace_addr;*/
/*            if (found == 0) {*/
/*                replace_addr = strstr(filebuffer, searchable_name);*/
/*                if (replace_addr != NULL) {*/
/*                    size_t len = strlen(searchable_name);*/
/*                    memmove(replace_addr, replace_addr + len, strlen(replace_addr+len) + 1 );*/
/*                    found = 1;*/
/*                }*/
/*            } */
/*            write(tempfileptr, filebuffer, nbytes);*/
/*            free(filebuffer);*/
/*        } */
/**/
/*        if (found == 1) {*/
/*            ftruncate(tempfileptr, total_chars - strlen(name) - 1);*/
/*            remove(path);*/
/*            rename(tempfilepath, path);*/
/*        } else {*/
/*            remove(tempfilepath);*/
/*        }*/
/**/
/*        close(tempfileptr);*/
/*        free(tempfilepath);*/
/*        free(searchable_name);*/
/*    } */
/*    close(file_to_read);*/
/*    return found;*/
/*}*/

int file_delete_line(char* path, char* name) {
    /* Open file */
    int file_to_read = open(path, O_RDONLY);
    int found = 0;
    if (file_to_read == -1) {
        puts("File could not be opened!");
        return -1;
    }

    /* Determine the size of the file */
    off_t total_chars = lseek(file_to_read, 0, SEEK_END);
    if (total_chars == -1) {
        puts("Could not determine file size!");
        close(file_to_read);
        return -1;
    }
    lseek(file_to_read, 0, SEEK_SET);

    /* Temporary file */
    char *tempfilepath = alloccat(path, "___temp___");
    int tempfileptr = open(tempfilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (tempfileptr == -1) {
        puts("Temporary file could not be created!");
        close(file_to_read);
        free(tempfilepath);
        return -1;
    }

    char *searchable_name = alloccat(name, "\n");
    size_t len = strlen(searchable_name);
    char filebuffer[BUFSIZE];

    ssize_t nbytes;
    ssize_t write_offset = 0;

    while ((nbytes = read(file_to_read, filebuffer, BUFSIZE)) > 0) {
        char* replace_addr = NULL;

        if (!found) {
            replace_addr = strstr(filebuffer, searchable_name);
            if (replace_addr != NULL) {
                found = 1;
                size_t bytes_to_write = replace_addr - filebuffer;

                /* Write the part before the found line to the temp file */
                if (write(tempfileptr, filebuffer, bytes_to_write) != bytes_to_write) {
                    puts("Error writing to temporary file!");
                    close(file_to_read);
                    close(tempfileptr);
                    free(tempfilepath);
                    free(searchable_name);
                    return -1;
                }

                /* Skip the line to be deleted */
                size_t remaining_bytes = nbytes - (bytes_to_write + len);
                if (remaining_bytes > 0) {
                    /* Write the remaining part after the line to be deleted */
                    if (write(tempfileptr, replace_addr + len, remaining_bytes) != remaining_bytes) {
                        puts("Error writing to temporary file!");
                        close(file_to_read);
                        close(tempfileptr);
                        free(tempfilepath);
                        free(searchable_name);
                        return -1;
                    }
                }
                write_offset = bytes_to_write + remaining_bytes;
            } else {
                if (write(tempfileptr, filebuffer, nbytes) != nbytes) {
                    puts("Error writing to temporary file!");
                    close(file_to_read);
                    close(tempfileptr);
                    free(tempfilepath);
                    free(searchable_name);
                    return -1;
                }
            }
        } else {
            /* Write the remaining data after the found line */
            if (write(tempfileptr, filebuffer + write_offset, nbytes - write_offset) != nbytes - write_offset) {
                puts("Error writing to temporary file!");
                close(file_to_read);
                close(tempfileptr);
                free(tempfilepath);
                free(searchable_name);
                return -1;
            }
            write_offset = 0; // Reset after the first write after finding the line
        }
    }

    if (nbytes == -1) {
        puts("Error reading from file!");
        close(file_to_read);
        close(tempfileptr);
        free(tempfilepath);
        free(searchable_name);
        return -1;
    }

    /* If file was changed, rename */
    if (found == 1) {
        close(file_to_read);
        close(tempfileptr);
        remove(path);
        rename(tempfilepath, path);
    } else {
        close(file_to_read);
        close(tempfileptr);
        remove(tempfilepath);
    }

    free(tempfilepath);
    free(searchable_name);
    return found;
}

int line_insert_fgets(char* path, char* where, char* name ) {
    char* copypath = alloccat(path, "___temp___"); FILE* fread = fopen(path, "r");
    FILE* fw = fopen(copypath, "w");
    int res = 0;
    if (fread == NULL) {
        res = -1;
    } else {
        if (fw == NULL) {
            res = -2;
        }
        else {
            register char* copywhere = alloccat(where, "\n");
            char* copyname = alloccat(name, "\n");
            char buffer[31];
            while (NULL != fgets(buffer, 31, fread)) {
                if (0 == strcmp(buffer, copywhere)) {
                    fputs(copyname, fw);
                    res = 1;
                }
                fputs(buffer, fw);
            }
            fclose(fw);
            remove(path);
            rename(copypath, path);
            free(copyname);
            free(copywhere);
        } 
        fclose(fread);
    }
    free(copypath);
    return 0;
}

int line_replace_fgets(char* path, char* where, char* name ) {
    char* copypath = alloccat(path, "___temp___");
    FILE* fread = fopen(path, "r");
    FILE* fw = fopen(copypath, "w");
    int res = 0;
    if (fread == NULL) {
        res = -1;
    } else {
        if (fw == NULL) {
            res = -2;
        }
        else {
            register char* copywhere = alloccat(where, "\n");
            char* copyname = alloccat(name, "\n");
            char buffer[30];
            while (NULL != fgets(buffer, 30, fread)) {
                if (0 == strcmp(buffer, copywhere)) {
                    fputs(copyname, fw);
                    res = 1;
                }
                else {
                    fputs(buffer, fw );
                }
            }
            fclose(fw);
            remove(path);
            rename(copypath, path);
            free(copyname);
            free(copywhere);
        } 
        fclose(fread);
    }
    free(copypath);
    return res;
}

int file_prepend(char* path, char* what) {
    /* Open file */
    int file_to_read = open(path, O_RDONLY);
    int found = 0;
    if (file_to_read == -1) {
        puts("File could not be opened!");
        found = -1;
    } else {
        /* How many times we will chunk */
        uint total_chars = lseek(file_to_read, 0, SEEK_END);
        uint number_of_chunks = total_chars / BUFSIZE;
        uint remaining_chars = total_chars % BUFSIZE;
        lseek(file_to_read, 0, SEEK_SET);

        /* Temporary file */
        char *tempfilepath = alloccat(path, "___temp___");
        int file_to_write = open(tempfilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        /* Insert Pre Pended Line */
        write(file_to_write, what, strlen(what));
        write(file_to_write, "\n", 1);

        /* Copy File*/
        uint nbytes = BUFSIZE;
        char* filebuffer = malloc(nbytes * sizeof filebuffer);;
        for (register unsigned int i = 1; i <= number_of_chunks+1; i++) {
            if (i == number_of_chunks+1) nbytes = remaining_chars;
            read(file_to_read, filebuffer, nbytes);
            write(file_to_write, filebuffer, nbytes);
        } 
        free(filebuffer);
        close(file_to_write);

        remove(path);
        rename(tempfilepath, path);

        free(tempfilepath);
        close(file_to_read);
        found = 1;
    } /* end of else statement */
    return found;
}

int file_append(char* path, char* what) {
    /* Open file */
    int file_to_read = open(path, O_RDONLY);
    int found = 0;
    if (file_to_read == -1) {
        found = -1;
    } else {
        close(file_to_read);
        /* Append Mode */
        int file_to_write = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        write(file_to_write, what, strlen(what));
        write(file_to_write, "\n\0", 1);
        close(file_to_write);
        found = 1;
    } 
    return found;
}

int file_touch(char* path) {
    int file_to_touch = open(path, O_RDONLY);
    int status = FALSE;
    if (file_to_touch != -1) {
        status = FALSE;  
        close(file_to_touch);
    } else {
        file_to_touch = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_to_touch == -1) {
            status = -1;  
        } else {
            status = TRUE;
            close(file_to_touch);
        }
    }

    return status;
}
