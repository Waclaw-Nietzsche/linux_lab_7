#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <cstring>
#include <signal.h>

using namespace std;

void sssSIGUSR1(int signal)
{
    cout << "Fork2: SIGUSR1" << endl;
}

void sssSIGUSR2(int signal)
{
    cout << "Fork2: SIGUSR2" << endl;
}

void sssSIGQUIT(int signal)
{
    cout << "Fork2: SIGQUIT" << endl;
}

/* Параметры: входной файл №2, дескриптор канала 1, 2 */
int main(int argc, char *argv[])
{
    cout << "Fork2: I am fork 2!" << endl;
    //cout << "Fork2: My group is: " << getpgid(getpid()) << endl;
    int check = 0, bytesread = 0;
    sigset_t *sssig = NULL;
    int sssigptr1 = SIGUSR2;
    int sssigptr2 = SIGQUIT;
    siginfo_t info;
    sigemptyset(sssig);
    sigaddset(sssig, sssigptr2);
    sigprocmask(SIG_BLOCK, sssig, NULL);

    int pipem[2];
    char buff[256], text[256];
    ssize_t buffsize;

    struct sigaction sssig1, sssig2, sssig3;
    sssig1.sa_handler = sssSIGUSR1;
    sssig2.sa_handler = sssSIGUSR2;
    sssig3.sa_handler = sssSIGQUIT;

    if (sigaction(SIGUSR1, &sssig1, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR1!" << endl;
    }
    if (sigaction(SIGUSR2, &sssig2, NULL) == -1)
    {
        cerr << "Can't handle with SIGUSR2!" << endl;
    }
    if (sigaction(SIGQUIT, &sssig3, NULL) == -1)
    {
        cerr << "Can't handle with SIGQUIT!" << endl;
    }

    sigwaitinfo(sssig, &info);
    //pause();
    //sigsuspend(sssig);
    sigaddset(sssig, sssigptr1);
    sigdelset(sssig, sssigptr2);
    sigprocmask(SIG_BLOCK, sssig, NULL);
    pipem[0] = atoi(argv[2]);
    pipem[1] = atoi(argv[3]);
    ofstream output2(argv[1], ios_base::out);
    close(pipem[1]);
    do
    {
        sigwaitinfo(sssig, &info);
        //sigsuspend(sssig);
        cout << "Fork2: Started 2" << endl;

        if ((bytesread = read(pipem[0], text, 1)) == -1)
        {
            cerr << "Fork2: Channel is empty! Exiting..." << endl;
            kill(0, SIGTERM);
            return -1;
        }
        else
        {
            cout << text << endl;
            output2 << text << endl;
        }

        cout << "Fork2: Kill 1" << endl;
        kill(getppid(), SIGUSR1);
    } 
    while (bytesread != 0);
    output2.close();
    return 0;
}