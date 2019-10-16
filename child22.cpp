#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <cstring>

using namespace std;

void sSIGUSR1(int signal)
{
    cout << "SIGUSR1" << endl;
}

void sSIGUSR2(int signal)
{
    cout << "SIGUSR2" << endl;
}

/* Параметры: входной файл №2, дескриптор канала 1, 2 */
int main(int argc, char *argv[])
{
    cout << "I am fork 2!" << endl;
    pid_t pid1;
    int check = 0, bytesread = 0;
    sigset_t *sig;
    int sigptr1 = SIGUSR2;
    int sigptr2 = SIGQUIT;
    sigemptyset(sig);
    sigaddset(sig, sigptr1);
    sigaddset(sig, sigptr2);

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

    int pipem[2];
    char buff[256], text[256], pid[32];
    ssize_t buffsize;

    pipem[0] = atoi(argv[2]);
    pipem[1] = atoi(argv[3]);
    ofstream output2(argv[1], ios_base::out);
    close(pipem[1]);
    sigwait(sig, &sigptr2);
    if ((read(pipem[0], pid, 8)) == -1)
    {
        cerr << "Channel is empty! Exiting..." << endl;
        return -1;
    }
    else
    {
        string str = pid;
        pid1 = stoi(str);
        cout << "child2:PID_USR1 is " << pid1 << endl;
    }
    sigwait(sig, &sigptr2);
    do
    {
        sigwait(sig, &sigptr1);
        cout << "Started 2" << endl;
        if ((bytesread = read(pipem[0], text, 1)) == -1)
        {
            cerr << "Channel is empty! Exiting..." << endl;
            kill(pid1, SIGTERM);
            return -1;
        }
        else
        {
            cout << text << endl;
            output2 << text << endl;
        }
        cout << "Kill 1" << endl;
        kill(pid1, SIGUSR1);
    } 
    while (bytesread != 0);
    output2.close();
    return 0;
}