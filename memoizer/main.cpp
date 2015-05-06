#define DEBUG 1
#include <iostream>

#include <deque>          // std::deque
#include <queue>          // std::queue
#include <map>            // std::map
#include <vector>         // std::vector

template<class InputType, class OutputType> class Memoizer {
    typedef typename std::deque<InputType> CacheContainerType;
    typedef typename std::queue<InputType> CacheType;
    typedef typename std::map<InputType, OutputType> MapType;
    typedef OutputType (*FunctionType) (InputType);

    private:
        int cacheSize;
        CacheType queue;
        MapType map;
        FunctionType function;

    public:
        Memoizer(FunctionType _function, int _cacheSize) : cacheSize(_cacheSize) {
            function = _function;
            if ( cacheSize > 0 ) {
                queue = CacheType();
                map = MapType();
            }
        }

        OutputType operator () (InputType x) {
            if ( cacheSize <= 0 ) {
                return function(x);
            }

            OutputType result;
            typename MapType::iterator it = map.find(x);
            if (it == map.end()) {  // Item is not in the cache

                result = function(x);      // Evaluate function's return value

                if ( map.size() == cacheSize ) {
                    // Remove the oldest cached item
                    InputType discardedItem = queue.front();
                    queue.pop();
                    map.erase(discardedItem);
                    #if DEBUG
                        std::cout << "Item removed from cache" << std::endl;
                    #endif
                }
                #if DEBUG
                    std::cout << "Item added to cache" << std::endl;
                #endif
                map[x] = result;    // Cache return value
                queue.push(x);      // Document that it's cached
            }
            else {                  // Item is in the cache
                result = map[x];    // Retrieve the cached value
                #if DEBUG
                    std::cout << "Item retrieved from cache" << std::endl;
                #endif
            }
            return result;
        }
};

int f(int x){
    return x*2;
}

int g(char* x){
    if ( x != nullptr ) {
        return (int)(*x);
    }
    else {
        return -1;
    }
}

template<typename T>
int* h(std::vector<T>* x){
    if ( x->size() > 0 ) {
        return &(x->at(0));
    }
    else {
        return nullptr;
    }
}

int main(int argc, char** argv){

    Memoizer<int,int> mem_f = Memoizer<int,int>(f, 2);
    int x = 4;
    std::cout << mem_f(x) << std::endl;     // f(x) is cached
    std::cout << mem_f(x) << std::endl;     // f(x) is retrieved

    std::cout << mem_f(2*x) << std::endl;   // f(2x) is cached

    std::cout << mem_f(3*x) << std::endl;   // f(3x) is cached, f(x) is discarded

    std::cout << mem_f(x) << std::endl;     // f(x) is cached, f(2x) is discarded

    std::cout << mem_f(3*x) << std::endl;   // f(3x) is retrieved


    std::cout << "\n\n";
    Memoizer<char*, int> mem_g(g, 1);
    char c = 'c';
    std::cout << mem_g(&c) << std::endl;

    // Modifying the value at the address will have no effect on the output
    // if the address is in the cache
    c = 'd';
    std::cout << mem_g(&c) << std::endl;


    std::cout << "\n\n";
    Memoizer<std::vector<int>*, int*> mem_h(h<int>,2);
    std::vector<int> v;
    v.push_back(4);

    int* item = mem_h(&v);
    // The item in the cache can be modified with the reference returned
    if ( item != nullptr ) {
        std::cout<< *item << std::endl;
        *item = 100;
        std::cout<< *mem_h(&v) << std::endl;
    }
}
