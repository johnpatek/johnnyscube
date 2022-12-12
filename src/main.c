#include "application.h"

int main(int argc, const char **argv)
{
    if (argc > 1)
    {
        application_t application;
        const char *error;

        application_create(&application, &error);
        application_initialize(application, argv[1], &error);
        application_loop(application, &error);
        application_destroy(application);
    }
    return 0;
}
