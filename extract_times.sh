files=($(find $1*))

avg=0
for o in "${files[@]}"
do
    buf=$(grep -Eo '[0-9.]+ seconds' $o | tail -1 | sed -e "s/\ seconds//")
    avg=$(echo "$buf + $avg" | bc -l)
done

avg=$(echo "$avg / ${#files[@]}" | bc -l)

echo Extracted time: $avg
