#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char* argv[]) {
    double mass = 1.0; // Mass of particels.
    double sfac = 100000.0; // Slowness factor.

    srand((unsigned)time(NULL));
    FILE* fp = fopen("data.dat", "w");
    fprintf(fp, "%d\n", atoi(argv[1]));
    for (int i = 0; i < atoi(argv[1]); i++) {
        fprintf(fp, "%lf %lf %lf %lf %lf %lf %lf\n",
            mass,
            (-1+2*((double)rand())/(double)RAND_MAX),
            (-1+2*((double)rand())/(double)RAND_MAX),
            (-1+2*((double)rand())/(double)RAND_MAX),
            ((-1+2*((double)rand())/(double)RAND_MAX)/sfac),
            ((-1+2*((double)rand())/(double)RAND_MAX)/sfac),
            ((-1+2*((double)rand())/(double)RAND_MAX)/sfac)
        );
    }
    
    fclose(fp);
    return 0;
}