#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <linux/perf_event.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <chrono>
#include <unordered_set>
#include <regex>
#include <algorithm>
// #include "src/Common/ProgressIndication.h"


#define PERF_TYPE_HARDWARE 0x0
#define PERF_COUNT_HW_CPU_CYCLES 0x3
#define PERF_COUNT_HW_INSTRUCTIONS 0x0
#define PERF_COUNT_HW_CACHE_REFERENCES 0x6
#define PERF_COUNT_HW_CACHE_MISSES 0x8
#define PERF_TYPE_SOFTWARE 0x1
#define PERF_COUNT_SW_PAGE_FAULTS 0x2
#define PERF_COUNT_SW_DISK_READS 0x4
#define PERF_COUNT_SW_DISK_WRITES 0x5

namespace coster {

	class QueryCoster {
	public:
		struct Metrics
		{
			
			uint64_t elapsed_us_;

			uint64_t cpu_time_us_ref;

			uint64_t cpu_time_us_;

			uint64_t cpu_cycles_;

			uint64_t instructions_;

			uint64_t cache_references_;

			uint64_t cache_misses_;

			uint16_t disk_block_reads;

			uint16_t disk_block_writes;

			uint64_t memory_b_;

			uint64_t memory_b_max;

			int isused_avx;//

		};
		QueryCoster()//
		{
			// perf_event_attr 
			cycles_us=0;
			attrs[0] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
			attrs[1] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
    		attrs[2] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);
    		attrs[3] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);
    		attrs[4] = create_perf_event_attr(PERF_TYPE_SOFTWARE, PERF_COUNT_SW_DISK_READS);
			attrs[5] = create_perf_event_attr(PERF_TYPE_SOFTWARE, PERF_COUNT_SW_DISK_WRITES);

		}
		~QueryCoster()//
		{
			writeCSV("data.csv");
			close(fd);
			close(fd_instructions);
			close(fd_cache_references);
			close(fd_cache_misses);
			close(fd_disk_reads);
			close(fd_disk_writes);
			//
		}
		void CPUTime();
		void CPUCycles();
		void CPUInstructions();
		void CPUCacheReferences();
		void CPUCacheMisses();
		void DiskBlockReads();
		void DiskBlockWrites();
		void MemoryConsumption();
		void start();
		void stop();
		void writeCSV(const std::string& filename);
		Metrics metrics;
		struct perf_event_attr attrs[7];
		int fd;
		int fd_instructions;
		int fd_cache_references;
		int fd_disk_reads;
		int fd_disk_writes;
		int fd_cache_misses;
		uint64_t start_time_microseconds;
		uint64_t event_time_microseconds;
		uint64_t query_time_microseconds;
		std::string model_name_;
		uint64_t ref_cycles_us_;
		uint64_t cycles_us;
		uint64_t memory_usage_start;
		uint64_t max_host_usage_start;
		uint64_t memory_usage_event;
		uint64_t max_host_usage_event;
		
		inline uint64_t timeInMicroseconds(std::chrono::time_point<std::chrono::system_clock> timepoint)
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(timepoint.time_since_epoch()).count();
		}
		struct perf_event_attr create_perf_event_attr(uint32_t type, uint64_t config)
		{
    		struct perf_event_attr attr;
    		memset(&attr, 0, sizeof(attr));
    		attr.size = sizeof(attr);
    		attr.type = type;
    		attr.config = config;
    		attr.disabled = 1;
    		attr.exclude_kernel = 1;
    		attr.exclude_hv = 1;
    		// attr.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
    		return attr;
		}
	};
}
