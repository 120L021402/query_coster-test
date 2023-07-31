#include "query_coster.h"
int main()
{
	coster::QueryCoster a;
	a.start();
	for(int i;i<=10000000;i++){

	}
	a.stop();
	std::cout<<"elapsed_us_="<<a.metrics.elapsed_us_<<"\n";
	std::cout<<"cycles_us="<<a.cycles_us<<"\n";
	std::cout<<"cpu_time_us_="<<a.metrics.cpu_time_us_<<"\n";
	std::cout<<"cpu_cycles_="<<a.metrics.cpu_cycles_<<"\n";
	std::cout<<"cache_references_="<<a.metrics.cache_references_<<"\n";
	std::cout<<"cache_misses_="<<a.metrics.cache_misses_<<"\n";
	std::cout<<"instructions_="<<a.metrics.instructions_<<"\n";
	std::cout<<"disk_block_reads="<<a.metrics.disk_block_reads<<"\n";
	std::cout<<"disk_block_writes="<<a.metrics.disk_block_writes<<"\n";
	std::cout<<"isused_avx="<<a.metrics.isused_avx<<"\n";
}