#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <cstring>
#include <signal.h>

using namespace std;

void ssSIGUSR1(int signal)
{
    cout << "Fork1: SIGUSR1" << endl;
}

void ssSIGUSR2(int signal)
{
    cout << "Fork1: SIGUSR2" << endl;
}

void ssSIGQUIT(int signal)
{
    cout << "Fork1: SIGQUIT" << endl;
}

/* Параметры: входной файл №1, дескриптор канала 1, 2 */
int main(int argc, char *argv[])
{
    cout << "Fork1: I am fork 1!" << endl;
    //cout << "Fork1: My group is: " << getpgid(getpid()) << endl;
    int check = 0, bytesread = 0;
    sigset_t *ssig = NULL;
    int ssigptr1 = SIGUSR1;
    int ssigptr2 = SIGQUIT;
    siginfo_t info;
    sigemptyset(ssig);
    sigaddset(ssig, ssigptr2);
    sigprocmask(SIG_BLOCK, ssig, NULL);

    struct sigaction ssig1, ssig2, ssig3;
    ssig1.sa_handler = ssSIGUSR1;
    ssig2.sa_handler = ssSIGUSR2;
    ssig3.sa_handler = ssSIGQUIT;

    int pipem[2];
    char buff[256], text[256];
    ssize_t buffsize;
    
    if (sigaction(SIGUSR1, &ssig1, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR1!" << endl;
    }
    if (sigaction(SIGUSR2, &ssig2, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR2!" << endl;
    }
    if (sigaction(SIGQUIT, &ssig3, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR2!" << endl;
    }
    sigwaitinfo(ssig, &info);
    //pause();
    //sigsuspend(ssig);
    sigaddset(ssig, ssigptr1);
    sigdelset(ssig, ssigptr2);
    sigprocmask(SIG_BLOCK, ssig, NULL);
    pipem[0] = atoi(argv[2]);
    pipem[1] = atoi(argv[3]);
    ofstream output1(argv[1], ios_base::out);
    close(pipem[1]);
    do
    {
        sigwaitinfo(ssig, &info);
        //pause();
        //sigsuspend(ssig);
        cout << "Fork1: Started 1" << endl;

        if ((bytesread = read(pipem[0], text, 1)) == -1)
        {
            cerr << "Fork1: Channel is empty! Exiting..." << endl;
            kill(0, SIGTERM);
            return -1;
        }
        else
        {
            cout << text << endl;
            output1 << text << endl;
        }

        cout << "Fork1: Kill 2" << endl;
        kill(getppid(), SIGUSR2);
    } 
    while (bytesread != 0);
    output1.close();
    return 0;
}