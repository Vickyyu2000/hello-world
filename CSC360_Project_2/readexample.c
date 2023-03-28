#include <stdio.h>

typedef struct
{
    char c;
    char s[50];
    int i;
    double f;
} mytype;

int main(int argc, char const *argv[])
{
    FILE *file;

    file = fopen("example.dat", "r");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }

    mytype current; // for current line
    int read = 0;

    do
    {
        read = fscanf(file,
                      "%c,%49[^,],%d,%lf\n",
                      &current.c,
                      current.s,
                      &current.i,
                      &current.f);

        printf("%c-%s-%d-%.5f\n", current.c, current.s, current.i, current.f);
    } while (!feof(file));

    return 0;
}