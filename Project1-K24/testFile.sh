#!/bin/bash

# An dothoyn ligotera apo 4 orismata einai sfmala
if [[ $# -lt 4 ]] 
then

	echo "Number of arguments less than 4. Must be 4."
	exit

# An dothoyn perissotera apo 4 orismata einai sfmala
elif [[ $# -gt 4 ]] 
then

	echo "Number of arguments more than 4. Must be 4."
	exit

# An den mporei na anoixtei to arxeio iwn einai sflama.
elif [[ ! ( -f $1 ) ]]
then

	echo "Virus file not exists or access denied."
	exit

# An den mporei na anoixtei to arxeio xwrwn einai sflama.
elif [[ ! ( -f $2 ) ]]
then
	
	echo "Country file not exists or access denied."
	exit

# An to plhthos twn grammwn den einai akeraios einai sflama.
elif ! ( [ -n "$3" ] && [ "$3" -eq "$3" ] 2>/dev/null )
then

	echo "Number of lines not an integer."
	exit


# An h apofash diplwn id den einai akeraios einai sflama.
elif ! ( [ -n "$4" ] && [ "$4" -eq "$4" ] 2>/dev/null )
then

	echo "Douplicates allow decision not an integer."
	exit
	
else
	
	# Exontas kanei toys basikoys elegxoys eksetazoyme to periexomeno twn arxeiwn
	# ka to plhthos twn grammwn
		
	# Oi ioi antigrafontai se pinaka
	readarray -t arrayOfViruses < $1
	
	# Ypologizetai kai to plithos toys	
	numberOfViruses=${#arrayOfViruses[@]}

	# An einai mideniko einai sflama.
	if [[ $numberOfViruses -eq 0 ]]
	then
	
		echo "File of viruses empty."

		exit
		
	fi

	# Oi xwres antigrafontai se pinaka
	readarray -t arrayOfCountries < $2
	
	# Ypologizetai kai to plithos toys	
	numberOfCountries=${#arrayOfCountries[@]}
	

	# An einai mideniko einai sflama.	
	if [[ $numberOfCountries -eq 0 ]]
	then
	
		echo "File of countries empty."

		exit
		
	fi

	# Epishs an to plithos twn grammwn einai mideniko einai sfalma
	
	if [[ $3 -le 0 ]]
	then
	
		echo "Number of lines supposed to be positive."

		exit
		
	fi
	
fi
	
rm -f inputFile

# Periptwsh sthn opoia den 8eloume dipla id 
if [[ $4 -eq 0  ]]
then
	
	# Ftiaxnoyme enan pinaka apo shmaies
	# Ka8e fora poy epilegetai o antistoixos akeraios h shmaia ginetai 1
	for (( i=0; i<10000; i++ ))
	do
	
		chosen[$i]=0
	
	done
	
	# Ftiaxnoyme mia mia tis grammes kai tis grafoume sto arxeio
	for (( i=0; i<$3; i++ ))
	do
	
		# Epilegoyme tyxaio citizenId
		citizenId=$(($RANDOM%10000))
		
		# An h shmaia epiloghs toy einai ish me 0, "egkrinetai" kai h shmaia toy ginetai 1
		if [[ ${chosen[$citizenId]} -eq 0 ]]
		then
		
			chosen[$citizenId]=1
			
		else
		# Diaforetika psaxnoyme to amesws epomeno mh epilegmeno		
		
			while [[ ${chosen[$citizenId]} -eq 1 ]]
			do
						
				citizenId=$(($citizenId+1))
				
				# An ftasoyme sto telos toy pinaka shmaiwn kai den exei vre8ei mh epilegmeno citizenId
				# ksekiname pali apo thn prwth thesh
				if [[ $citizenId -eq 10000 ]]
				then
				
					$citizenId=0
				fi
								
			done
			
			# Apo th diadikasia prokyptei mh epilegmeno citizenId
			# To epilegoyme ypswnontas thn antistoixh shmaia			
			chosen[$citizenId]=1
			
		
		fi
		
				# Epilegetai to plh8os grammatwn toy onomatos
		nameNumOfLetters=$((3+$(($RANDOM%10))))
	
		# Epilegetai to onoma
		name=$(tr -dc a-z </dev/urandom | head -c $nameNumOfLetters)

		# Epilegetai to plh8os grammatwn toy epwnimoy
		lastNameNumOfLetters=$((3+$(($RANDOM%10))))
		
		# Epilegetai ari8modeikths xwras
		countryIndex=$(($RANDOM%numberOfCountries))
		
		# Epilegetai xwra
		country=${arrayOfCountries[$countryIndex]}		
		
		# Epilegetai to epwnimo
		lastName=$(tr -dc a-z </dev/urandom | head -c $lastNameNumOfLetters)
		
		# Epilegetai hlikia
		age=$(($RANDOM%120+1))			
		
		# Epilegetai ari8modeikths iou
		virusIndex=$(($RANDOM%numberOfViruses))
		
		# Epilegetai ios
		virus=${arrayOfViruses[$virusIndex]}
				
		# Se o ti afora toys emboliasmoys 8ewroyme oti an ena atomo einai katw apo 20 etwn exei 25% pithanothta
		# na exei emboliastei, an einai apo 20 mexri 50 exei 50% pithanothta na exei emvoliastei enw an einai
		# panw apo 50 exei 75% pithanothta na exei emboliastei		
		if [[ $age -lt 20 ]]
		then
			
			if [[ $(($RANDOM%4)) -eq 0 ]]
			then
			
				vaccineDone=1
				
			else
			
				vaccineDone=0
				
			fi
			
		elif [[ $age -lt 50 ]]
		then
			
			if [[ $(($RANDOM%4)) -lt 2 ]]
			then
			
				vaccineDone=1
				
			else
			
				vaccineDone=0
				
			fi
		
		else

			if [[ $(($RANDOM%4)) -lt 2 ]]
			then
			
				vaccineDone=1
				
			else
			
				vaccineDone=0
				
			fi
		
		fi
		
		# Edw dhmioyrgeitai kai grafetai h grammh
		if [[ $vaccineDone -eq 0 ]]
		then
		
		# Periptwsh sthn opoia to atomo den exei kanei emvolio
		
			echo $citizenId $name $lastName $country $age $virus "NO" >> inputFile
			
		else
		
			# Periptwsh sthn opoia to atomo exei kanei emvolio
			
			# Epilegetai hmera
			day=$(($RANDOM%30+1))
			
			# An einai monopshfia symplhrwnoyme to 0 aristera 
			if [[ $day -lt 10 ]]
			then
			
				day="0"$day
				
			fi
			
			# Epilegetai mhnas
			month=$(($RANDOM%12+1))
			
			# An einai monopshfios symplhrwnoyme to 0 aristera 
			if [[ $month -lt 10 ]]
			then
			
				month="0"$month
				
			fi
			
			# Epilegetai etos eite 2020 eite 2021
			year=$(($RANDOM%2+2020))
											
			echo $citizenId $name $lastName $country $age $virus "YES" $day"-"$month"-"$year  >> inputFile
		
		
		fi
	
	done
	

else

	for (( i=0; i<$3; i++ ))
	do
		
		# To deytero epilegmeno citizenId 8a einai iso me to prwto apla gia na eimaste sigoyroi
		# oti 8a exoyme toylaxiston mia epanalhpsh citizenId.
		# Apo ekei kai pera ana 100 citizenId swzetai to citizenId poy epilegetai se mia metablhth kai
		# 99 gyroys meta epanalamvanetai.
		# Konta se aytes tis epanalhpseis mporei na exoyme ki alles tyxaies.
		if [[ $(($i%100)) -eq 0 ]]
		then
		
			citizenId=$(($RANDOM%10000))
			duplicateCitizenId=$citizenId
		
		elif [[ $(($i%100)) -eq 99 ]]
		then
		
			citizenId=$duplicateCitizenId
			
		elif [[ $i -ne 1 ]]
		then
		
			citizenId=$(($RANDOM%10000))
		
		fi
		
		# Epilegetai to plh8os grammatwn toy onomatos
		nameNumOfLetters=$((3+$(($RANDOM%10))))
	
		# Epilegetai to onoma
		name=$(tr -dc a-z </dev/urandom | head -c $nameNumOfLetters)

		# Epilegetai to plh8os grammatwn toy epwnimoy
		lastNameNumOfLetters=$((3+$(($RANDOM%10))))
		
		# Epilegetai ari8modeikths xwras
		countryIndex=$(($RANDOM%numberOfCountries))
		
		# Epilegetai xwra
		country=${arrayOfCountries[$countryIndex]}		
		
		# Epilegetai to epwnimo
		lastName=$(tr -dc a-z </dev/urandom | head -c $lastNameNumOfLetters)
		
		# Epilegetai hlikia
		age=$(($RANDOM%120+1))			
		
		# Epilegetai ari8modeikths iou
		virusIndex=$(($RANDOM%numberOfViruses))
		
		# Epilegetai ios
		virus=${arrayOfViruses[$virusIndex]}
				
		
				# Se o ti afora toys emboliasmoys 8ewroyme oti an ena atomo einai katw apo 20 etwn exei 25% pithanothta
		# na exei emboliastei, an einai apo 20 mexri 50 exei 50% pithanothta na exei emvoliastei enw an einai
		# panw apo 50 exei 75% pithanothta na exei emboliastei		
		if [[ $age -lt 20 ]]
		then
			
			if [[ $(($RANDOM%4)) -eq 0 ]]
			then
			
				vaccineDone=1
				
			else
			
				vaccineDone=0
				
			fi
			
		elif [[ $age -lt 50 ]]
		then
			
			if [[ $(($RANDOM%4)) -lt 2 ]]
			then
			
				vaccineDone=1
				
			else
			
				vaccineDone=0
				
			fi
		
		else

			if [[ $(($RANDOM%4)) -lt 2 ]]
			then
			
				vaccineDone=1
				
			else
			
				vaccineDone=0
				
			fi
		
		fi
		
		# Edw dhmioyrgeitai kai grafetai h grammh
		if [[ $vaccineDone -eq 0 ]]
		then
		
		# Periptwsh sthn opoia to atomo den exei kanei emvolio
		
			echo $citizenId $name $lastName $country $age $virus "NO" >> inputFile
			
		else
		
		# Periptwsh sthn opoia to atomo exei kanei emvolio
			
			# Epilegetai hmera
			day=$(($RANDOM%30+1))
			
			# An einai monopshfia symplhrwnoyme to 0 aristera 
			if [[ $day -lt 10 ]]
			then
			
				day="0"$day
				
			fi
			
			# Epilegetai mhnas
			month=$(($RANDOM%12+1))
			
			# An einai monopshfios symplhrwnoyme to 0 aristera 
			if [[ $month -lt 10 ]]
			then
			
				month="0"$month
				
			fi
			
			# Epilegetai etos eite 2020 eite 2021
			year=$(($RANDOM%2+2020))
											
			echo $citizenId $name $lastName $country $age $virus "YES" $day"-"$month"-"$year  >> inputFile
		
		
		fi
	
	done
	
fi
