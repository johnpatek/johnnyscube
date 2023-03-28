#include <cube.h>

int main(int argc, char **argv)
{
    cube_application *application;
    
    application = NULL;

    if(argc > 1)
    {
        if (application_create(&application, argv[1]) != CUBE_SUCCESS)
        {
            puts("failed to create application");
            goto done;
        }

        if (application_loop(application) != CUBE_SUCCESS)
        {
            puts("failed to run application");
            goto done;
        }
    }

done:
    if(application != NULL)
    {
        application_destroy(application);
    }
    return 0;
}