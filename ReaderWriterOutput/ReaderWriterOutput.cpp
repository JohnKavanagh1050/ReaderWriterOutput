// main the entry point for the console application.
//

#include "stdafx.h"
#include <SDL.h>
#include <fstream>
#include <string>
#include <iostream>
#include <list>

int nr = 0; //num of readers

//Data access semaphore
SDL_sem* readerWriter = NULL;
SDL_sem* mutexR = NULL;

int Reader(void * data);
int Writer(void * data);

//applications main function
int _tmain(int argc, _TCHAR* argv[])
{
	//makes a list for readers and writers
	std::list<SDL_Thread*> readers;
	std::list<SDL_Thread*> writers;

	//Initialize semaphore
	readerWriter = SDL_CreateSemaphore(1);
	mutexR = SDL_CreateSemaphore(1);

	//creates 10 reader and writer threads using semaphores
	for (int i = 0; i < 10; i++)
	{
		readers.push_back(SDL_CreateThread(Reader, "reader", NULL));
		writers.push_back(SDL_CreateThread(Writer, "writer", &i));
	}

	system("pause");
	return 0;
}

int Reader(void * data)
{
	while (true)
	{

		SDL_SemWait(mutexR);//Lock in mutexR
		nr = nr + 1;
		
		if (nr == 1)
		{
			SDL_SemWait(readerWriter);//lock in readerWriter
		}
		SDL_SemPost(mutexR);//releases lock

		std::cout << "reading from textfile" << std::endl;

		std::fstream myfile("Threads.txt"); //loads our thread txt file
		std::string line;

		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::cout << line << '\n';
			}
			myfile.close();
		}

		SDL_SemWait(mutexR);//lock mutexR
		nr = nr - 1;
		if (nr == 0)
		{
			SDL_SemPost(readerWriter);//if last thread, release the lock
		}
		SDL_SemPost(mutexR);//release mutexR

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 640);//lazyfoo master of disaster
	}
	return 0;
}

int Writer(void * data)
{
	int i = *(int *)data;
	while (true)
	{
		SDL_SemWait(readerWriter);//lock readerWriter
		std::cout << "writing to textfile" << std::endl;

		std::fstream myfile;
		myfile.open("Threads.txt");//Open text file
		myfile << "Thread is: " << i << std::endl; //Output to text file
		myfile.close();//Close text file

		SDL_SemPost(readerWriter);//releases lock

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 640);
	}
	return 0;
}

