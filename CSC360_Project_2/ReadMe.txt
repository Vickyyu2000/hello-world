This ReadMe file contains a descrption of data_av.c and a report
=======================================================DESCRIPTION===========================================================
data_av.c is a program that reads temperature data from files and calculates the highest and lowest temperatures 
and the average temperature for each city in both multi-threaded mode and single-threaded mode. The program uses a struct CityData 
to store the data for each city. The struct contains the name of the city, the maximum and minimum temperature, the total temperature, 
and the total number of values processed. The program also uses two additional instances of the CityData structs to keep 
track of the city with  the highest and lowest temperature overall.

This program reads max and min tempretures from 10 data files:  Charlottetown, Edmonton, Halifax,Montreal, Ottawa, 
                                                                Quebec, Toronto, Vancouver, Victoria and, Winnipeg. 
All data files are assumed to be in the same directory as the executable file.
The files are named as: <city name>.dat and formatted as:
    o The first line contains a header line describes the subsequent data columns. This line should be ignored.
    o The subsequent lines are formatted as <max_temp>\t<min_temp>

In function process_file, it ignores the header line and only reads lines that are in form <max>\t<min>. Any empty lines 
or lines with only one temperature will be ignored and will not be counted towards the total values processed for the 
corresponding city or any variable of the CityData struct. Each valid line is considered a processed value; in other 
words, the total number of values processed is equal to the total number of valid lines in the file.


Multi-thread Mode
    "$ ./data_av -m"
    This mode uses mutex locks to ensure that only one thread can access a shared variable city_data at a time and print the output.

Single-thread Mode
    "$ ./data_av"
    In this mode, each city's weather data is processed sequentially using the single_thread function.
=============================================================================================================================

==========================================================REPORT=============================================================
· Execution time in clock(use clock() function) for each mode
    results of 10 runs in multi-thread mode: 
        290217
        290830
        289634
        287042
        285741
        291563
        281141
        297897
        301814
        284531
    average: 290041 clocks

    results of 10 runs in single-thread mode: 
        179141
        180277
        181202
        179557
        177986
        165961
        175767
        173092
        176133
        168682
    average: 175779.8 clocks

    observation:
        Based on the execution time results, it appears that the multi-threaded mode took longer 
        on average, with an average execution time of 290041 clocks compared to 175779.8 clocks 
        for the single-threaded mode. This suggests that the locks used in the multi-threaded mode 
        may have caused the execution to fallback as a serialized execution rather than parallel 
        execution, which would explain the longer execution times.

· Synchronization issues and your recommended solution with a comparaion results(with or without applying your solution)
    race condition:
        This program has multiple threads accessing shared data (city_data, max_temp_city, and 
        min_temp_city). Specifically, each thread can potentially modify the same shared data 
        simultaneously, leading to race conditions. For example, if two threads try to update 
        the value of max_temp_city at the same time, they might read the current value of 
        max_temp_city, update it based on their own data, and write the new value back to 
        max_temp_city, effectively overwriting each other's changes.
    Inconsistent Data:
        Another issue is the potential for data inconsistency when accessing the total_temp and 
        total_value fields of the CityData struct. If multiple threads try to update these fields 
        at the same time, they might read an inconsistent value and modify it based on their own data, 
        leading to incorrect results.
    Output interleaving:
        When multiple threads are trying to write to the same output stream, their outputs can get 
        mixed up, resulting in unexpected and potentially incorrect output. This happens because each 
        thread is executing independently and the scheduler can switch between them at any time. For 
        example:
            ==============================
            Data for:Victoria
            Victoria's lowest temp is: -15.6 degrees Celsius
            ==============================
            Data for:Halifax
            Halifax's lowest temp is: -26.5 degrees Celsius
            Halifax's highest temp is: 34.3 degrees Celsius
            The avg temp for Halifax is: 7.07244
            Total values processed for Halifax are: 30471
            Victoria's highest temp is: 39.4 degrees Celsius
            The avg temp for Victoria is: 9.86853
            Total values processed for Victoria are: 31000

    My solution: 
        race condition -> mutex lock:
            The mutex lock ensures that only one thread can access the shared data at a time, 
            which prevents conflicts and data corruption.
        Inconsistent Data -> parallel programming:
            I creates 10 threads and let each one thread handle one city and ensure that each 
            thread has exclusive access to its corresponding city data, which prevents inconsistent 
            data.
        Output interleaving -> mutex lock:
            The mutex lock synchronize the output to the console or file by letting only one thread 
            writes to the console.

· Bonus: Test your program on different OS like: Linux, Mac OS and Windows. Report the number of CPUs/Core for each system
    Windows (4 cores)
        results of 10 runs in multi-thread mode: 
            123
            124
            138
            138
            124
            144
            143
            146
            137
            173
        average: 139 clocks
        
        result of 10 runs in single-thread mode:
            399
            417
            414
            430
            421
            408
            413
            427
            437
            435
        average:  420.1 clocks

        Oberservation:
            This result is really surprising. Not only is the overall speed on quad-core Windows 
            much faster in multi-threaded mode than in single-threaded mode, but also the opposite 
            on Linux when using SSH.