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

			if(cache->ways[i].valid==0) {
			
				//make the way valid and place block
				cache->ways[i].valid==1;
				set_way_tag(cache->ways,address,cache_lines);
				
				//storing block index in LRU queue
				cache->LRU.push_front(i);
				break;
			}

			else {
				// evict LRU block
				int LRU_way=cache->LRU.pop_back();

				// and then set the tag for that way
			}

		

		}

	}




}

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


