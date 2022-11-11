#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

pthread_mutex_t mutex_mapper;
pthread_mutex_t mutex_reducer;

struct my_mapper
{
    int id;
    int complete;
    int number_of_reducer;
    int number_of_mapper;
    int number_of_in_files;
    int *number_of_element_in_every_array;
    int **array;
    char **in_file;
};

struct my_reducer
{
    int id;
    int number_of_mapper;
    struct my_mapper *mapper;
};

int calc_pow(int x, int y)
{
    int value = 1;
    for (size_t i = 0; i < y; i++)
    {
        value = value * x;
    }
    return value;
}

int check_root(int n, int y)
{
    if (n == 1 || n == 0)
    {
        return 1;
    }

    // approach 1
    for (size_t i = 2; i <= sqrt(n); i++)
    {
        if (n == calc_pow(i, y))
        {
            return 1;
        }
    }
    return 0;
}

int getNthRoot(double m, int n)
{
    double low = 1.0;
    double high;
    if (n == 2)
    {
        high = sqrt(m);
    }
    if (n == 3)
    {
        high = cbrt(m);
    }
    if (n >= 4)
    {
        high = sqrt(sqrt(m));
    }

    while ((high - low) > 0.01)
    {
        double mid = (low + high) / 2.0;
        if (pow(mid, n) < m)
        {
            low = mid;
        }
        else
        {
            high = mid;
        }
    }

    double checkValue = ceil(low);
    if (pow(checkValue, n) == m)
    {
        return 1;
    }
    return 0;
}

int check_complete(struct my_reducer reducer)
{
    for (size_t i = 0; i < reducer.number_of_mapper; i++)
    {
        if (reducer.mapper[i].complete == 0)
        {
            return 0;
        }
    }
    return 1;
}

int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int count_distinct_adv(int *arr, int n)
{
    int res = 1;

    for (int i = 1; i < n; i++)
    {
        if (arr[i - 1] != arr[i])
        {
            res++;
        }
    }
    return res;
}

int count_distinct(int *arr, int n)
{
    int res = 1;

    for (int i = 1; i < n; i++)
    {
        int j = 0;
        for (j = 0; j < i; j++)
            if (arr[i] == arr[j])
                break;

        if (i == j)
            res++;
    }
    return res;
}

char **get_args(int argc, char **argv, int *M, int *R, int *number_of_in_files)
{
    *M = atoi(argv[1]);
    *R = atoi(argv[2]);

    char str[50];

    FILE *ptr;

    ptr = fopen(argv[3], "r+");

    if (ptr == NULL)
    {
        printf("file can't be opened \n");
    }

    fgets(str, 50, ptr);
    *number_of_in_files = atoi(str);

    char **in_file = (char **)malloc(sizeof(char *) * (*number_of_in_files));
    for (size_t i = 0; i < (*number_of_in_files); i++)
    {
        in_file[i] = (char *)malloc(sizeof(char) * 20);
    }

    int i = 0;
    while (fgets(in_file[i], 50, ptr) != NULL)
    {
        in_file[i][strlen(in_file[i]) - 1] = '\0';
        i++;
    }
    fclose(ptr);
    return in_file;
}

void *f_map(void *arg)
{
    struct my_mapper *data = (struct my_mapper *)arg;
    for (int i = data->id; i < data->number_of_in_files; i = i + data->number_of_mapper)
    {
        FILE *ptr;
        ptr = fopen(data->in_file[i], "r+");
        char str[20];
        fgets(str, 50, ptr);

        // read number from file
        while (fgets(str, 20, ptr) != NULL)
        {
            char *ptr;
            double number;

            number = strtod(str, &ptr);
            for (int j = 2; j < data->number_of_reducer + 2; j++)
            {
                if (getNthRoot(number, j) == 1)
                {
                    data->array[j - 2][data->number_of_element_in_every_array[j - 2]] = number;
                    data->number_of_element_in_every_array[j - 2]++;
                }
            }
        }
        fclose(ptr);
    }
    data->complete = 1;
    pthread_exit(NULL);
}

