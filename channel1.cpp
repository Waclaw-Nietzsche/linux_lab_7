#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

/* Параметры: входной файл №1, №2, выходной файл */
int main(int argc, char *argv[])
{
    assert(argc == 4);
    pid_t forkPID[2];
    int status[2];
    int pipem[2];
    int bytesread = 0;
    char text[256];
    char pipem0[8], pipem1[8];

    ofstream output(argv[3], ios_base::out);

    if (pipe(pipem) == 0)
    {
        cout << "Channel is opened." << endl;
        const int flags = fcntl(pipem[0], F_GETFL, 0);
        fcntl(pipem[0], F_SETFL, flags | O_NONBLOCK);
        snprintf(pipem0, sizeof(pipem0), "%d", pipem[0]);
        snprintf(pipem1, sizeof(pipem1), "%d", pipem[1]);
        for (int i = 0; i < 2; i++)
        {
            if ((forkPID[i] = fork()) == 0)
            {
                if (i == 0)
                {
                    
                    execl("child1", "child1", argv[1], pipem0, pipem1, NULL);
                }
                else
                {
                    execl("child2", "child2", argv[2], pipem0, pipem1, NULL);
                }
                exit(0);
            }
            else if (forkPID[i] == -1)
            {
                cerr << "Can't create fork! Exiting..." << endl;
                exit(1);
            }
        } 

    }
    else
    {
        cerr << "Can't open pipe! Exiting..." << endl;
        return -1;
    }
    sleep(1);
    do
    {
        if ((bytesread = read(pipem[0], text, 1)) == -1)
        {
        cerr << "Can't read from the channel! Exiting..." << endl;
        return -1;
        }
        else
        {
            output << text << endl;
        }
    } 
    while (bytesread != 0);
    cout << "Here now!" << endl;
    for (int i = 0; i < 2; i++)
    {
        waitpid(forkPID[i], &status[i], 0);
    }

    for (int i = 0; i < 2; i++)
    {
        if (close(pipem[i]) == -1)
        {
            cerr << "Can't close the pipe! Exiting..." << endl;
            return -1;
        }
    }
    output.close();
    cout << "Channel is closed." << endl;
    return 0;
}
