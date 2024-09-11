#include "habits.h"

int file_check(char* path) {
    int file_to_touch = open(path, O_RDONLY);
    int status = FALSE;

    if (file_to_touch == -1) {
        status = ERR_NOFILE;  
    } else {
        status = TRUE;
        close(file_to_touch);
    }

    return status;
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
            status = ERR_NOFILE;  
        } else {
            status = TRUE;
            close(file_to_touch);
        }
    }

    return status;
}

char* alloccat(char* dest, char* add) {
    char* copydest = (char*) malloc(sizeof(char)*(strlen(dest)+(strlen(add))+1));
    strcpy(copydest, dest);
    strcat(copydest, add);
    return copydest;
}

/* appending is easy, no sectoring needed */
int file_append(char* path, char* what) {
    int file_to_read = open(path, O_RDONLY);
    int found = 0;
    if (file_to_read == -1) {
        close(file_to_read);
        return ERR_NOFILE;
    }

    close(file_to_read);
    int file_to_write = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    write(file_to_write, what, strlen(what));
    write(file_to_write, "\n\0", 1);
    close(file_to_write);
    found = 1;

    return found;
}

/* prepending is hard */
int file_prepend(char* path, char* what) {
    int file_to_read = open(path, O_RDONLY);
    int found = 0;

    if (file_to_read == -1) {
        puts("File could not be opened!");
        close(file_to_read);
        return -1;
    } 

    uint total_chars = lseek(file_to_read, 0, SEEK_END);
    lseek(file_to_read, 0, SEEK_SET);
    char *tempfilepath = alloccat(path, "___temp___");
    int file_to_write = open(tempfilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(file_to_write, what, strlen(what));

    if ( *(what + strlen(what) ) != '\n' )
        write(file_to_write, "\n", 1);

    unsigned char filebuffer[BUFSIZ];
    for (uint i = 1; i <= total_chars/BUFSIZ + 1; i++) {
        int bytes_read = read(file_to_read, (unsigned char*) filebuffer, BUFSIZ);
        write(file_to_write, filebuffer, bytes_read);
    } 

    close(file_to_write);
    remove(path);
    rename(tempfilepath, path);
    free(tempfilepath);
    close(file_to_read);
    return found;
}

/* deletes line if found "what" */
int file_delete_line(char* path, char* what) {
    int file_to_read = open(path, O_RDONLY);

    if (file_to_read == -1) {
        puts("File could not be opened!");
        return ERR_NOFILE;
    } 

    uint total_chars = lseek(file_to_read, 0, SEEK_END);
    lseek(file_to_read, 0, SEEK_SET);

    char filebuffer[total_chars+1];
    read(file_to_read, (unsigned char*) filebuffer, total_chars);
    *(filebuffer+total_chars) = '\0'; /* necessary */

    char* found;
    if (NULL != (found = strstr(filebuffer, what)) ) 
    {
        char *tempfilepath = alloccat(path, "___temp___");
        int file_to_write = open(tempfilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        write(file_to_write, filebuffer, found-filebuffer); 
        char* next_line = strchr(found, '\n');

        if (next_line != NULL)
            write(file_to_write, next_line+1, strlen(next_line+1));
        else 
            write(file_to_write, "\0", 1);

        close(file_to_write);
        remove(path);
        rename(tempfilepath, path);
        free(tempfilepath);
    }

    close(file_to_read);
    return TRUE;
}

/* insert line */
int file_insert_line(char* path, char* where, char* what) {
    int file_to_read = open(path, O_RDONLY);
    if (file_to_read == -1) {
        puts("File could not be opened!");
        return -1;
    } 

    uint total_chars = lseek(file_to_read, 0, SEEK_END);
    lseek(file_to_read, 0, SEEK_SET);
    char *tempfilepath = alloccat(path, "___temp___");
    int file_to_write = open(tempfilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    int is_found = 0;

    char filebuffer[BUFSIZ+1];

    for (uint i = 1; i <= total_chars/BUFSIZ + 1; i++) {
        int bytes_read = read(file_to_read, (unsigned char*) filebuffer, BUFSIZ);
        filebuffer[BUFSIZ] = '\0'; /* This line is necessary since we are not reading the entire file */
        if ( is_found == 0) {
            char* found = strstr(filebuffer, where);
            if (found != NULL) { 
                char* next_line = found + strlen(where) + 1;
                write(file_to_write, filebuffer, found-filebuffer); 
                write(file_to_write, what, strlen(what)); 
                if (strchr(what, '\n') == NULL) {
                    write(file_to_write, "\n", 1);
                }
                write(file_to_write, found, strlen(found));
                is_found = 1;
            } else {
                write(file_to_write, filebuffer, bytes_read);
            }
        } else {
            write(file_to_write, filebuffer, bytes_read);
        }

    } 

    close(file_to_write);
    remove(path);
    rename(tempfilepath, path);
    free(tempfilepath);
    close(file_to_read);

    return is_found;
}

int file_list_dir(char* path) {
    DIR *directory = opendir(path);    
    return 1;
}
