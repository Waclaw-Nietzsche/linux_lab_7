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
    cout << "Main: SIGUSR1" << endl;
}

void sSIGUSR2(int signal)
{
    cout << "Main: SIGUSR2" << endl;
}

void sSIGQUIT(int signal)
{
    cout << "Main: SIGQUIT" << endl;
}

void sSIGTERM(int signal)
{
    cout << "Main: SIGTERM" << endl;
}

/* Параметры: откуда читает.тхт кудапишет1.тхт кудапишет2.тхт */
int main(int argc, char const *argv[])
{
    int pipem[2], status[2];
    pid_t forkPID[2];
    char buff[256], pipem0[8], pipem1[8];
    ssize_t buffsize;
    
    sigset_t *sig = NULL;
    siginfo_t info;
    int sigptr = SIGTERM;
    sigemptyset(sig);
    sigaddset(sig, sigptr);
    sigprocmask(SIG_BLOCK, sig, NULL);
    
    struct sigaction sig1, sig2, sig3, sig4;
    sig1.sa_handler = sSIGUSR1;
    sig2.sa_handler = sSIGUSR2;
    sig3.sa_handler = sSIGQUIT;
    sig4.sa_handler = sSIGTERM;

    cout << "Main: My group is: " << getpgid(getpid()) << endl;

    signal(SIGUSR1, &sSIGUSR1);
    signal(SIGUSR2, &sSIGUSR2);
    signal(SIGQUIT, &sSIGQUIT);
    signal(SIGTERM, &sSIGTERM);

    if (pipe(pipem) == 0)
    {
        cout << "Main: Channel is opened." << endl;
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

    ifstream input1(argv[1], ios_base::in);
    while(!input1.eof())
    {
        string str;
        getline(input1, str);
        strcpy(buff, str.c_str());
        int length = str.length();
        if((buffsize = write(pipem[1], &buff, length)) != length)
        {
            cerr << "Can't write full length!" << endl;
            return -1;
        }
        cout << buff; 
    }
    cout << endl;
    cout << "Main: Reading finished!" << endl;

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
    sleep(1);
    kill(0,SIGQUIT);
    sleep(1);
    kill(forkPID[0],SIGUSR1);
    
    for (int i = 0; i < 2; i++)
    {
        waitpid(forkPID[i], &status[i], 0);
    }
    cout << "Main: Finishing!" << endl;
    for (int i = 0; i < 2; i++)
    {
        if (close(pipem[i]) == -1)
        {
            cerr << "Can't close the pipe! Exiting..." << endl;
            return -1;
        }
    }
    input1.close();
    cout << "Main: Finished!" << endl;
    return 0;
}
