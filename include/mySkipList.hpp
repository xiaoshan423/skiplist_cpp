#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <mutex>

template<typename K, typename V>
class Node{
    public:
        Node() = default;
        Node(K key, V value, int level){
            this->key_ = key;
            this->value_ = value;
            this->curr_level = level;
            forward = new Node*[curr_level]; // 创建一个节点的同时创建一个指向下一个节点的指针数组，数组中的内容是空的
            memset(forward, 0, sizeof(Node*) * curr_level);
        }
        ~Node(){
            delete []forward;
        }
        K getKey(){
            return this->key_;
        }
        V getValue(){
            return this->value_;
        }
        void setValue(V value){
            this->value_ = value;
        }
        Node<K, V>** forward;
        int curr_level;
    private:
        K key_;
        V value_;
};

template<typename K, typename V>
class SkipList{
    public:
        SkipList() = default;
        SkipList(int max_level){
            this->max_level_ = max_level;
            K key;
            V value;
            header_ = new Node<K, V>(key, value, max_level); // 头节点实现分配好可以指向所有层级节点的指针数组
            curr_max_level_ = 0; // 跳表刚建立时只有一层
            element_count_ = 0;
            srand((int)time(NULL));
        }
        ~SkipList(){
            delete header_;
        }
        int getCurrLevel(){
            return this->curr_max_level_;
        }
        int getCount(){
            return this->element_count_;
        }
    private:
        std::mutex mtx;
        int max_level_; // 这个跳表的最大层数
        int curr_max_level_; // 这个跳表目前的层数
        int element_count_;
        Node<K, V>* header_; // 跳表的头节点
    public:
        // 插入键值对
        int insertNode(K key, V value){
            mtx.lock();
            // 首先掷硬币决定这个节点应该在同时插入到第几层作为索引，更新当前最高层数
            int iLevel = randomLevel();
            // std::cout << "random level is " << iLevel << ", curr max level is " << curr_max_level_ << std::endl;

            int max_level = iLevel > curr_max_level_ ? iLevel : curr_max_level_;
            Node<K, V>* update[max_level + 1]; // 节点指针数组，保存每一层的最合适的插入点
            memset(update, 0, sizeof(Node<K, V>*) * (max_level + 1));

            Node<K, V>* current = this->header_;
            // 在每一层找到适合插入当前节点的位置，记录到update数组中
            for(int i=curr_max_level_; i>=0; i--){
                // 用两个指针依次遍历，找到包含key的区间
                Node<K, V>* next = current->forward[i];
                while(next != nullptr && next->getKey() < key){
                    current = next;
                    next = next->forward[i];
                }
                update[i] = current;
            }
            // 考虑一下重复插入的问题，只考虑第一层
            Node<K, V>* bottom = update[0];
            if(bottom->forward[0] && bottom->forward[0]->getKey() == key){
                mtx.unlock();
                return -1;
            }

            // 如果当前节点随机层数大于当前最高层数，这个节点将成为该层除头节点外的第一个有效节点，必须链接到头节点后面
            if(iLevel > curr_max_level_){
                for(int i=curr_max_level_+1; i<=iLevel; i++){
                    update[i] = this->header_;
                }
                curr_max_level_ = iLevel;
            }

            Node<K, V>* iNode = new Node<K, V>(key, value, iLevel + 1);

            // 在每一层的插入点进行插入
            for(int i=iLevel; i>=0; i--){
                iNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = iNode;
            }
            element_count_++;
            mtx.unlock();
            return 0;
        }
        // 根据键搜索节点值
        V searchNode(K target){
            Node<K, V>* curr = this->header_;
            for(int i=curr_max_level_; i>=0; i--){
                while(curr->forward[i]){
                    K next_key = curr->forward[i]->getKey();
                    if(next_key < target){ // 如果要找的key比下一个key还大，层间遍历
                        curr = curr->forward[i];
                    }else if(next_key == target){ // 如果当前层找到了key，直接返回
                        return curr->forward[i]->getValue();
                    }else{
                        break; // 当前层没有找到，进入下一层
                    }
                }
            }
            return V();
        }
        // 根据键删除节点
        bool deleteNode(K target){
            Node<K, V>* update[curr_max_level_+1];
            memset(update, 0, sizeof(Node<K, V>*) * (curr_max_level_+1));
            int height = -1;
            Node<K, V>* curr = this->header_;
            // 首先找到对应的节点和对应的层数
            for(int i=curr_max_level_; i>=0; i--){
                while(curr->forward[i]){
                    K key = curr->forward[i]->getKey();
                    if(key < target){ // 如果比要删除的节点key小，层间遍历
                        curr = curr->forward[i];
                    }else if(key == target){ // 如果找到了要删除的节点key，保存前一个节点
                        update[i] = curr;
                        if(height < 0){
                            height = i; // 从高向下遍历时记录下第一次出现的层高
                        }
                        break; // 然后跳入下一层
                    }else{
                        break;
                    }
                }
            }
            if(height >= 0){
                Node<K, V>* target_node = update[0]->forward[0];
                for(int i=height; i>=0; i--){
                    update[i]->forward[i] = target_node->forward[i];
                    if(header_->forward[i] == nullptr){ // 更新当前最高层数
                        curr_max_level_ = i-1;
                    }
                }
                delete target_node;
                element_count_--;
                return true;
            }
            return false;
        }

        bool updateNode(K target, V value){
            Node<K, V>* curr = this->header_;
            for(int i=curr_max_level_; i>=0; i--){
                while(curr->forward[i]){
                    K next_key = curr->forward[i]->getKey();
                    if(next_key < target){ // 如果要找的key比下一个key还大，层间遍历
                        curr = curr->forward[i];
                    }else if(next_key == target){ // 如果当前层找到了key，直接返回
                        curr->forward[i]->setValue(value);
                        return true;
                    }else{
                        break; // 当前层没有找到，进入下一层
                    }
                }
            }
            return false;
        }

        // 将整个跳表打印出来
        void printList(){
            std::cout << "count of all elements is: " << element_count_ << std::endl;
            for(int i=curr_max_level_; i>=0; i--){
                Node<K, V>* curr = header_;
                std::cout << "\033[34m" << i << ": " << "\033[0m";
                while(curr->forward[i]){
                    std::cout << curr->forward[i]->getKey() << " ";
                    curr = curr->forward[i];
                }
                std::cout << std::endl;
            }
        }

        void export2CSV(std::string path){
            std::ofstream os;
            os.open(path, std::ios::out);
            Node<K, V>* curr = this->header_;
            while(curr->forward[0]){
                os << curr->forward[0]->getKey() << " " << curr->forward[0]->getValue() << std::endl;
                curr = curr->forward[0];
            }
            os.close();
        }

        void loadFromFile(std::string path){
            std::ifstream is;
            is.open(path, std::ios::in);
            std::string line;
            while(getline(is, line)){
                if(line.empty()){
                    continue;
                }
                int pos = 0;
                if((pos = line.find(' ')) != std::string::npos){
                    K key = std::stoi(line.substr(0, pos));
                    V value = line.substr(pos+1, line.size()-pos-1);
                    this->insertNode(key, value);
                }
            }
        }
    private:
        // 按照0.5的概率获取可作为索引的最高层数
        int randomLevel(){
            int level = 0; // 如果是0，不作为索引
            while(rand() % 2){ // rand() % 2等于0或者等于1，连续等于1的情况的概率越来越低
                ++level;
            }
            level = level < max_level_ ? level : max_level_ - 1; // 不能超过跳表的最高层数
            return level;
        }
};