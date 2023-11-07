// #include <linkedlist.h>
#include <functional>
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string> 
#include <string.h> 
#include <vector> 
#include <iostream> 
#include "pack109final.hpp"


using namespace std; 


class hashset {
  private:
    
    File* buckets; 

    // The number of buckets in the array
    size_t size; 

    // Generate a prehash for an item with a given size
    unsigned long prehash(string name);

  public:
    
    // Initialize an empty hash set, where size is the number of buckets in the array
    hashset(size_t size);
    hashset();
    // Free all memory allocated by the hash set
    ~hashset();

    // Hash an unsigned long into an index that fits into a hash set
    unsigned long hash(string name);

    // Insert item in the set. Return true if the item was inserted, false if it wasn't (i.e. it was already in the set)
    // Recalculate the load factor after each successful insert (round to nearest whole number).
    // If the load factor exceeds 70 after insert, resize the table to hold twice the number of buckets.
    bool insert(string key, File value);

    File get(string key); 

    // Remove an item from the set. Return true if it was removed, false if it wasn't (i.e. it wasn't in the set to begin with)
    bool remove(string name);

    // Return true if the item exists in the set, false otherwise
    bool contains(string name);

    // Resize the underlying table to the given size. Recalculate the load factor after resize
    void resize(size_t new_size);

    // Returns the number of items in the hash set
    size_t len();

    // Returns the number of items the hash set can hold before reallocating
    size_t capacity();

    // Print Table. You can do this in a way that helps you implement your hash set.
    void print();

};



unsigned long hashset::prehash(string name) 
{

    unsigned long h = 5381; 
    int k; 
    char* nameRef = (char*)name.c_str(); 
    while(k == *nameRef++){
        h = ((h << 5) + h) + k; 
    }
    return h; 
    
}


hashset::hashset(size_t size)
{
    this->size = size; 
    this->buckets = new File[size]; 
    for(int i = 0; i < size; i++){
        File current; 
        current.name = ""; 
        vec content; 
        current.bytes = content; 
        buckets[i] = current; 
    }
}


hashset::hashset()
{
    size = 0; 
    buckets = new File[10]; 

}


hashset::~hashset()
{
  free(this->buckets); 

}


unsigned long hashset::hash(string name)
{
  unsigned long pre = this->prehash(name);
  unsigned long s = (unsigned long)(pre%this->size-1);
  return s;
}


bool hashset::insert(string key, File value)
{
  
    bool check = false; 
    unsigned long index = hash(key); 
    if(this->contains(key) == true){
        if(buckets[index].name.compare(key) != 0){
            resize(this->size*2); 
            insert(key, value); 
        }else{
            check = true; 
            this->buckets[index].name = value.name; 
            this->buckets[index].bytes.clear(); 
            for(int i = 0; i < value.bytes.size(); i++){
                this->buckets[index].bytes.push_back(value.bytes.at(i)); 
            }

        }
    }else{
        this->buckets[index].name = value.name; 
        for(int i = 0; i < value.bytes.size(); i++){
            buckets[index].bytes.push_back(value.bytes.at(i));
        }

    }
    if(len() >=  (this->size*.5)){
        resize(this->size * 2); 
    }
    
    return check; 
}

File hashset::get(string key){
  
  unsigned long hashed = hash(key); 
  
  if(this->contains(key)){
    return this->buckets[hashed]; 
  }else{
    
    throw; 
  }
}



bool hashset::contains(string name)
{
  unsigned long hashed = hash(name);
  if(this->buckets[hashed].bytes.size() != 0){
    
    return strcmp(this->buckets[hashed].name.c_str(), name.c_str()) == 0; 
  }
  return false; 
}


void hashset::resize(size_t new_size)
{
    File* old = this->buckets; 
    File* n = new File[new_size]; 
    this->size = new_size; 

    for(int i = 0; i < (int)new_size/2; i++){
        if(old[i].bytes.size() != 0){
            unsigned long hashed = hash(old[i].name); 
            n[hashed] = old[i]; 
        }

    }
    this->buckets = n; 
}


size_t hashset::len()
{
  size_t len = 0;
  for(int i =0;i<this->size;i++)
  {
    if(this->buckets[i].bytes.size() != 0){
        len++; 
    }
  }
  return len;
}



size_t hashset::capacity()
{
  return (size_t)(0.7*(double)this->size) - 1;
}


void hashset::print()
{
  for(int i =0;i<this->size;i++)
  {
    printf("%s\n", this->buckets[i].name.c_str()); 
  }
}
