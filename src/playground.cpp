#include <string>
using namespace std;

void CacheGet(set_associative *cache,string address,int associativity,int cache_lines) {

	int num_loads=num_loads+1;
	int hit_flag;
	int correct_way;

	// finding hit or no and which way
	for(int i=0;i<associativity;i++) {

		if(Iswayhit(cache.ways[i],address,cache_lines)){
			hit_flag=1;
			correct_way=i;
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

			if(cache.ways[i].valid==0) {
			
				//make the way valid and place block
				cache.ways[i].valid==1;
				set_way_tag(cache.way,address,cache_lines);
				
				//storing block index in LRU
				cache.LRU.enqueue(i);
				break;
			}

			else {
				// evict LRU block
				int LRU_way=cache.LRU.dequeue	

				// and then set the tag for that way
			}

		

		}

	}




}

