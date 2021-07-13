# Create Input Directory

## Usage

./testFile.sh diseases.txt countries.txt {num_of_records} {0/1}
./create_infiles.sh inputFile {name_of_directory} {num_of_files_per_subdir}

## Example

./testFile.sh diseases.txt countries.txt 20000 1
./create_infiles.sh inputFile inputDirectory 4

## Description

The above commands create an inputDirectory of {num_of_records} records, divided
in subdirectories (one subdirectory for each country). Every subdirectory
contains {num_of_files_per_subdir} files for each subdirectory(country).
{0/1} --> 0 corresponds to no duplicate citizenIDs, whereas 1 allows duplicates.
