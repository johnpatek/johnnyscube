#include "application.h"

int main(int argc, char ** argv)
{
    int status;
    application_t application;

    status = CUBE_SUCCESS;
    application = NULL;
    
    if(argc > 1)
    {
        status = application_create(&application,argv[1]);
        if(status != CUBE_SUCCESS)
        {
            fprintf(stderr,"main: failed to create new application\n");
            goto done;
        }

        status = application_loop(application);
        if(status != CUBE_SUCCESS)
        {
            fprintf(stderr,"main: failed to start application loop\n");
            goto done;
        }
    }

done:
    application_destroy(application);
    return 0;
}
