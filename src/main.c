#include "application.h"

int main()
{
    int status;
    const char * error;
    application_t application;

    status = 0;
    error = NULL;
    application = NULL;

error:
    if(error != NULL)
    {
        fprintf("main: %s",error);
    }
done:
    application_destroy(application);
    return 0;
}