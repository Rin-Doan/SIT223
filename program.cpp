#include <iostream>
#include <fstream> // used to write file
#include <vector>
#include <chrono> // this library is used to access time classes chrono(library)::std::chrono::functions


#define range 100
#define thread_num 4

using namespace std;
// using namespace std::chrono;

struct RandomData{
    int start;
    int end;
    int seed_factor;
    int m[range][range];
};

struct MultiplyData{
    int start;
    int end;
    int mA[range][range],mB[range][range],mC[range][range];
};

void* RandomTask(void* arg){
    RandomData* thread = static_cast<RandomData*>(arg);
    
    srand(time(NULL)*thread->seed_factor);
    for (int row=thread->start;row<=thread->end;row++){
        for (int col=0; col<range; col++ ){
            thread->m[row][col]= rand()%100;
        }
    }

    return nullptr;
}

void* MultiplyTask(void* arg){
    MultiplyData* thread = static_cast<MultiplyData*>(arg);
    int temp1=0;
    int temp2=0;
    for(int rowA=thread->start; rowA<thread->end; rowA++){
        for(int colB=0; colB<range;colB++){
            for(int rowB=0; rowB<range;rowB++){
                temp1= thread->mA[rowA][rowB]*thread->mB[rowB][colB];
                temp2+=temp1;
            }
            thread->mC[rowA][colB]=temp2;
            temp1=0;
            temp2=0;
        }
    }


    return nullptr;
}

void ShowMatrix(int matrix[range][range]) { 
    cout<<endl;
    cout << "Matrix:" << endl;
    for (int i = 0; i < range; i++) {
        for (int j = 0; j < range; j++) {
            cout << matrix[i][j] << "  ";
        }
        cout << endl;
    }
}


void writeMatrixToFile(int matrix[range][range], const string& filename) {
    ofstream outputFile(filename); // initialize the object of output file
    if (!outputFile) {  // check if is there any error
        cout << "Error opening the file." << endl;
        return;
    }
    // used output file object to wite to the file

    for (int i = 0; i < range; ++i) {
        for (int j = 0; j < range; ++j) {
            outputFile << matrix[i][j] << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
}


int main() {

    int seed_factor=16;
    pthread_t randomthreadID[thread_num];
    pthread_t multiplythreadID[thread_num];

    int matrixA[range][range];
    int matrixB[range][range];
    int matrixC[range][range];

    int partition= range/(thread_num/2);

    /*
auto: automatically deduce the data type of the variable
high_resolution_clock : class with smallest possible tick
now : function find the current time 
*/
    auto startTime = chrono::high_resolution_clock :: now();

    // MatrixA
    for(int i=0; i<thread_num/2;i++){
        
        RandomData *threadData =(RandomData*)malloc(sizeof(RandomData));
        
        threadData->start=partition*i;
        threadData->end = partition*(i+1)-1;
        threadData->seed_factor=seed_factor;
        threadData->m[range][range]=matrixA[range][range];

        pthread_create(&randomthreadID[i], nullptr, RandomTask,threadData);
        
    }
    //MatrixB
    for(int i=0; i<thread_num/2;i++){
        RandomData *threadData =(RandomData*)malloc(sizeof(RandomData));

        threadData->start=partition*i;
        threadData->end = partition*(i+1)-1;
        threadData->seed_factor=seed_factor;
        threadData->m[range][range]=matrixB[range][range];

        pthread_create(&randomthreadID[i+thread_num/2], nullptr, RandomTask,threadData);
        
    }

    for(int i=0; i<thread_num;i++){
        pthread_join(randomthreadID[i],nullptr);
    }


    //MatrixC
    partition=range/thread_num;
    for(int i=0; i<thread_num;i++){

        MultiplyData *threadData=(MultiplyData*)malloc(sizeof(MultiplyData));

        threadData->start=partition*i;
        threadData->end = partition*(i+1)-1;
        threadData->mA[range][range]=matrixA[range][range];
        threadData->mB[range][range]=matrixB[range][range];
        threadData->mC[range][range]=matrixC[range][range];

        pthread_create(multiplythreadID,nullptr,MultiplyTask, threadData);

    }
    for(int i=0; i<thread_num;i++){
        pthread_join(multiplythreadID[i],nullptr);
    }
    
    
    auto endTime = chrono::high_resolution_clock::now();

    //ShowMatrix(matrixC);

    auto time= endTime - startTime;
    // assign duration to microsecond datatype (microsecond is a class) --> to see the value of duration --> use duration.count()
    auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
    // ShowMatrix(range, matrixA);
    // ShowMatrix(range, matrixB);

    writeMatrixToFile(matrixC, "output_matrix.txt");

    cout << "EXECUTION TIME: " << duration.count() <<" microsecond"<< endl ;

    //ShowMatrix(matrixA);

    return 0;
}