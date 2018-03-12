#include <iostream>
#include <cstring>

//#include <stdio.h>
//#include <stdlib.h>
#include <sys/shm.h> //Memoria compartida
#include <unistd.h>
#include <sys/ipc.h>


#define SIZE_SHAMEM 1024

int main()
{
    int pid, ID_memory;
    char *ptrSharedMemory = NULL;

    std::cout<<"=== Demo Shared Memory ===\n"<<std::endl;

    if( (ID_memory = shmget(1315511, SIZE_SHAMEM, 0664|IPC_CREAT) ) == -1 )
    { //El primer valor es un identificador unico, puede dar problemas
        std::cerr<<"Error: reserve shared memory"<<std::endl;
    } //Creo la memoria compartida

    pid = fork();

    switch(pid)
    {
        case -1:
            std::cerr<<"Error: creating childs, using fork"<<std::endl;
            break;

        case 0:
            //code for child process
            ptrSharedMemory  = (char *)shmat(ID_memory,(void *)0,0); //Asociacion

            std::cout<<"Hello my name is Luke"<<std::endl;
            strcpy(ptrSharedMemory ,"UAEM 2018 parallel programming rules!!!");

            break;

        default:
            // code for parent process
            sleep(5); // waiting for some data, yes we need syncronization

            ptrSharedMemory  = (char *)shmat(ID_memory,NULL,0); //Asociacion

            std::cout<<"Luke, I am your father"<<std::endl;
            std::cout<<"Text in shared memory: "<<ptrSharedMemory <<std::endl;

            shmdt(&ptrSharedMemory ); //Desasociacion

            if(shmctl(ID_memory,IPC_RMID,NULL)==-1)
            {
                std::cerr<<"something went wrong"<<std::endl;
            }

            break;
    }

    return 0;
}
