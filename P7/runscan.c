#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int name_search(int fd, int num_group, const struct ext2_group_desc *group, struct ext2_super_block *super, int inode_no, char *inode_name);
int data_copy(int fd, struct ext2_inode *inode, FILE* fp);
int write_detail(struct ext2_inode *inode, FILE* fp);
                                       
int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    // creates the new directory
    if(mkdir(argv[2], 0777) != 0){
        printf("Error: Unable to create directory.\n");
        return -1;
    }

    int fd;
    fd = open(argv[1], O_RDONLY);    /* open disk image */

    ext2_read_init(fd);

    struct ext2_super_block super;
    struct ext2_group_desc group;
    struct ext2_inode inode;

    unsigned int block_size = 1024; 

    // read first the super-block and group-descriptor
    // superblock and groupblock are the same for each group, so only need to read it once and stort it in an adress
    read_super_block(fd, 0, &super);
    read_group_desc(fd, 0, &group); 

    // number of groups of this disk image
    int num_group = (super.s_blocks_count + blocks_per_group - 1) / blocks_per_group; 

    char buffer[1024]; // buffer to store the data block
    int inode_no = 1; // inode starts at 1
    FILE *fp; // pointer to newly created inode number file 
    FILE *fp_act; // pointer to newly created actual name file 
    FILE *fp_det; // pointer to txt detail file

    // each group has its own inode table
    // for each group, loop through its inode table
    // read_inode's 2nd parameter is offset to the inode table of the group
    // 3rd parameter is the inode number within that group's inode table
    // idx + 1 bc inode number starts at 1, but idx starts at 0
    // the final offset will be [(offset to the inode table of the group) + (idx + 1 - 1) * inode_size]
    // example: offset to 2nd group's inode table is 2000; 
    // to reach the 1st inode of the group: 2000 + ((0+1)th - 1) * inode_size

    for(int ngroup = 0; ngroup < num_group; ngroup++){

        for(int idx = 0; idx < (int)super.s_inodes_per_group; idx++){
            read_inode(fd, locate_inode_table(ngroup, &group) , idx + 1, &inode, super.s_inode_size);

            if(S_ISREG(inode.i_mode)){
                lseek(fd, inode.i_block[0]*block_size, SEEK_SET); // read the first data block of the inode
                read(fd, &buffer, block_size);

                if (buffer[0] == (char)0xff &&
                    buffer[1] == (char)0xd8 &&
                    buffer[2] == (char)0xff &&
                    (buffer[3] == (char)0xe0 ||
                    buffer[3] == (char)0xe1 ||
                    buffer[3] == (char)0xe8)) 
                {
                    
                    // create the jpg file with inode number
                    char name[EXT2_NAME_LEN];
                    char inode_num[10];
                    sprintf(inode_num, "%d", inode_no);
                    strcpy(name, argv[2]);
                    strncat(name, "/file-", 7);
                    strncat(name, inode_num, strlen(inode_num));
                    strncat(name, ".jpg", 5);
                    fp = fopen(name, "w");
                    if (fp == NULL) {
                        printf("Error: Unable to create file1.\n");
                        return -1;
                    }
                    
                    // create the jpg file with actual name
                    char *jpg_name = malloc(EXT2_NAME_LEN);
                    name_search(fd,num_group, &group, &super, inode_no, jpg_name);

                    char name_act[EXT2_NAME_LEN];
                    strcpy(name_act, argv[2]);
                    strncat(name_act, "/", 2);
                    strncat(name_act, jpg_name, strlen(jpg_name));
                    fp_act = fopen(name_act, "w");
                    if (fp_act == NULL) {
                        printf("Error: Unable to create file2.\n");
                        return -1;
                    }

                    // creat the txt file with details
                    char name_det[EXT2_NAME_LEN];
                    strcpy(name_det, argv[2]);
                    strncat(name_det, "/file-", 7);
                    strncat(name_det, inode_num, strlen(inode_num));
                    strncat(name_det, "-details.txt", 13);
                    fp_det = fopen(name_det, "w");
                    if (fp_det == NULL) {
                        printf("Error: Unable to create file3.\n");
                        return -1;
                    }

                    // copy data and detail
                    data_copy(fd, &inode, fp);
                    data_copy(fd, &inode, fp_act);
                    write_detail(&inode, fp_det);
                    
                }
            }
                inode_no++;
        }
    }

    return 0;
}

