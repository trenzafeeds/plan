/******************
 * utils.h
 ******************/

#ifndef UTILSHEADER
#define UTILSHEADER

/* Flags */
enum {
      NOFLAG = 0, // b0
      FLAG1  = 1, // b1
      FLAG2  = 2, // b10
      FLAG3  = 4, // b100
      FLAG4  = 8  // b1000
};

int getindex(int val, int *arr, int len);
void arrwipe(int *arr, int len);

int strcut(char *src, char *dest, int begin, int end);
char *strcop(char *s);

bool file_exists(char *fname);
FILE *open_file(char *fpath, char *action);

void *smalloc(size_t n);
void *scalloc(size_t nmemb, size_t size);
void *smemcpy(void *dest, const void *src, size_t n);
void *smemset(void *str, int c, size_t n);

void sys_error(char *msg, int exit_code);
void app_error(char *msg, int exit_code);
void debug(char *msg, ...);

#endif /* UTILSHEADER */
