# Project 7 - File systems
## Objectives
- To get experience with file system structure
- To get acquainted with file system utilities such as mkfs and debugfs
- To help the police catch the bad guy

In this project, you will recover all image files from an ext2 disk image. One of you should work on the content recovery of the jpg images (the first part), and the other one should work on the recovery of file details (the second part).

## Outline
1. Student Information
2. Usage Instructions
3. Implementation
4. Handin

## 1. Student Information
- Section: LEC 001
- Group 60

|           | student 1      | student 2      |
| :-------- | :------------- | :------------- |
| name      | Yu Shan Lu     | Pin Chun Lu    |
| cs login  | yul            | pin-chun       |
| wisc ID   | 9084761288     | 9084990002     |
| email     | ylu437@wisc.edu| plu49@wisc.edu |

## 2. Usage Instructions
### 2.1 Compilation and Execution
To compile the `runscan.c` file and execute it, follow these steps:

#### Project File Structure

```
.
├── disk_images/
├── jpg_images/ output examples generated from ../disk_images/image-02
├── rcheck.py # check whether jpg and text files are the same as expected format
├── README # File provided by TA with project
├── starter_code/
│   ├── ext2_fs.h
│   ├── Makefile
│   ├── read_ext2.c
│   ├── read_ext2.h
│   ├── runscan
│   ├── runscan.c # Main program file for ext2 scanner
│   └── README.md # File with additional information about this project implementation
└── test_disk_images/ # Subdirectory for a specific test case
    ├── test_0/
    │   ├── image-02 # Test case's given disk image file
    │   └── output/ # Subdirectory containing expected output jpg and text files for test case
...
```
#### Compilation and Execution
To compile the ext2 scanner program, run the following command in the `starter_code` directory:
```
$ make # compile runscan.c
$ ./runscan ../test_disk_images/image-02 output02 # to run the program with a specific disk image file, and store the output files in the ouput02 directory.
```

`make` command is equivalent to `gcc -Wall -Wextra -Werror -g runscan.c read_ext2.c -o runscan`

### 2.2 Deletion
#### Makefile
```
clean:
	rm -f $(EXEC)
```

#### Terminal
```
$ make clean
```

### 2 Testing
#### Running test cases

```
$ ./runscan ../test_disk_images/test_0/image-02 ./output_test_0
```


#### Verifying output files
```
$ python3 ../rcheck.py ./output0 /home/cs537-1/tests/P7/test_disk_images/test_0/output
```

## 3. Implementation
1. Open an img file and get related information: 
 	- Read first the super-block and group-descriptor. 

		`read_super_block(fd, 0, &super);`
    
	 - Superblock and groupblock are the same for each group, so only need to read it once and store it in an adress

		`read_group_desc(fd, 0, &group);`

2. For each group, loop through its inode table

    Each group has its own inode table. For each group, loop through its inode table
    `read_inode()`'s 2nd parameter is offset to the inode table of the group, 3rd parameter is the inode number within that group's inode table, idx + 1 bc inode number starts at 1, but idx starts at 0, the final offset will be `[(offset to the inode table of the group) + (idx + 1 - 1) * inode_size]`
    
	For example: offset to 2nd group's inode table is 2000;
    to reach the 1st inode of the group: `2000 + ((0+1)th - 1) * inode_size`
 
	- Create the jpg file with inode number (e.g. file-14.jpg)
	- Create the jpg file with actual name (e.g. indonesian-flag-extra-small.jpg)
	- Create the txt file with details (e.g. file-14-details.txt)

## 4. Handin
### Hand-in Instructions
> Hand in your source code (*.c and *.h), a Makefile, and a README file. Your Makefile should build the executable 'runscan'. If your program does not work perfectly, your README file should explain what does not work and the reason (if you know). The handin directory will be ~cs537-1/handin/P7. Only ONE member of your group needs to submit, similar to P6. Please fill out the quiz to let us know who is submitting. (from `@1777 `in the piazza)

### Files for Submission
```
1. ext2_fs.h
2. read_ext2.h
3. read_ext2.c
4. runscan.c
5. Makefile
6. README.md
```

### Hand-in Instructions
```
$ pwd 
/home/CSLOGIN/private/cs537/p7/starter_code
$ ls 
ext2_fs.h  Makefile  README.md  read_ext2.c  read_ext2.h  runscan.c 
$ cp ext2_fs.h ~cs537-1/handin/yul/P7
$ cp Makefile ~cs537-1/handin/yul/P7
$ cp README.md ~cs537-1/handin/yul/P7
$ cp read_ext2.c ~cs537-1/handin/yul/P7
$ cp read_ext2.h ~cs537-1/handin/yul/P7
$ cp runscan.c ~cs537-1/handin/yul/P7
```
