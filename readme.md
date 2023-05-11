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



*For a working draft of the project's outline and pseudocode, see `reqs.md`.*

@juandapl, @NicholasRaffone, 

27/04/2023
