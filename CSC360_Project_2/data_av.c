#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define NUM_CITIES 10
void single_thread(int city_index);
void* process_file(void* arg);
typedef struct
{
    double max_temp;
    double min_temp;
    char city_name[20];
    double total_temp;
    int total_value;
} CityData;

CityData city_data[NUM_CITIES];
CityData max_temp_city;
CityData min_temp_city;
pthread_mutex_t lock;
pthread_mutex_t print;

int main(int argc, char const *argv[])
{
    clock_t start, end;
    start = clock();

    // Initialize city data
    strcpy(city_data[0].city_name, "Charlottetown");
    strcpy(city_data[1].city_name, "Edmonton");
    strcpy(city_data[2].city_name, "Halifax");
    strcpy(city_data[3].city_name, "Montreal");
    strcpy(city_data[4].city_name, "Ottawa");
    strcpy(city_data[5].city_name, "Quebec");
    strcpy(city_data[6].city_name, "Toronto");
    strcpy(city_data[7].city_name, "Vancouver");
    strcpy(city_data[8].city_name, "Victoria");
    strcpy(city_data[9].city_name, "Winnipeg");

    for (int i = 0; i < NUM_CITIES; i++)
    {
        city_data[i].max_temp = -1000;
        city_data[i].min_temp = 1000;
        city_data[i].total_temp = 0;
        city_data[i].total_value = 0;
    }
    max_temp_city.max_temp = -1000;
    min_temp_city.min_temp = 1000;
    
    if (argc > 1 && strcmp(argv[1], "-m") == 0)
    {   //multi-thread here 

        // Initialize mutex lock
  
        pthread_mutex_init(&lock, NULL);
        pthread_mutex_init(&print, NULL);

        // Create threads for each city
        pthread_t threads[NUM_CITIES];
        int city_index[NUM_CITIES];

        for (int i = 0; i < NUM_CITIES; i++) {
            city_index[i] = i;
            int err = pthread_create(&threads[i], NULL, process_file, (void*)&city_index[i]);
            if (err != 0){
                printf("Error creating thread for city %d\n", i);
            }
        }     

        // Wait for all threads to complete
        for (int i = 0; i < NUM_CITIES; i++)
        {
            pthread_join(threads[i], NULL);
        }

        int total = 0;
        for (int i = 0; i < NUM_CITIES; i++)
        {
            total += city_data[i].total_value;
        }

        pthread_mutex_destroy(&lock);
        pthread_mutex_destroy(&print);

        printf("==============================\n");
        printf("Total values evaluated is: %d\n", total);
        printf("The lowest temp overall is: %.1lf reported in: %s\n", min_temp_city.min_temp, min_temp_city.city_name);
        printf("The highest temp overall is: %.1lf reported in: %s\n", max_temp_city.max_temp, max_temp_city.city_name);
        end = clock();
        printf("Elapsed time: %ld clocks\n", end-start);
        exit(0);
    }

    for (int i = 0; i < NUM_CITIES; i++){
        single_thread(i);
    }
    int total = 0;
    for (int i = 0; i < NUM_CITIES; i++)
    {
        total += city_data[i].total_value;
    }
    printf("==============================\n");
    printf("Total values evaluated is: %d\n", total);
    printf("The lowest temp overall is: %.1lf reported in: %s\n", min_temp_city.min_temp, min_temp_city.city_name);
    printf("The highest temp overall is: %.1lf reported in: %s\n", max_temp_city.max_temp, max_temp_city.city_name);
    end = clock();
    printf("Elapsed time: %ld clocks\n", end-start);
   
}

