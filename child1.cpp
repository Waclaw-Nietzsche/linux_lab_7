#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <cstring>

using namespace std;


/* Параметры: входной файл №1, дескриптор канала 1, 2 */
int main(int argc, char *argv[])
{
    int pipem[2];
    char buff[256];
    ssize_t buffsize;
    cout << "I am fork 1!" << endl;
    pipem[0] = atoi(argv[2]);
    pipem[1] = atoi(argv[3]);
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
    }
    return 0;
}