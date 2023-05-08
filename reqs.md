## Requirements

FileNode: NO/ just literally 
- Metadata
    - Name
    - Type
    - Size
    - Owner
    - Group
    - Rights
    - File or dir?
- pointer to data
- filenode* of children

### Maybe organization
- Header
    - n_objs
    - data-start (offset)
- Data Area
    - all identified by offsets
    - each object:
        - metadata (fixed size)
        - data (variable size)
        We read x bytes as specified in metadata
    OR special END DIR token, which has data 0 and just cds to parent.

flatten everything in this way:
    1   dir root( dir 1, 2, 3 dir, 5, 6, end dir,  4, end dir, 5) end dit
    /
    2 3 dir  4 
        5 6

        /1 2 3 /1/3 5 6 /1 4 /

Links
- case 1 
    - if its a link, the data area tells u what it links to
    - sys call that tells u how many links to a file. have a lil vector of inode ids that are links (or inode ids)
        - if 2+ links
            - 1st occurrence, file dnf on vector: do as normal file
            - 2nd occurrence, file found on vector 

### functionally

1 thing
- a function that takes the root node of the file structure and flat writes it to a file
- a function that takes a file and reads it into a file structure

2 actually getting files in
- a function that takes a dir and writes to the file structure
- a function that takes a file and writes to the file structure

3 ls function in a dena readable format

4 append function

5 manage links

### structure of the zip file
we need a current path of the program

we always start dir root

adzip [] files
file: metadata (samesies), nbytes, data[nbytes]
specials: dir, doesnt have data, just has metadata - when extracting: make dir, enter dir (append dir name to path)
        end dir - when extracting: pop from path
