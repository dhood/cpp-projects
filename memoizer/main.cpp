#define DEBUG 1
#if DEBUG
    #include <iostream>
#endif

#include <deque>          // std::deque
#include <queue>          // std::queue
#include <map>            // std::map


// Returns a function which can be called in place of the input function f
// to memoize the function result for the cacheSize most recent distinct arguments
using std::function;
template<typename InputType, typename OutputType>
function<OutputType(InputType)> memoize(function<OutputType(InputType)> f, int cacheSize) {

    typedef typename std::deque<InputType> CacheContainerType;
    typedef typename std::queue<InputType> ValuesCachedType;
    typedef typename std::map<InputType, OutputType> CacheType;

    if (cacheSize > 0) {                        // Cache is to be used
        ValuesCachedType valuesCurrentlyCached;
        CacheType cache;
        return [=](InputType x) mutable -> OutputType {
            OutputType result = f(x);

            typename CacheType::iterator it = cache.find(x);

            if (it == cache.end()) {            // Item is not in the cache
                result = f(x);                  // Evaluate function's return value

                if ( cache.size() == cacheSize ) { // Cache is at capacity
                    // Remove the oldest cached item
                    InputType discardedItem = valuesCurrentlyCached.front();
                    valuesCurrentlyCached.pop();
                    cache.erase(discardedItem);
                    #if DEBUG
                        std::cout << "Item removed from cache" << std::endl;
                    #endif
                }
                #if DEBUG
                    std::cout << "Item added to cache" << std::endl;
                #endif
                cache[x] = result;              // Cache return value
                valuesCurrentlyCached.push(x);  // Document that it's cached
            }
            else {                              // Item is in the cache
                result = cache[x];              // Retrieve the cached value
                #if DEBUG
                    std::cout << "Item retrieved from cache" << std::endl;
                #endif
            }
            return result;
        };
    }
    return f;                                   // Case where no cache to be used
}


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

#include <iostream>
#include <vector>
int main(int argc, char** argv){

    auto mem_f = memoize<int, int>(f, 2);
    int x = 4;
    std::cout << mem_f(x) << std::endl;     // f(x) is cached
    std::cout << mem_f(x) << std::endl;     // f(x) is retrieved

    std::cout << mem_f(2*x) << std::endl;   // f(2x) is cached

    std::cout << mem_f(3*x) << std::endl;   // f(3x) is cached, f(x) is discarded

    std::cout << mem_f(x) << std::endl;     // f(x) is cached, f(2x) is discarded

    std::cout << mem_f(3*x) << std::endl;   // f(3x) is retrieved


    std::cout << "\n\n";
    auto mem_g = memoize<char*, int>(g, 1);
    char c = 'c';
    std::cout << mem_g(&c) << std::endl;

    // Modifying the value at the address will have no effect on the output
    // if the address is in the cache
    c = 'd';
    std::cout << mem_g(&c) << std::endl;


    std::cout << "\n\n";
    auto mem_h = memoize<std::vector<int>*, int*>(h<int>,2);
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
