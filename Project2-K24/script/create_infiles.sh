#!/bin/bash

# An dothoyn ligotera apo 3 orismata einai sfmala
if [[ $# -lt 3 ]] 
then

	echo "Number of arguments less than 3. Must be 3."
	exit

# An dothoyn perissotera apo 3 orismata einai sfmala
elif [[ $# -gt 3 ]] 
then

	echo "Number of arguments more than 3. Must be 3."
	exit

# An den mporei na anoixtei to arxeio eggrafwn einai sflama.
elif [[ ! ( -f $1 ) ]]
then

	echo "Input file does not exist or access denied."
	exit
# An to plhthos twn arxeiwn ana directory den einai akeraios einai sflama.
elif ! ( [ -n "$3" ] && [ "$3" -eq "$3" ] 2>/dev/null )
then

	echo "Number of files per directory not an integer."
	exit
# An to plhthos twn arxeiwn ana directory einai mideniko einai sfalma
elif [[ $3 -le 0 ]]
then

	echo "Number of files per directory supposed to be positive."

	exit
	
fi

# An to input_dir yparxei einai sfalma, alliws dhmimoyrgeitai.
if [[ -d $2 ]]
then

    echo "Input directory already exists"
    exit

else
	mkdir $2
fi


# Ta citizen records antigrafontai se pinaka
readarray -t arrayOfEntries < $1
# mapfile -t arrayOfEntries < $1


# Ypologizetai kai to plithos toys
numberOfEntries=${#arrayOfEntries[@]}

# An einai mideniko einai sflama.
if [[ $numberOfEntries -eq 0 ]]
then

	echo "Input File empty."

	exit
	
fi

# Oi diaforetikes xwres antigrafontai se pinaka
arrayOfCountries=($(cut -d " " -f 4  inputFile | sort -u))

# Ypologizetai kai to plithos toys
numberOfCountries=${#arrayOfCountries[@]}

for (( i=0; i<$numberOfCountries; i++ ))
do
	# echo ${arrayOfCountries[i]}
	mkdir "$2/${arrayOfCountries[i]}"
	for (( j=0; j<$3; j++ ))
	do
		touch "$2/${arrayOfCountries[i]}/${arrayOfCountries[i]}-$j.txt"
	done

	# echo $var
done