void *process_file(void *arg)
{
    int *p_city_index = (int*) arg;
    int city_index = *p_city_index;

    char filename[50];
    sprintf(filename, "%s.dat", city_data[city_index].city_name);
    
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        pthread_exit(NULL);
    }

    // Ignore header line
    char line[100];
    fgets(line, sizeof(line), file);
    
    while(fgets(line, sizeof(line), file))
    {
        double max_temp, min_temp;

        //if input not match then jump to read next line
        if (sscanf(line, "%lf\t%lf", &max_temp, &min_temp) != 2) continue;
        
        //each line is considered a "value" for total values processed
        city_data[city_index].total_value++; 
        city_data[city_index].total_temp += (max_temp+min_temp);
        if(max_temp > city_data[city_index].max_temp)
        {
            city_data[city_index].max_temp = max_temp;
        }
        if(min_temp < city_data[city_index].min_temp)
        {
            city_data[city_index].min_temp = min_temp;
        }
        pthread_mutex_lock(&lock);
        if(max_temp > max_temp_city.max_temp)
        {
            max_temp_city.max_temp = max_temp;
            memset(max_temp_city.city_name, 0, sizeof(max_temp_city.city_name));
            strcpy(max_temp_city.city_name, city_data[city_index].city_name);
        }
        if(min_temp < min_temp_city.min_temp)
        {
            min_temp_city.min_temp = min_temp;
            memset(min_temp_city.city_name, 0, sizeof(min_temp_city.city_name));
            strcpy(min_temp_city.city_name, city_data[city_index].city_name);
        }
        pthread_mutex_unlock(&lock);
        
    }
    pthread_mutex_lock(&print);
    printf("==============================\n");
    printf("Data for:%s\n", city_data[city_index].city_name);
    printf("%s's lowest temp is: %.1lf degrees Celsius\n", city_data[city_index].city_name, city_data[city_index].min_temp);
    printf("%s's highest temp is: %.1lf degrees Celsius\n", city_data[city_index].city_name, city_data[city_index].max_temp);
    printf("The avg temp for %s is: %.5f\n", city_data[city_index].city_name, city_data[city_index].total_temp/(city_data[city_index].total_value*2));
    printf("Total values processed for %s are: %d\n", city_data[city_index].city_name, city_data[city_index].total_value);
    pthread_mutex_unlock(&print);

    fclose(file);
    pthread_exit(NULL);
}

void single_thread(int city_index)
{

    char filename[50];
    sprintf(filename, "%s.dat", city_data[city_index].city_name);
    
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filename);
    }

    // Ignore header line
    char line[100];
    fgets(line, sizeof(line), file);
    
    while(fgets(line, sizeof(line), file))
    {
        double max_temp, min_temp;

        //if input not match then jump to read next line
        if (sscanf(line, "%lf\t%lf", &max_temp, &min_temp) != 2) continue;
        
        //each line is considered a "value" for total values processed
        city_data[city_index].total_value++; 
        
        city_data[city_index].total_temp += (max_temp+min_temp);
        if(max_temp > city_data[city_index].max_temp)
        {
            city_data[city_index].max_temp = max_temp;
        }
        if(min_temp < city_data[city_index].min_temp)
        {
            city_data[city_index].min_temp = min_temp;
        }

        if(max_temp > max_temp_city.max_temp)
        {
            max_temp_city.max_temp = max_temp;
            memset(max_temp_city.city_name, 0, sizeof(max_temp_city.city_name));
            strcpy(max_temp_city.city_name, city_data[city_index].city_name);
        }
        if(min_temp < min_temp_city.min_temp)
        {
            min_temp_city.min_temp = min_temp;
            memset(min_temp_city.city_name, 0, sizeof(min_temp_city.city_name));
            strcpy(min_temp_city.city_name, city_data[city_index].city_name);
        }

        
    }

    printf("==============================\n");
    printf("Data for:%s\n", city_data[city_index].city_name);
    printf("%s's lowest temp is: %.1lf degrees Celsius\n", city_data[city_index].city_name, city_data[city_index].min_temp);
    printf("%s's highest temp is: %.1lf degrees Celsius\n", city_data[city_index].city_name, city_data[city_index].max_temp);
    printf("The avg temp for %s is: %.5f\n", city_data[city_index].city_name, city_data[city_index].total_temp/(city_data[city_index].total_value*2));
    printf("Total values processed for %s are: %d\n", city_data[city_index].city_name, city_data[city_index].total_value);


    fclose(file);
}