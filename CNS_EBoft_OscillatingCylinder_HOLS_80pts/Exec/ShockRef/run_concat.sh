for counter in {0..4200..10}
	do
		echo $counter
		if [[ $counter -lt 10 ]]
                then
                        append=000$counter
		elif [[ $counter -lt 100 ]]
		then
			append=00$counter
		elif [[ $counter -lt 1000 ]]
		then
        		append=0$counter
		elif [[ $counter -lt 10000 ]]
		then
        		append=$counter
		fi
		echo $counter
		dir="dir"
		dir+=$append
		pressure="pressure_surface_hist."
		skin_friction="skin_friction_surface_hist."
		pressure+=$append
		pressure+=".txt"
		skin_friction+=$append
		skin_friction+=".txt"

		cat $dir/pressure_surface.*.txt > $pressure
		cat $dir/skin_friction.*.txt > $skin_friction
     	let counter=counter+10
done
