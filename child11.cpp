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
    int pipem[2];
    char buff[256], text[256];
    ssize_t buffsize;

    pipem[0] = atoi(argv[2]);
    pipem[1] = atoi(argv[3]);
    int check = 0, bytesread = 0;
    sigset_t *ssig = NULL;
    int ssigptr1 = SIGUSR1;
    int ssigptr2 = SIGQUIT;
    int sigcaught;
    siginfo_t info;
    sigemptyset(ssig);
    sigaddset(ssig, ssigptr2);
    sigprocmask(SIG_BLOCK, ssig, NULL);

    signal(SIGUSR1, &ssSIGUSR1);
    signal(SIGUSR2, &ssSIGUSR2);
    signal(SIGQUIT, &ssSIGQUIT);

    pause();
    sigemptyset(ssig);
    sigaddset(ssig, ssigptr1);

    ofstream output1(argv[1], ios_base::out);
    close(pipem[1]);
    do
    {
        pause();
        cout << "Fork1: Started 1" << endl;
        
        if ((bytesread = read(pipem[0], text, 1)) == -1)
        {
            cerr << "Fork1: Channel is empty! Exiting..." << endl;
            kill(0, SIGTERM);
            sleep(1);
            return -1;
        }
        else
        {
            cout << text << endl;
            output1 << text << endl;
        }

        cout << "Fork1: Kill 2" << endl;
        kill(0, SIGUSR2);
    } 
    while (bytesread != 0);
    output1.close();
    return 0;
}