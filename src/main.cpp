#include "mySkipList.hpp"

void insert_test(SkipList<int, std::string>& skip_list){
    skip_list.insertNode(1, "zhao");
    skip_list.insertNode(10, "qian");
    skip_list.insertNode(50, "sun");
    skip_list.insertNode(100, "li");
    skip_list.insertNode(1000, "zhou");
    skip_list.insertNode(5000, "wu");
    skip_list.insertNode(10000, "zheng");
    skip_list.insertNode(100000, "wang");
    std::cout << "curr max level is: " << skip_list.getCurrLevel() << std::endl;
}

void search_test(SkipList<int, std::string>& skip_list){
    int data[10] = {1, 10, 20, 30, 100, 1000, 2000, 3000, 10000, 100000};
    for(int i=0; i<10; i++){
        std::string str = skip_list.searchNode(data[i]);
        if(!str.empty()){
            std::cout << data[i] << " is found, value is: " << str << std::endl;
        }else{
            std::cout << data[i] << " is not found" << std::endl;
        }
    }
}

void delete_test(SkipList<int, std::string>& skip_list){
    int data[5] = {50, 1000, 20, 1, 150};
    for(int i=0; i<5; i++){
        bool is_deleted = skip_list.deleteNode(data[i]);
        if(is_deleted){
            std::cout << data[i] << " is deleted" << std::endl;
        }else{
            std::cout << data[i] << " is not found" << std::endl;
        }
    }
    std::cout << "\033[32m" << "data after delete is : " << "\033[0m" << std::endl 
              << "curr max level is: " << skip_list.getCurrLevel() << std::endl;
    skip_list.printList();
}

void update_test(SkipList<int, std::string>& skip_list){
    std::pair<int, std::string> new_data[5] = {{1, "liu"}, {10, "su"}, {50, "qi"}, {100, "zhang"}, {250, "wei"}};

    for(int i=0; i<5; i++){
        if(skip_list.updateNode(new_data[i].first, new_data[i].second)){
            std::cout << new_data[i].first << " update successfully, new value is: " 
                      << skip_list.searchNode(new_data[i].first) <<std::endl;
        }else{
            std::cout << new_data[i].first << " is not found" << std::endl; 
        }
        
    }
}

int main(int argc, char** argv){
    SkipList<int, std::string> skip_list(4);

    std::cout << "\033[32m" << "prepare to insert data" << "\033[0m" <<std::endl;
    insert_test(skip_list);
    
    skip_list.printList();

    std::cout << "\033[32m" << "prepare to search data" << "\033[0m" << std::endl;
    search_test(skip_list);

    std::cout << "\033[32m" << "prepare to update data" << "\033[0m" << std::endl;
    update_test(skip_list);

    std::cout << "\033[32m" << "prepare to delete data" << "\033[0m" << std::endl;
    delete_test(skip_list);
    
    std::cout << "\033[32m" << "prepare to export data to csv file" << "\033[0m" << std::endl;
    std::string path = "../data/dbfile.csv";
    skip_list.export2CSV(path);

    std::cout << "-----------------------" << std::endl;
    std::cout << "\033[32m" << "prepare to load data from csv file" << "\033[0m" << std::endl;
    SkipList<int, std::string> new_list(4);
    new_list.loadFromFile(path);
    new_list.printList();
    std::string path_ = "../data/dbfile_new.csv";
    skip_list.export2CSV(path_);
    return 0;
}