void *f_reduce(void *arg)
{
    struct my_reducer *reducer = (struct my_reducer *)arg;

    // check if all mapper complete
    while (check_complete(*reducer) == 0)
    {
    }

    // access every mapper to calculate total element for current reducer
    int total_element = 0;
    for (size_t i = 0; i < reducer->number_of_mapper; i++)
    {
        total_element = total_element + reducer->mapper[i].number_of_element_in_every_array[reducer->id];
    }

    // concat integer array _ approach 2
    int arr[2000000];
    memcpy(arr, reducer->mapper[0].array[reducer->id], sizeof(int) * reducer->mapper[0].number_of_element_in_every_array[reducer->id]);
    int offset = reducer->mapper[0].number_of_element_in_every_array[reducer->id];
    for (size_t i = 1; i < reducer->number_of_mapper; i++)
    {
        memcpy(arr + offset, reducer->mapper[i].array[reducer->id], sizeof(int) * reducer->mapper[i].number_of_element_in_every_array[reducer->id]);
        offset = offset + reducer->mapper[i].number_of_element_in_every_array[reducer->id];
    }
    qsort(arr, total_element, sizeof(int), cmpfunc);

    // count number of difference elements
    int result = count_distinct_adv(arr, total_element);
    char result_convert[5];
    sprintf(result_convert, "%d", result);

    // create name of output file
    char str[20] = "out";
    char number[5];
    sprintf(number, "%d", reducer->id + 2);
    strcat(str, number);
    strcat(str, ".txt");

    // write data to file
    FILE *fptr;

    fptr = fopen(str, "w");
    fputs(result_convert, fptr);

    fclose(fptr);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int number_of_mapper;
    int number_of_reducer;
    int number_of_in_files;
    char **in_file;
    struct my_mapper *mapper;
    struct my_reducer *reducer;
    pthread_t *threads;

    in_file = get_args(argc, argv, &number_of_mapper, &number_of_reducer, &number_of_in_files);

    threads = (pthread_t *)malloc((number_of_mapper + number_of_reducer) * sizeof(pthread_t));
    mapper = (struct my_mapper *)malloc(number_of_mapper * sizeof(struct my_mapper));
    reducer = (struct my_reducer *)malloc(number_of_reducer * sizeof(struct my_reducer));

    pthread_mutex_init(&mutex_reducer, NULL);

    // start mapper
    for (size_t i = 0; i < number_of_mapper; i++)
    {
        mapper[i].id = i;
        mapper[i].complete = 0;
        mapper[i].number_of_in_files = number_of_in_files;
        mapper[i].number_of_reducer = number_of_reducer;
        mapper[i].number_of_mapper = number_of_mapper;
        mapper[i].in_file = (char **)malloc(sizeof(char *) * number_of_in_files);
        mapper[i].array = (int **)malloc(sizeof(int *) * number_of_reducer);
        for (size_t j = 0; j < number_of_reducer; j++)
        {
            mapper[i].array[j] = (int *)malloc(sizeof(int) * 4000000);
        }

        mapper[i].number_of_element_in_every_array = (int *)malloc(sizeof(int) * number_of_reducer);
        for (size_t j = 0; j < number_of_reducer; j++)
        {
            mapper[i].number_of_element_in_every_array[j] = 0;
        }

        for (size_t j = 0; j < number_of_in_files; j++)
        {
            mapper[i].in_file[j] = (char *)malloc(sizeof(char) * 20);
            strcpy(mapper[i].in_file[j], in_file[j]);
        }

        pthread_create(&threads[i], NULL, f_map, &mapper[i]);
    }

    // start reducer
    for (size_t i = number_of_mapper; i < number_of_mapper + number_of_reducer; i++)
    {
        reducer[i - number_of_mapper].id = i - number_of_mapper;
        reducer[i - number_of_mapper].number_of_mapper = number_of_mapper;
        reducer[i - number_of_mapper].mapper = mapper;
        pthread_create(&threads[i], NULL, f_reduce, &reducer[i - number_of_mapper]);
    }

    for (size_t i = 0; i < number_of_mapper + number_of_reducer; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_reducer);
    return 0;
}