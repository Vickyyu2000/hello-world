#!/bin/bash

for i in {01..15} ; do
	echo tests/in$i.txt

	./driver-new.py --in "tests/in$i.txt" --out "new-test-result/new-out$i.txt"

	diff "tests/out$i.txt" "new-test-result/new-out$i.txt" >/dev/null
	


	if [ $? -eq 0 ]; then
		echo "test $i passed"
	else
		echo "**** failed test $i ****"
	fi


done
