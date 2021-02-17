files=($(find $1*))

avg=0
for o in "${files[@]}"
do
  buf=$(grep -Eo '[0-9.]+ seconds' $o | tail -1 | sed -e "s/\ seconds//")
  buf=${buf:="-1"}
  
  if [ $buf != "-1" ]
  then
    echo $o: $buf
    avg=$(echo "$buf + $avg" | bc -l)        
  fi
done

avg=$(echo "$avg / ${#files[@]}" | bc -l)

echo Extracted time: $avg
