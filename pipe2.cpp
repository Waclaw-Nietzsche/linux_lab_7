#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <signal.h>

using namespace std;

void sSIGUSR1(int signal)
{
    cout << "SIGUSR1" << endl;
}

void sSIGUSR2(int signal)
{
    cout << "SIGUSR2" << endl;
}

int main(int argc, char const *argv[])
{
    int pipem[2], status[2];
    pid_t forkPID[2];
    char buff[256], pipem0[8], pipem1[8], pid1[8], pid2[8];
    ssize_t buffsize;

    struct sigaction sig1, sig2;
    sig1.sa_handler = sSIGUSR1;
    sig2.sa_handler = sSIGUSR2;
    
    if (sigaction(SIGUSR1, &sig1, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR1!" << endl;
    }
    if (sigaction(SIGUSR2, &sig2, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR2!" << endl;
    }

    if (pipe(pipem) == 0)
    {
        cout << "Channel is opened." << endl;
        const int flags = fcntl(pipem[0], F_GETFL, 0);
        fcntl(pipem[0], F_SETFL, flags | O_NONBLOCK);
        snprintf(pipem0, sizeof(pipem0), "%d", pipem[0]);
        snprintf(pipem1, sizeof(pipem1), "%d", pipem[1]);
    }
    else
    {
        cerr << "Can't open pipe! Exiting..." << endl;
        return -1;
    }

    for (int i = 0; i < 2; i++)
    {
        if ((forkPID[i] = fork()) == 0)
        {
            if (i == 0)
            {
                execl("child11", "child11", argv[2], pipem0, pipem1, NULL);
            }
            else
            {
                execl("child22", "child22", argv[3], pipem0, pipem1, NULL);
            }
            exit(0);
        }
        else if (forkPID[i] == -1)
        {
            cerr << "Can't create fork! Exiting..." << endl;
            exit(1);
        }
    } 
    snprintf(pid1, sizeof(pid1), "%d", forkPID[0]);
    snprintf(pid2, sizeof(pid2), "%d", forkPID[1]);
    if((write(pipem[1], &pid2, 8)) != 8)
    {
        cerr << "Can't write full length!" << endl;
        return -1;
    } 
    if((write(pipem[1], &pid1, 8)) != 8)
    {
        cerr << "Can't write full length!" << endl;
        return -1;
    } 
    sleep(1);
    for (int i = 0; i < 2; i++)
    {
        kill(forkPID[i], SIGQUIT);
        sleep(1);
    }
    

    ifstream input1(argv[1], ios_base::in);
    while(!input1.eof())
    {
        string str;
        getline(input1, str);
        cout << str;
        strcpy(buff, str.c_str());
        int length = str.length();
        if((buffsize = write(pipem[1], &buff, length)) != length)
        {
            cerr << "Can't write full length!" << endl;
            return -1;
        } 
    }
    for (int i = 0; i < 2; i++)
    {
        kill(forkPID[i], SIGQUIT);
        sleep(1);
    }
    kill(forkPID[0], SIGUSR1);
    for (int i = 0; i < 2; i++)
    {
        waitpid(forkPID[i], &status[i], 0);
    }
    cout << "Going to finish!" << endl;
    for (int i = 0; i < 2; i++)
    {
        if (close(pipem[i]) == -1)
        {
            cerr << "Can't close the pipe! Exiting..." << endl;
            return -1;
        }
    }
    input1.close();
    return 0;
}
