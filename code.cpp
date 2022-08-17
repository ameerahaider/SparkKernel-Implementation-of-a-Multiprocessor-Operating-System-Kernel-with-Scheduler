#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <queue>
#include <stack>

using namespace std;

//Global Variabls
int noOfProcs, ProcNo = 0;
string inputFile, outputFile, scheduler;
int noOfCPU, timeSlice;
int cpuTimeCounter = 0, ioTimeCounter = 0, computerClock = 0;
int ContextSwitchCounter = 0;

//************************** PROCESS PCB **************************
struct PCB {

    string name;
    int priority;
    double arrivalTime;
    string type;
    string state;
    double cpuTime;
    double ioTime;
    stack<int> stk;

    int waitTime;
    int TAT;

}temp;

//Global List of Processes
PCB* process;
//************************** PROCESS **************************

//************************** QUEUE **************************
class node {

	PCB t;

public:

	node* next;

	PCB getData() { return t; }
	void setTask (PCB x) { t = x; }
};

class Queue {

	node* front;
	node* rear;

public:

	Queue() {

		front = NULL;
		rear = NULL;
	}

	node* getFront() { return front; }
	node* getRear() { return rear; }

	PCB Front() {
		
		node* frontNode = getFront();

		return frontNode->getData();
	}

	bool isEmpty() {

		if (getFront() == NULL) return true;
		else return false;
	}

	void enqueue (PCB value) {

		node* newNode = new node;
		newNode->setTask(value);
		newNode->next = NULL;

		if (front == NULL) {
			
			front = rear = newNode;
			rear->next = NULL;
		}

		else {
			
			rear->next = newNode;
			rear = newNode;
		}
        //cout << "ADDED " << value.name << endl;
	}

	void dequeue() {

		node* temp;

		temp = front->next;
        PCB tempp = front->getData();
        //cout << "REMOVED " << tempp.name << endl;
		delete front;
		front = temp;
	}
};

//Global Queues
Queue fcfs, priorityQueue, newQueue, readyQueue, runningQueue, terminatedQueue, blockedQueue;
//************************** QUEUE **************************



//Structure to Store File Data
struct Data {

    int r,c; //Rows and Cols
    string** text; //String Array

    Data () {

        r = c = noOfProcs = 0;

        text = new string* [r];
        for (int i = 0; i <= r; i++) {
            text[i] = new string[c];
        }

        process = new PCB[r];
    }

    Data (int row, int col) {

        r = noOfProcs = row;
        c = col;

        text = new string * [r];
        for (int i = 0; i <= r; i++) {
            text[i] = new string[c];
        }
        
        process = new PCB[noOfProcs];
    }

    //Reading Process Information From File
    void processInfo() {

        fstream fin;
        string word;

        fin.open(inputFile);

        //Insertion from given file
        for (int i = 0; i <= r; i++) {
            for (int j = 0; j < c; j++) {

                fin >> word;
                //cout << "Word " << word << endl;
                text[i][j] = word;
            }
        }


        fin.close();

        //Adding Values to Objects
        int objNo = 0;
        for (int i = 1; i <= noOfProcs; i++){

            process[objNo].name = text[i][0];
            process[objNo].priority = stoi(text[i][1]);    
            process[objNo].arrivalTime = stof(text[i][2]);
            process[objNo].type = text[i][3];
            process[objNo].cpuTime = stof(text[i][4]);
            process[objNo].state = "NEW";
            process[objNo].ioTime = stof(text[i][5]);
            objNo++;
        }
    }

    void showProcesses() {

        cout << endl << endl << "PROCESS INFORMATION" << endl << endl;

        for (int i = 0; i < noOfProcs; i++) {

            cout << process[i].name << "\t"
            << process[i].priority << "\t"
            << process[i].type << "\t"
            << process[i].state << "\t"
            << process[i].arrivalTime << "\t"
            << process[i].cpuTime << "\t"
            << process[i].ioTime << endl;
        }
    }
};

//Global Variables
Data* fileData;

//******************** FUNCTIONS ********************
//File Reader
void fileCounter () {

    fstream fin;
    fin.open(inputFile);
    int row = 1, col = 1;

    //Counting Row and Cols
    char x;
    while (!fin.eof()) {

        fin.get(x);
        //cout << "Char " << x << endl;
        
        if (x == '\n') {
            row++;
            //cout << "New Row " << row << endl;
        }
        if (row == 2 && x == '\t'){
            col++;
            //cout << "New Col" << endl;
        }  
    }

    fin.close();

    row--; //Not Counting First Row

    cout << "Rows " << row << endl;
    cout << "Columns " << col << endl;

    //Creating Object of Data
    fileData = new Data(row, col);
    fileData->processInfo();
    fileData->showProcesses();
}

//Calculations
//Function to find waiting timer of each process
void waitTime() {

    process[0].waitTime = 0;

    for (int i = 1; i < noOfProcs; i++)
            process[i].waitTime =  process[i-1].cpuTime +  process[i-1].waitTime;

}