int name_search(int fd,                                     // the disk image file descriptor 
                    int num_group,                             // how many groups
                    const struct ext2_group_desc *group,       // address of the group descriptor
                    struct ext2_super_block *super,            // address of the super block descriptor
                    int inode_no,                              // number of the inode, name of which to be searched
                    char *inode_name                           // where the name will be stored
                    ){       
        // printf("\n----searching for inode %d----\n", inode_no);
        struct ext2_inode inode;
        for(int ngroup = 0; ngroup < num_group; ngroup++){
            for(int idx = 0; idx  <= (int)super->s_inodes_per_group; idx++){
                    read_inode(fd, locate_inode_table(ngroup, group) , idx + 1, &inode, super->s_inode_size);
                    
                    if (S_ISDIR(inode.i_mode)){
                        lseek(fd, inode.i_block[0]*block_size, SEEK_SET); // if the inode is a directory inode, go to its first data block
                        // printf("in directory block: %d\n", inode.i_block[0]);
                        char buffer[1024];
                        read(fd, buffer, block_size);
                        int byte_count = 0;

                        while(byte_count < 1024){ // traverse the data block and split the directory entry
                            struct ext2_dir_entry* dentry = (struct ext2_dir_entry*) & (buffer[byte_count]);
                            // printf("%d, name: %s\n",dentry->inode, dentry->name );
                            if((int)(dentry->inode) == inode_no){ // check if current inode matches the given
                                strncpy(inode_name, dentry->name, dentry->name_len);
                                return 0;
                            }
                            int offset = 0; // offset to add to the byte count; name length needs to be 4-bytes aligned
                            if(dentry->name_len != 4){
                                offset = ( 4 - ( dentry->name_len % 4 ) );
                            }
                            byte_count = byte_count + 4 + 2 + 2 + dentry->name_len + offset; // 4 is inode number bytes, 2 is entry len, 2 is name lenth
                    }
                }
        }
    }
    return 0;
}

int data_copy(int fd, struct ext2_inode *inode, FILE* fp){
        // place the head back to start of first datablock
        int last_block = inode->i_size % block_size;
        // first calculate how many data blocks needed
        int blocks_needed = inode->i_size / block_size;
       
        char data_buf[block_size];

        if(blocks_needed >= 1){  // case when one or more than one block is needed
            
            if(blocks_needed > 12){
                blocks_needed = 12;  // copying only 12 direct blocks at this stage
            }

            for(int i = 0; i < blocks_needed; i++){
                lseek(fd, inode->i_block[i]*block_size, SEEK_SET); 
                read(fd, data_buf, block_size);
                fwrite(data_buf, 1, block_size, fp);
            }

            if(blocks_needed < 12){    
                read(fd, data_buf, last_block);
                fwrite(data_buf, 1, last_block, fp);
            }

        } else {
                lseek(fd, inode->i_block[0]*block_size, SEEK_SET); 
                read(fd, data_buf, last_block);
                fwrite(data_buf, 1, last_block, fp);
        }
        
        blocks_needed = inode->i_size / block_size; //23

        if(blocks_needed > 12){ // case when 13~268 datablock are needed (first indirect block)

            blocks_needed -= 12;
            if(blocks_needed > 256){
                blocks_needed = 256;
            }
            lseek(fd, inode->i_block[12]*block_size, SEEK_SET);   // locate where the single indirect block starts
            u_int indir_block = 0; // block number 
            read(fd, &indir_block, 4); // 633
            lseek(fd, indir_block*block_size, SEEK_SET);

            for(int i = 0; i < blocks_needed; i++){
                read(fd, data_buf, block_size);
                fwrite(data_buf, 1, block_size, fp);
            } 

            if(blocks_needed < 256){
                read(fd, data_buf, last_block);
                fwrite(data_buf, 1, last_block, fp);
            }
        }

        blocks_needed = inode->i_size / block_size;
        
        if(blocks_needed > 268){
            blocks_needed -= 268;     
            int *indir_block = malloc(4);
            int double_ind = blocks_needed / 256;
            int blocks_left = blocks_needed % 256; // blocks left that does'nt requre a full 256 data blocks
            int double_ind_cnt = 0; // counter for the first indirect level
            for( double_ind_cnt = 0; double_ind_cnt < double_ind; double_ind_cnt ++ ){
                // locate the first element of first indirect level
                lseek(fd, inode->i_block[13]*block_size + double_ind_cnt * 4, SEEK_SET);
                read(fd, indir_block, 4); 
                lseek(fd, (*indir_block)*block_size, SEEK_SET); // second level of indirect
                read(fd, indir_block, 4);  
                lseek(fd, (*indir_block)*block_size, SEEK_SET); // actual datablock

                for(int j = 0; j < 256; j++ ){
                    read(fd, data_buf, block_size);
                    fwrite(data_buf, 1, block_size, fp);
                }
            }
            // the left overs
            lseek(fd, inode->i_block[13]*block_size + (double_ind_cnt) * 4, SEEK_SET);
            read(fd, indir_block, 4);
            lseek(fd, (*indir_block)*block_size, SEEK_SET); 
            read(fd, indir_block, 4);  
            lseek(fd, (*indir_block)*block_size, SEEK_SET); 
            for(int j = 0; j < blocks_left; j++ ){
                read(fd, data_buf, block_size);
                fwrite(data_buf, 1, block_size, fp);
            }
            read(fd, data_buf, last_block);
            fwrite(data_buf, 1, last_block, fp);
        }
    return 0;
}

int write_detail(struct ext2_inode *inode, FILE* fp){

    char count[20];
    char size[20];
    char uid[20]; 

    sprintf(count, "%d", inode->i_links_count);
    sprintf(size, "%d", inode->i_size);
    sprintf(uid, "%d", inode->i_uid);

    fputs(count, fp);
    fputs("\n", fp);
    fputs(size, fp);
    fputs("\n", fp);
    fputs(uid, fp);
    
    return 0;

}