# Abu Dhabi / Alex Delis Tar (adtar)
### Created by Juan Piñeros and Nicholas Raffone

#### adtar archives filesystem structures into a convenient, easy to handle .ad file, and reconstructs them wherever you want.

## Usage

### Compilation
Run the command `make` to assemble all files

### Removing files
Run the command `make clean` to delete all files

### Running the program

#### Create a new archive file
Run `./adtar -c {name_of_archived_file} {file_or_directory_to_archive} [{another file/dir} ...]` to archive a file or directory. The program will create a file with the name specified in the current directory. To archive multiple files/directories in the same file, provide a list of space separated files/directories to archive.

#### Append to an existing archive file
Run `./adtar -a {name_of_archived_file} {file_or_directory_to_archive} [{another file/dir} ...]` to append the files specified to the existing archive file. The program will append the files to the end of the archive file.

#### Print file hierarchy
Run `./adtar -p {name_of_archived_file}` to print the hierarchy of files in the archive file. 
Run `./adtar -m {name_of_archived_file}` to print the hierarchy including metadata and permissions information.

#### Extract files
Run `./adtar -x {name_of_archived_file}` to extract all the files in the archive file to the current directory. 
Run `./adtar -x {name_of_archived_file} [{file_or_directory_to_extract} ...]` to extract only specific files/directories. To extract multiple, provide a space/separated list of files/directories to extract.

## Design Choices

### File System Structure
In order to ease the implementation and have less overhead, we decided to flatten the file system as a tree traversal. The archived file is an array of file objects, which are defined as follows:

- Metadata: `struct MetadataField` (of fixed size)
    - File name
    - File type (`T_FILE (0), T_LINK (1), T_DIR (2),` or `T_CLOSE (3)`)
    - File permissions
    - File size
    - File owner
    - File group
- File contents: `void*` (of variable size `MetadataField.size`)

Only `FILE` type files have contents and a variable size. The other types are defined as follows:
    - `T_LINK`: `MetadataField.size` is an `int`, which stores the `inode` number it is linked to (more on this later)
    - `T_DIR`: `MetadataField.size` is `0`
    - `T_CLOSE`: `MetadataField.size` is `0`, and is a special token that indicates the end of a directory.

### Tree traversal
We traverse the file tree depth-first, which makes recursive extraction easier. When we reach the end of a directory, we add a `T_CLOSE` token to the file array. This token indicates that the directory has ended, and that we should return to the parent directory.

#### Example
Consider the following sequence of files and directories:

`[dir 1, file 1, file 2, dir 2, file 3, file 4, end dir, file 5, end dir, file 6]`.

Actually, what our `T_CLOSE` tokens do is make this more understandable, both to us and to our parser:

`[dir 1, `(`file 1, file 2, dir 2, `(`file 3, file 4, end dir`)`, file 5, end dir`)`, file 6]`.

This generates a file structure that looks like this:
```
dir 1
    file 1
    file 2
    dir 2
        file 3
        file 4
file 5
```

#### Replication
This system allows us to reconstruct a file structure using a simple algorithm:
```
while there are still bytes to read in the archive:
    read a MetadataField worth of bytes (it is always the same size)
    if file:
        read MetadataField.size bytes
        flush to file
    else if dir:
        create dir
        cd to dir
    else if link:
        read an int
        create a link to the inode # specified by the int
    else if end dir:
        cd to parent
```
We also replicate the owner id, group id, and permissions encountered when building the archive file.

### Link management
Hard links are links to inodes in the file system. In practice, they are indistinguishable from regular files. However, we need to keep track of them in order to avoid duplicating files when creating the file and extracting.

We consider three cases:
- The file is a link, but all other references are outside the scope of the archive file. In this case, we treat the file as a regular file.
- The file is a link, but it is the first appearance of the inode in the scope of the archive. In this case, we treat the file as a regular file.
- The file is a link, but its inode number already appears in the archive. In this case, we treat the file as a link to the inode number specified in the file.

#### Building the archive file
When building the file, we keep an array of `inodes` already encountered. This is an array of `struct INodeArrayElem{unsigned long inodeval; char filepath[1024];}` Every time we encounter a file, we check whether its inode number is already in the array. If it is, we add a `T_LINK` token to the file array, which indicates that the file is a link to another file. The `T_LINK` token contains the index of the `inode` in the array.

#### Extracting from the archive file
Notice that we extract the files in the same order we added them to the archive file. This means that we can keep track of the `inodes` we have already encountered in the same way we did when building the archive file. When we encounter a `T_LINK` token, we simply create a link to the `filepath` specified by the index number in the array.

### Other operations

#### Appendage
Our structure makes it very easy to append files to the archive. We simply add the new files/structures to the end of the file array.

#### Selective extraction
The only drawback of our linear flattening approach is that in the worst case scenario, the whole tree has to be traversed in order to extract a single file. 

However, this is not a problem for directories, given that they can be recursively extracted. We just pay the time delay of traversing the whole archive once, and then we can extract all files in the directory by traversing the archive linearly until finding the respective `T_CLOSE` token.

@juandapl, @NicholasRaffone, 

27/04/2023