void TAT() {

        for (int i = 0; i < noOfProcs; i++)
            process[i].TAT = process[i].cpuTime + process[i].waitTime;
    }


    //function to find the average waiting timer of processes
    void calculations() {

        double TotalWait = 0, TotalTaT = 0;

        waitTime();
        TAT();

        //Display processes along with all details
        cout << "Processes Name\tBurst time\tWait time\tTAT\n";

        for (int k = 0; k < noOfProcs; k++)
        {
            
            cout << process[k].name << "\t" << process[k].cpuTime 
            << "\t" << process[k].waitTime << "\t" << process[k].TAT << endl;

            TotalWait = TotalWait + process[k].waitTime;
            TotalTaT = TotalWait + process[k].TAT;
        }

        cout << "Average Waiting Time = " << TotalWait / noOfProcs;
        cout << "\nAverage Turn Around Time = " << TotalTaT / noOfProcs;
        return;
    }

//Sort Processes Based on Arrival Time
void FCFS_Sort(){

    for (int i = 0; i < noOfProcs; i++) {
        for (int j = i+1; j < noOfProcs; j++) {

            if(process[i].arrivalTime > process[j].arrivalTime){

                temp = process[j];
                process[j] = process[i];
                process[i] = temp;
            }
        }
    }

    for (int i = 0; i < noOfProcs; i++){
        
        fcfs.enqueue(process[i]);
    }
}

//Storing Based on Priority
void Priority_Sort(){

    for (int i = 0; i < noOfProcs; i++) {
        for (int j = i+1; j < noOfProcs; j++) {

            if(process[i].priority < process[j].priority) {

                temp = process[j];
                process[j] = process[i];
                process[i] = temp;
            }
        }
    }

    for (int i = 0; i < noOfProcs; i++){
        
        priorityQueue.enqueue(process[i]);
    }
}

// in main call MakeQUeue for Ready
void start (Queue curr) {

    //cout << "FROM FCFS TO NEW" << endl;
    while (!curr.isEmpty()){

        PCB temp = curr.Front();
        temp.state = "NEW";
        newQueue.enqueue(temp);
        curr.dequeue();
    }
}

void wake_up(){

    //cout << "FROM NEW TO READY" << endl;
    while(!newQueue.isEmpty()){
        
        temp = newQueue.Front();
        temp.state = "READY";
        newQueue.dequeue();
        readyQueue.enqueue(temp); 
    }  
}

void IdleProcess(){

    //will run when no processes are available
    cout << "Idle Process\n";
}

void ContextSwitch (){

    //cout << "Context Switchting" << endl;

    if (readyQueue.isEmpty()) return;

    //will pass the new process to cpu
    temp = readyQueue.Front();
    temp.state = "RUNNING";
    readyQueue.dequeue();
    runningQueue.enqueue(temp);

    cpuTimeCounter = 0;
    ioTimeCounter = 0;
}

void terminateFunction(){

    //cout << "Terminating" << endl;
    //process terminates and exits the running queue and goes to terminate queue
    if (runningQueue.isEmpty()) return;

    temp = runningQueue.Front();
    temp.state = "TERMINATED";
    cout << "Terminated " << temp.name << endl;
    runningQueue.dequeue();
    terminatedQueue.enqueue(temp);
}

void Idle(){
    //cout << "Idle\n";
}

void yield() {
    //cout << "Yeild" << endl;
    sleep(2);
}

void force_preempt (int procNo) {

    cout << "Preempt\n";

    PCB temp;
    PCB temp2;

        temp = readyQueue.Front();
        temp2 = runningQueue.Front();

        if (readyQueue.isEmpty() || runningQueue.isEmpty()) return;
        //cout << "Prorities " << temp.priority << "  " << temp2.priority << endl;
        if(readyQueue.isEmpty()){
            
            if ( temp.priority < temp2.priority) {
                
                if (cpuTimeCounter == process[procNo].cpuTime){

                    runningQueue.dequeue();
                    terminatedQueue.enqueue(temp2);
                    readyQueue.dequeue();
                    readyQueue.enqueue(temp);

                    ContextSwitchCounter++;
                }
                else {

                    runningQueue.dequeue();
                    readyQueue.enqueue(temp2);
                    readyQueue.dequeue();
                    runningQueue.enqueue(temp);

                    ContextSwitchCounter++;
                }
            }
        }
}

