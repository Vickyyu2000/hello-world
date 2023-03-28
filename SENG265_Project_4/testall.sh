#!/bin/bash

for TESTNUM in {01..15} ; do
	echo tests/in${TESTNUM}.txt

	cat tests/in${TESTNUM}.txt | ./driver-original.py > tests/concord_out${TESTNUM}.txt

	CONCORD_EXIT=$?

	OUTPUT=$(diff tests/concord_out${TESTNUM}.txt tests/out${TESTNUM}.txt)

	if [[ $CONCORD_EXIT -ne 0 ]]; then
		echo -e "\e[31mFAiL\e[0m - Concord failed to exit successfully (exit value $CONCORD_EXIT)"
	elif [[ -n $OUTPUT ]]; then
		echo -e "\e[31mFAiL\e[0m - Expected text different from actual text"
	else
		echo -e "\e[32mPASS\e[0m"
	fi
	echo ""

done
