// main the entry point for the console application.

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

	//creates 10 reader and writer threads
	for (int i = 0; i < 10; i++)
	{
		readers.push_back(SDL_CreateThread(Reader, "reader", NULL));
		writers.push_back(SDL_CreateThread(Writer, "writer", &i));
	}

	system("PAUSE");
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

		std::cout << "Reading from textfile" << std::endl;

		std::fstream myfile("Threads.txt"); //loads our thread txt file
		std::string line;
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::cout << line << '\n' << std::endl;
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
		SDL_Delay(16 + rand() % 1000);
		//SDL_Delay(1000);
	}
	return 0;
}

int Writer(void * data)
{
	int i = *(int *)data;
	while (true)
	{
		SDL_SemWait(readerWriter);//lock readerWriter
		std::cout << "Writing to textfile" << std::endl;

		std::fstream myfile;  //load file to output too
		myfile.open("Threads.txt");//Opens text file
		myfile << "Thread is: " << i << std::endl; //Outputs to text file
		myfile.close();//Close text file

		SDL_SemPost(readerWriter);//releases lock

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 1000);
		//SDL_Delay(1000);
	}
	return 0;
}