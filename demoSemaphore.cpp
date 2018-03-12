#include <iostream>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/****************************************************************
 * Utilitary functions
 *      working with semaphores arrays
 *      if only one semaphore exits the param is zero
*******************************************************************/
void error(char* errorInfo)
{
    std::cerr<<errorInfo<<std::endl;
    exit(1);
}

/**
 * @brief doSignal
 * @param semid
 * @param numSem
 */
void doSignal(int semid, int numSem)
{
    struct sembuf sops; //Signal

    sops.sem_num = numSem;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1)
    {
        error("Error when try signal");
    }
}

/**
 * @brief doWait
 * @param semid
 * @param numSem
 */
void doWait(int semid, int numSem)
{
    struct sembuf sops;

    sops.sem_num = numSem; /* Sobre el primero, ... */
    sops.sem_op = -1;      /* ... un wait (resto 1) */
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1)
    {
        error("Error when try wait");
    }
}

/**
 * @brief initSem  initialize semaphore
 * @param semid     id Semaphore to init
 * @param numSem    numDemaphore
 * @param value
 */
void initSem(int semid, int numSem, int value)
{

    if (semctl(semid, numSem, SETVAL, value) < 0)
    {
        error("Error: semaphore initialization");
    }
}

/**
 * @brief main
 * @return
 */
int main()
{
    std::cerr<<"=== Demo synchronization using semaphores ===\n"<<std::endl;

    int semaphore;


    //semget http://pubs.opengroup.org/onlinepubs/7908799/xsh/semget.html
    //Create semamphore and assign permissions
    if( (semaphore=semget(IPC_PRIVATE,1,IPC_CREAT | 0700))<0)
    {
        error("Semaforo: semget");
    }

    initSem(semaphore,0,1);

    std::cerr<<"Here exist free pass"<<std::endl;

    switch (fork())
    {
        case -1:
            error("Error: trying  fork");

        case 0:
            /// Chils code
            doWait(semaphore,0);

            /// ----------------------
            /// BEGIN CRITICAL SECTION
            /// ----------------------
            std::cerr<<"Luke entering, so Darth Vader is waiting ..."<<std::endl;
            sleep(5);
            std::cerr<<"Luke leaving"<<std::endl;
            /// ----------------------
            /// END CRITICAL SECTION
            /// ----------------------

            doSignal(semaphore,0);
            exit(0);

        default: /* Padre */

            doWait(semaphore,0);

            /// ----------------------
            /// BEGIN CRITICAL SECTION
            /// ----------------------
            std::cerr<<"Darth Vader entering, so Luke is waiting ..."<<std::endl;
            sleep(5);
            std::cerr<<"Darth Vader leaving"<<std::endl;
            /// ----------------------
            /// END CRITICAL SECTION
            /// ----------------------

            doSignal(semaphore,0);
    }

    sleep(20);

    //free semaphore
    if ((semctl(semaphore, 0, IPC_RMID)) == -1)
    {
        error("erasing semaphore");
    }

    return 0;
}
