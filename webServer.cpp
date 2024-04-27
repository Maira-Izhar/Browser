#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

void* serverProg(void* data){
	cout << endl << "--------Server here!--------" << endl;
	char* fifoname = (char*)(data);
	int fileee = open(fifoname, O_RDWR);
	char filename[100];
	ssize_t bytesread = read(fileee, filename, sizeof(filename));
	if(bytesread == -1){
		cout << "error in readding filename";
		pthread_exit(NULL);
	}
	cout << "Received filename: " << filename << endl;
	int input = open(filename, O_RDONLY);
	if (input == -1){
            	cout << "Error opening file" << endl;
            	char fail = NULL;
            	write(fileee, &fail, 1);
            	pthread_exit(NULL);
      	}
	char buffer[2048];
	
	bytesread = read(input, buffer, sizeof(buffer));
	cout << "Contents read from the file: " << buffer;
	if(bytesread == -1){
		cout << "error in reading from file" << endl;
		pthread_exit(NULL);
	}
	cout << "Sending file contents to client" << endl;
	cout << "waiting for client" << endl;
	ssize_t byteswritten = write(fileee, buffer, strlen(buffer));
	if(byteswritten == -1){
		cout << "error in writing file contents to pipe";
		pthread_exit(NULL);
	}
	int* ret = new int;
	*ret = bytesread;
	pthread_exit((void*)ret);
}

int main(){
	char input[50];
	cout << "Enter file name: ";
	cin >> input;
	while(strcmp(input, "exit") != 0){
		cout << endl << "--------Client here!--------" << endl;
		cout << "Got filename " << input << " from command line" << endl;
		
		char* myfifo = "\mypipe";
		mkfifo(myfifo, 0666);
		int fd = open(myfifo, O_RDWR);
		if (fd == -1) {
            		perror("Error opening pipe");
            		exit(EXIT_FAILURE);
      		}
      		
		ssize_t byteswritten = write(fd, input, strlen(input));
		if(byteswritten == -1){
			cout << "error in writing filename";
			return 1;
		}
		cout << "Sending filename to server" << endl;
		cout << "Waiting for server to respond" << endl;

		pthread_t th1;
		pthread_create(&th1, NULL, &serverProg, myfifo);
		cout << "Thread created with ID: " << pthread_self() << endl;
		int* size;
		if(pthread_join(th1, (void**)&size)!= 0){
			perror("thread not joined.");
			break;
		}
		cout << endl << "--------Client backk!--------" << endl;
		char content[2048];
		ssize_t bytesread = read(fd, content, sizeof(content)); 
		if(bytesread == -1){
			cout << "error in reading contents from pipe" << endl;
			return 1;
		}
		if(content[0] != NULL){
			cout << "Contents of file from pipe: " << content << endl;
		}
		else{
			cout << "File not found" << endl;
		}
		cout << "Enter file name or exit: ";
		cin >> input;
	}
}
