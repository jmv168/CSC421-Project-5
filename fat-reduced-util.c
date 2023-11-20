#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<sys/time.h>
#include<assert.h>
#include<time.h>

#include "fat-reduced.h"

/*
 * name: fat-reduced-util.c
 * last update:
 *    13 nov 2022, bjr
 *     5 nov 2023, bjr
 *
 */

static struct DirEnt root_dir[DIR_N] ;
static unsigned int fat_table[FAT_N] ;
static struct Cluster cluster_table[CLUSTER_N] ;

void init_fat() {
	 int i ;
	 if (is_verbose_g>1) {
		printf("(%s,%d):\n\t%ld fat entries (%ld bytes),"
			"\n\t%ld dir entries (%ld bytes),"
			"\n\tdirent size is %ld bytes.\n", 
			__FILE__,__LINE__,
			(unsigned long) FAT_N, sizeof(fat_table), 
			(unsigned long) DIR_N, sizeof(root_dir), 
			 sizeof(struct DirEnt)
			) ;
	}

	for (i=0;i<FAT_N;i++) fat_table[i] = 0 ;
	for (i=0;i<sizeof(root_dir);i++) ((char *)root_dir)[i] = 0 ;
	return ;
}

int qu_action(int actc, char * actv[]) {
	if (is_verbose_g>1) {
		printf("(%s,%d):qu_action\n",__FILE__,__LINE__) ;
	}
	return ERR_ABORT ;
}

int ls_action(int actc, char * actv[]) {

	// ...

	if (is_verbose_g>1) {
		printf("(%s,%d):action: %s, %d\n",__FILE__,__LINE__,(char *)actv[0], actc) ;
	}
	
	
	// ...
	for (int i = 0; i < DIR_N; i++) { // Loop through dir
        if (root_dir[i].name[0] != '\0') {  // Check if entry not empty
            printf("%3d %6d %s\n", i, root_dir[i].len, root_dir[i].name); // If not empty, print
        }
    }

	
	return 0 ;
}

int wr_action(int actc, char * actv[]) {

	// ...

	if (is_verbose_g>1) {
		printf("(%s,%d):action: %s, %d\n",__FILE__,__LINE__,(char *)actv[0], actc) ;
	}
	
	if (strlen(actv[1])>FILENAME_LEN) actv[1][FILENAME_LEN] = '\0' ;


	// ...

    int directory_index = -1;
	int cluster_index = -1;

    for (int i = 0; i < DIR_N; i++) { // Find an empty directory entry and get its index
        if (root_dir[i].name[0] == '\0') {
            directory_index = i;
            break;
        }
    }
    if (directory_index == -1) {
        printf("No free directory entries were found.\n");
        return 0;
    }

    for (int i = 0; i < FAT_N; i++) {
        if (fat_table[i] == FAT_FREE) {  // Find a free cluster and get its index
            cluster_index = i;
            break;
        }
    }

    if (cluster_index == -1) {
		printf("No free clusters were found.\n");
        return 0;
    }

	// Write the file in the free cluster

    strncpy(root_dir[directory_index].name, actv[1], FILENAME_LEN);  // Copy the filename
    root_dir[directory_index].len = strlen(actv[2]);  // Set lenght
    root_dir[directory_index].starting_cluster = cluster_index;  // Set cluster index
    strncpy(cluster_table[cluster_index].data, actv[2], CLUSTER_SIZE); // Copy data

    
    fat_table[cluster_index] = FAT_LAST; // Update the table
	
	
	return 0 ;
}

int rd_action(int actc, char * actv[]) {

	// ...
	
	if (is_verbose_g>1) {
		printf("(%s,%d):action: %s, %d\n",__FILE__,__LINE__,(char *)actv[0], actc) ;
	}


	// ...
    for (int i = 0; i < DIR_N; i++) {  // Loop through directory
        if (strcmp(root_dir[i].name, actv[1]) == 0) {  // if file is found, print it
            int cluster_index = root_dir[i].starting_cluster;
            printf("%s\n", cluster_table[cluster_index].data);
            return 0;
        }
    }
	printf("ERROR: File was not found.\n"); // Print error if file not found
	return 0 ;
}

int rm_action(int actc, char * actv[]) {

	// ...
	
	if (is_verbose_g>1) {
		printf("(%s,%d):action: %s, %d\n",__FILE__,__LINE__,(char *)actv[0], actc) ;
	}


	// ...
    for (int i = 0; i < DIR_N; i++) {  // Loop through directory
        if (strcmp(root_dir[i].name, actv[1]) == 0) { // If found...
            root_dir[i].name[0] = '\0'; // Clear the entry
            root_dir[i].len = 0;
            int cluster_index = root_dir[i].starting_cluster;
            fat_table[cluster_index] = FAT_FREE;  // Indicate entry is free 
            return 0;
        }
    }
	printf("ERROR: File was not found.\n");

	return 0 ;
}
/* end of file */