//CPU Thread to do Schedulling
void* schedulerFunction(void *threadarg) {
    
    if ((scheduler == "f" || scheduler == "F") && noOfCPU == 1) {
        
        cout << endl << endl << "RUNNING FCFS SCHEDULLING " << endl;
        int countProc = 0;
        PCB temp;
        bool available = true;
    
        FCFS_Sort();
        start(fcfs);
        wake_up();

        if (newQueue.isEmpty())
            IdleProcess();

        while (ProcNo < noOfProcs) {

            if (process[ProcNo].type == "C") {

                while (cpuTimeCounter < process[ProcNo].cpuTime){

                    cpuTimeCounter++;
                    computerClock++;
                    cout << "CC " << computerClock << " " << cpuTimeCounter << ": " << process[ProcNo].name << " has CPU" << endl;
                    available = false;
                }

                available = true;

                ContextSwitch();
                ContextSwitchCounter++;

                terminateFunction();

                countProc++;
                ProcNo++;        
            }

            //for I/O bound processes
            else if(process[ProcNo].type == "I") {

                while (cpuTimeCounter != process[ProcNo].cpuTime){

                    cpuTimeCounter++;
                    computerClock++;
                    cout << "CC " << computerClock << " " << cpuTimeCounter << ": " << process[ProcNo].name << " has CPU" << endl;
                    available = false;
                }

                while (ioTimeCounter != process[ProcNo].ioTime){

                    ioTimeCounter++;
                    computerClock++;
                    cout << "CC " << computerClock << " " << ioTimeCounter << ": " <<  process[ProcNo].name << " is taking Input" << endl;
                    Idle();
                    yield();
                    available = false;
                }

                available = true;

                ContextSwitch();
                ContextSwitchCounter++;

                terminateFunction();

                countProc++;
                ProcNo++;   

            }
        }
        
        cout << endl << endl << "Total number of context switches are: " << ContextSwitchCounter << endl;
        cout << endl << endl << "Displaying Calculations" << endl;
        calculations();
    }
    else if ((scheduler == "p" || scheduler == "P") && noOfCPU == 1){

        cout << endl << endl << "RUNNING PREEMPTIVE PRIORITY SCHEDULLING " << endl;

        int countProc = 0;
        PCB temp;
        bool available = true;
        int counter = 0;

        Priority_Sort();
        start(priorityQueue);
        wake_up();

        if (newQueue.isEmpty())
            IdleProcess();

        ContextSwitch();
        while (ProcNo < noOfProcs){

            if (process[ProcNo].type == "C") {

                cout << "C " << process[ProcNo].name << endl;

                while (cpuTimeCounter < process[ProcNo].cpuTime){

                    cpuTimeCounter++;
                    computerClock++;
                    counter++;

                    cout << "CC " << computerClock << " " << cpuTimeCounter << ": " << process[ProcNo].name << " has CPU" << endl;
                    available = false;
                    
                    if (counter == timeSlice) {
                        
                        if (ProcNo != noOfProcs-1)
                            force_preempt(ProcNo);
                        counter = 0;
                    }

                    
                }


                available = true;
                //Call Context Switch to run next process
                ContextSwitch();
                ContextSwitchCounter++;

                //terminate the completed process
                terminateFunction();

                //itterating process number
                countProc++;
                ProcNo++;        
            }
            else if(process[ProcNo].type == "I"){

                cout << "I " << process[ProcNo].name << endl;

                while (cpuTimeCounter != process[ProcNo].cpuTime){

                    cpuTimeCounter++;
                    computerClock++;
                    counter++;
                    cout << "CC " << computerClock << " " << cpuTimeCounter << ": " << process[ProcNo].name << " has CPU" << endl;
                    available = false;

                    if (counter == timeSlice){

                        force_preempt(ProcNo);
                        counter = 0;
                    }
                }

                while (ioTimeCounter != process[ProcNo].ioTime){

                    ioTimeCounter++;
                    computerClock++;
                    cout << "CC " << computerClock << " " << ioTimeCounter << ": " <<  process[ProcNo].name << " is taking Input" << endl;
                    Idle();
                    yield();
                    available = false;
                }

                //cpu is now avaliable
                available = true;

                //call context switch to run next process
                ContextSwitch();
                ContextSwitchCounter++;

                //terminate the completed process
                terminateFunction();

                //itterating process number
                countProc++;
                ProcNo++; 
            }

        }
        cout << endl << endl << "Total number of context switches are: " << ContextSwitchCounter << endl;
        cout << endl << endl << "Displaying Calculations" << endl;
        calculations();
    }
    
} 

int main (int argc, char *argv[]) {
    
    if( argc == 6 ) {

        printf("Input File is %s\n", argv[1]);
        inputFile = argv[1];
        printf("# of CPUs is %s\n", argv[2]);
        noOfCPU = atoi(argv[2]);
        printf("Scheduler is %s\n", argv[3]);
        scheduler = argv[3];
        printf("Time Slice is %s\n", argv[4]);
        timeSlice = atoi(argv[4]);
        printf("Output File is %s\n", argv[5]);
        outputFile = argv[2];
   }
   else if( argc > 6 ) {
      printf("Too many arguments supplied.\n");
      return 0;
   }
   else {
      printf("Six argument expected.\n");
      return 0;
   }

    cout << endl << endl;

    fileCounter();

    //CPU Threads
    pthread_t threads[noOfCPU];
    for (int i = 0; i < noOfCPU; i++) {
        pthread_create(&threads[i], NULL, schedulerFunction, NULL);
	}

    //Joining CPU Threads
    for (int i = 0; i < noOfCPU; i++)
        pthread_join(threads[i], NULL);

    return 0;
}