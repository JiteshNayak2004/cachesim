/*

current plans: simulate a write through based direct mapped cache
future plans:
1. multilevel set associative cache
2. simulating coherency some sorta

thoughts:

1.while modelling cache fetching from memory wagera is actually not needed
2.what we intend to do is say there's a miss that entry we make it such that
we make note of misses and hits and update just the tag field data isn't 
relevant
*/

#include <bitset>
#include <cstdlib>
#include <ostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <vector>


using namespace std;

// structure of a write back cache line
struct cache_entry {
	bool valid;
	bool dirty;
	vector<bool> tag;
	vector<bool> data;
};

struct memory_entry {
    bitset<32> address;
    int value;
};

// Function to initialize cache entries with random values
void initialize_cache(cache_entry *cache, int cache_index, int cache_tag, int cache_block_size) {
    srand(time(0)); // Seed for random number generation

    for (int i = 0; i < cache_index; i++) {
        // Initialize valid and dirty bits with random true/false
        cache[i].valid=rand()%2;
        cache[i].dirty=rand()%2;

        // Initialize tag with random bits
        for (int j = 0; j < cache_tag; j++) {
            cache[i].tag[j] = rand() % 2;
        }

        // Initialize data with random bits
        for (int j = 0; j < cache_block_size * 8; j++) {
            cache[i].data[j] = rand() % 2;
        }
    }
}

// Function to display the cache contents in a nicely formatted way
void print_cache_contents(cache_entry *cache, int cache_index, int cache_tag, int cache_block_size) {
    cout << "Index | Valid | Dirty | Tag                             | Data\n";
    cout << "---------------------------------------------------------------------\n";

    for (int i = 0; i < cache_index; i++) {
        cout << i << "     | " << cache[i].valid << "     | " << cache[i].dirty << "     | ";

        // Print tag
        for (int j = 0; j < cache_tag; j++) {
            cout << cache[i].tag[j];
        }
        cout << " | ";

        // Print data
        for (int j = 0; j < cache_block_size * 8; j++) {
            cout << cache[i].data[j];
        }
        cout << "\n";
    }
}

bool IsCacheHit(cache_entry  *cache, string  address,int cache_lines) {


    bitset<32> bit_address;
    int decimal_address;

    // bits to encode no of cache lines
    int bits_cache_lines=ceil(log2(cache_lines));
    //converting the hex addr in trace to bits
    decimal_address=stoi(address, nullptr, 16);

    //finding index the mem_address corresponds to in the cache
    int address_index=decimal_address%cache_lines;

    cout<<"the address maps to this index in cache "<<(address_index)<<endl; 
    cout<<"the decimal equivalent of addr is "<<decimal_address<<endl;

    // address in 0s and 1s
    std::bitset<32> binary_address(decimal_address);
   

    // extracting the tag bits from msb to lsb from address
    vector<bool> address_tag;
    for(int i=31;i<bits_cache_lines;i--) {

	address_tag.push_back(binary_address[i]);
    }

    // checking valid and tag fields
    if(cache[address_index].valid==1) {
	
	if(cache[address_index].tag==address_tag) {

	    return 1;
	}

	else {
	    return 0;
	}

    }

    else{
	return 0;
    }

}

int main() {

	
	//initialization variables
	int cache_blocks=1024;
	int cache_block_size=4;
	int associativity=1;
	
	// cout<<"welcome to cache sim";
	//
	//
	//
	// cout<<"enter no of blocks in cache ";
	// cin>>cache_blocks;
	// 
	// cout<<"enter block size in bytes";
	// cin>>cache_block_size;
	//
	//
	// cout<<"enter associativity";
	// cin>>associativity;
	
	
	//finding no of words in a block and then taking log2
	int cache_index=log2(cache_blocks);
	
	//finding no of words in a block and then taking log2
	int word_offset=log2(cache_block_size/4);

	// every word is 4 bytes thus 2 bits 
	int byte_offset=2;
	int cache_tag=(32-(cache_index+word_offset+byte_offset));


	cout<<"no of cache blocks are "<<cache_blocks<<endl;
	cout<<"cache block size is "<<cache_block_size<< "bytes"<<endl;
	cout<<"the tag is "<<cache_tag<<" bits long"<<endl;
	cout<<"the index is "<<cache_index<<" bits long"<<endl;

	
	// resizing the struct's data and tag elements
	
	struct cache_entry cache[cache_index];

	for(int i=0;i<cache_index;i++) {

		cache[i].data.resize(cache_block_size*8);
		cache[i].tag.resize(cache_tag);

	}
	
	//printing the modified sizes
	
	// for(int i=0;i<cache_index;i++) {
	// 
	//
	// 	cout<<"the size of tag is "<<size(cache[i].tag)<<endl;
	// 	cout<<"the size of data in cache is "<<size(cache[i].data)<<endl;
	//
	// }
	//

	// Initialize cache with random values
	initialize_cache(cache, cache_index, cache_tag, cache_block_size);

	// Print the modified sizes and contents
	print_cache_contents(cache, cache_index, cache_tag, cache_block_size);


	
	IsCacheHit(cache,"f" ,10);








	return 0;



}
