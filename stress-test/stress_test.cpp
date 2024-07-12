/* ************************************************************************
> File Name:     stress_test.cpp
> Author:        程序员Carl
> 微信公众号:    代码随想录
> Created Time:  Sun 16 Dec 2018 11:56:04 AM CST
> Description:   
 ************************************************************************/

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include "../include/mySkipList.hpp"

#define NUM_THREADS 1
#define TEST_COUNT 1000000    // 1000000 / 1000
SkipList<int, std::string> skipList(15);  // 15 / 8

void *insertElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.insertNode(rand() % TEST_COUNT, "a" + std::to_string(i)); 
	}
    std::cout << "count of successfully inserted is: " << skipList.getCount() << std::endl;
    pthread_exit(NULL);
}

void *getElement(void* threadid) {
    static int num = 0;
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        if(!skipList.searchNode(rand() % TEST_COUNT).empty()){
            num++;
        }
        count++;
	}
    std::cout << "count of successfully found is: " << num << std::endl;
    pthread_exit(NULL);
}

void *deleteElement(void* threadid) {
    static int num = 0;
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
    int count_before = skipList.getCount();
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		if(skipList.deleteNode(rand() % TEST_COUNT)){
            num++;
        }
	}
    std::cout << "count of successfully deleted is: " << num << std::endl;
    pthread_exit(NULL);
}

int main() {
    srand (time(NULL));  
    {
        // 插入
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, insertElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed: " << elapsed.count() << std::endl;

        std::string path = "../data/dbfile.csv";
        skipList.export2CSV(path);
    }
    // 查找
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;
        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, getElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "get elapsed: " << elapsed.count() << std::endl;
    }
    // 删除
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;
        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, deleteElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "delete elapsed: " << elapsed.count() << std::endl;

        std::string path = "../data/dbfile_new.csv";
        skipList.export2CSV(path);
    }

	pthread_exit(NULL);
    return 0;

}
