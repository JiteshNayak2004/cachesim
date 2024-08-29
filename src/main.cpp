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


tasks:
1. convert direct map as a special case of set associative
have a function to decide no of ways
2. have a function for cache eviction
3. make IsWayHit support associativity
*/

#include <bitset>
#include <cstdlib>
#include <deque>
#include <ostream>
#include <queue>
#include <string>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <vector>


// global metrics
int num_loads=0;
int num_stores=0;
int num_cache_hits=0;
int num_load_hits=0;
int num_store_hits=0;
int num_mem_accesses=0;


//initialization variables
int cache_blocks=1024;
int cache_block_size=4;
int associativity=1;



using namespace std;

// structure of a write back cache line
struct cache_entry {

    bool valid;
    bool dirty;
    vector<bool> tag;
    vector<bool> data;
};


// structure of a set associative cache
struct set_associative {


    //hold's the queue of block's indices for LRU eviction
    deque<int> LRU;
    // as every way of a set is like a cache entry only
    vector<cache_entry> ways;



};

// Function to initialize cache entries with 0s 

void initialize_cache(cache_entry *cache, int cache_index, int cache_tag, int cache_block_size) {

    for (int i = 0; i < cache_index; i++) {
	// Initialize valid and dirty bits with random true/false
	cache[i].valid=0;
	cache[i].dirty=0;

	// Initialize tag with 0s
	for (int j = 0; j < cache_tag; j++) {
	    cache[i].tag[j] = 0;
	}

	// Initialize data with random bits
	for (int j = 0; j < cache_block_size * 8; j++) {
	    cache[i].data[j] = 0;
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

// extracts tag from address 
vector<bool> Addr2CacheTag(int cache_lines,string address) {

    bitset<32> bit_address;
    int decimal_address;

    // bits to encode no of cache lines
    int bits_cache_lines=ceil(log2(cache_lines));
    //converting the hex addr in trace to bits
    decimal_address=stoi(address, nullptr, 16);

    //finding index the mem_address corresponds to in the cache
    int address_index=decimal_address%cache_lines;

    // address in 0s and 1s
    std::bitset<32> binary_address(decimal_address);

    // extracting the tag bits from msb to lsb from address
    vector<bool> address_tag;

    // i>bits_cache_lines is critical
    for(int i=31;i>bits_cache_lines;i--) {
	address_tag.push_back(binary_address[i]);
    }

    return address_tag;
}

// extracts index from address
int Addr2CacheIndex(int cache_lines,string address) {

    // bits to encode no of cache lines
    int bits_cache_lines=ceil(log2(cache_lines));
    //converting the hex addr in trace to bits
    int decimal_address=stoi(address, nullptr, 16);

    //finding index the mem_address corresponds to in the cache
    int address_index=decimal_address%cache_lines;

    return address_index;


}


// cache entry lookup operation:1 indicates hit and 0 is not a hit
bool IsWayHit(cache_entry  *way, string  address,int cache_lines) {


    vector<bool> address_tag;
    address_tag=Addr2CacheTag(cache_lines,address);

    int address_index;
    address_index=Addr2CacheIndex(cache_lines,address);

    // checking valid and tag fields 
    if(way[address_index].valid==1) {

	if(way[address_index].tag==address_tag) 
	    return 1;
	else 
	    return 0;
    }

    else
    return 0;


}

//a function that takes in address and extracts the tag 
//and changes it along with the valid bit

void set_way_tag(cache_entry *cache, string address,int cache_lines) {


    int address_index;
    address_index=Addr2CacheIndex(cache_lines,address);

    vector<bool> address_tag;
    address_tag=Addr2CacheTag(cache_lines,address);

    //setting the tag of the corresponding cache_line
    cache[address_index].tag=address_tag;

    //setting the valid bit of the corresponding cach_line

    cache[address_index].valid=1;
}



// set associative version of CacheGet

void CacheGet(set_associative *cache,string address,int associativity,int total_cache_lines) {

	int num_loads=num_loads+1;
	int hit_flag;
	int way_hit;
    
	//set index the address maps to also total_cache_lines=total no of sets
	int AddrSetIndex=Addr2CacheIndex( total_cache_lines, address);
	


	// finding hit or no and which way
	for(int i=0;i<associativity;i++) {
	
		//we search all the ways of the set the address maps to
		if(IsWayHit(&(cache[AddrSetIndex].ways[i]),address,total_cache_lines)){
			hit_flag=1;
			way_hit=i;
		}
	}

	if(hit_flag) {

		int num_cache_hits=num_cache_hits+1;
		int num_load_hits=num_load_hits+1;
	}

	else {
		int num_mem_accesses=num_mem_accesses+1;
		
		// have to check all the ways are they filled
		// if not place block(write tag) in that way
		// if full evict LRU block and write in that
		// way

		for(int i=0;i<associativity;i++) {

			if(cache[AddrSetIndex].ways[i].valid==0) {
			
				//make the way valid and place block
				cache[AddrSetIndex].ways[i].valid=1;
				set_way_tag(&(cache[AddrSetIndex].ways[i]), address, total_cache_lines);
				
				//storing block index in LRU queue
				cache->LRU.push_front(i);
				break;
			}

			else {
				// evict LRU block
				int LRU_way=cache->LRU.back();
				cache->LRU.pop_back();

				// and then set the tag for that way
			}

		

		}

	}




}




// checks whether block exists in a way of the set associative cache
// needs one rewrite honestly
void CacheGet(cache_entry *cache,string address,int cache_lines) {

    num_loads=num_loads+1;

    int hit_status=IsWayHit(cache,address,cache_lines);

    if(hit_status) {

	num_cache_hits=num_cache_hits+1;
	num_load_hits=num_load_hits+1;


    }

    else {
	// as a miss need to access memory for a block
	num_mem_accesses=num_mem_accesses+1;

	//need to write the tag of the fetch block on cache data not required as pointless
	set_way_tag(cache,address,cache_lines);

    }


}



// cache set for store operations
void CacheSet(cache_entry *cache,string address,int cache_lines) {


    num_stores=num_stores+1;
    int cache_index=Addr2CacheIndex(cache_lines,address);
    int hit_status=IsWayHit(cache,address,cache_lines);

    if(hit_status&&cache[cache_index].dirty) {
	num_cache_hits=num_cache_hits+1;
	num_store_hits=num_store_hits+1;
    }
    else {
	// miss meaning need to go to lower level of memory which is memory here
	num_mem_accesses=num_mem_accesses+1;

	// setting the tag for the fetched block
	set_way_tag(cache, address, cache_lines);
	
	//setting the dirty bit
	cache[cache_index].dirty=1;
    }

}

// a function that show's cache hit/miss statistics

void cache_result() {

    cout<<"num_loads are "<<num_loads<<endl;
    cout<<"num_stores are "<<num_stores<<endl;
    cout<<"num_cache_hits are "<<num_cache_hits<<endl;
    cout<<"num_load_hits are "<<num_load_hits<<endl;
    cout<<"num_store_hits are "<<num_store_hits<<endl;
    cout<<"num_mem_accesses are "<<num_mem_accesses<<endl;


}


int main() {



    // all direct mapping essential value finding 

    //finding no of words in a block and then taking log2
    int cache_index=log2(cache_blocks);

    //finding no of words in a block and then taking log2
    int word_offset=log2(cache_block_size/4);

    // every word is 4 bytes thus 2 bits 
    int byte_offset=2;
    int cache_tag=(32-(cache_index+word_offset+byte_offset));


    int associativity=3;


    // all set associative essential value finding
    
    int set_index=log2(cache_blocks/associativity);
    int set_tag=(32-(set_index+word_offset+byte_offset));

    // creating the set associative structure
    
    struct set_associative associative_cache[set_index];
    
    //resizing the ways according to associativity
    for(int i=0;i<set_index;i++) {

	associative_cache[i].ways.resize(associativity);
    }


    cout << "--------------------------" << endl;
    cout << "| Number of cache blocks | " << cache_blocks << endl;
    cout << "| Cache block size       | " << cache_block_size << endl;
    cout << "| Tag size               | " << cache_tag << endl;
    cout << "| Index size             | " << cache_index << endl;
    cout << "--------------------------" << endl;


    // creating the cache structure convert into a function later

    struct cache_entry cache[cache_index];

    for(int i=0;i<cache_index;i++) {

	cache[i].data.resize(cache_block_size*8);
	cache[i].tag.resize(cache_tag);

    }


    // Initialize cache with random values
    initialize_cache(cache, cache_index, cache_tag, cache_block_size);


    CacheSet(cache,"12341234",10);
    CacheSet(cache,"12341234",10);
    print_cache_contents(cache, cache_index, cache_tag, cache_block_size);



    cache_result();
    return 0;



}
