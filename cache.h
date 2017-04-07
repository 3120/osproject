
int *cache_table;//for storing the # of references to file
int *cfd_table; //for storing cfd
char *file_table; //for storing file contents
static int table_size = 0;

void cache_init(int size);

int cache_open(char *file);

int cache_send(int cfd, int client, int n);

int cache_filesize(int cfd);

int cache_close(int cfd);
