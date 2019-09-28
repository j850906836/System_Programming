#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <linux/limits.h>

int level = 0;
int i=0;
int stair=0;
typedef struct arr{
	char name[25];
	char path[25];
	int down;
}arr;
arr array[5000];
void storeName(char* name,char* path) {
	strcpy(array[i].name,name);
	strcpy(array[i].path,path);
	array[i].down=stair;
	i++;
}
int compare(const void *a,const void *b) {
	return strcmp(((struct arr*)a)->name,((struct arr*)b)->name);
}	
int mycompare(const void *a,const void *b) {
	return ((struct arr*)a)->down-((struct arr*)b)->down;
}
void print(){
	int j=0;
	while(j<i) {
		printf("%s\t%s\n",array[j].name,array[j].path);
		j++;
	}
}
void listDir(char* pathName)
{
    level++;
    DIR* curDir = opendir(pathName);
    assert(curDir!=NULL);
    char* newPathName = (char*)malloc(PATH_MAX);
    struct dirent entry;
    struct dirent* result;
    int ret;
    ret = readdir_r(curDir, &entry, &result);
    assert(ret == 0);
    while(result != NULL) {
        if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0) {
            ret = readdir_r(curDir, &entry, &result);
            assert(ret == 0);
            continue;
        }
        if (entry.d_type == DT_LNK)
            storeName(entry.d_name,pathName);
        if (entry.d_type == DT_REG)
            storeName(entry.d_name,pathName);
        if (entry.d_type == DT_DIR) {
            storeName(entry.d_name,pathName);
	    stair++;
            sprintf(newPathName, "%s/%s", pathName, entry.d_name);
          //  printf("%s\n", newPathName);
            listDir(newPathName);
        }
        ret = readdir_r(curDir, &entry, &result);
        assert(ret == 0);        
    }
    closedir(curDir);
    level--;
}

int main(int argc, char** argv) {
    listDir(argv[1]);
    qsort(array,i,sizeof(struct arr),compare);
    qsort(array,i,sizeof(struct arr),mycompare); 	
    print();		
}
