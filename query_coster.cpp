#include "query_coster.h"


namespace coster {

	void QueryCoster::CPUTime(){
		std::string line;
		std::ifstream infile("/proc/cpuinfo");
		while (std::getline(infile, line)) {
			// printf("%s",line.c_str());
			// llvm::StringRef str(line);

			// auto [name, value] = str.split(":");
			// value = value.trim(" ");

			// if (name.startswith("model name")) {
			// 	model_name_ = value.str();
			// 	std::regex cpu_freq_regex("\\s[\\d.]+GHz");
			// 	std::cmatch m;
			// 	std::regex_search(model_name_.c_str(), m, cpu_freq_regex);
			// 	if (!m.empty()) {
			// 		double base_cpu_ghz = std::stod(m[0].str());
			// 		ref_cycles_us_ = static_cast<uint64_t>(base_cpu_ghz * 1000);
			// 	}
			// }
			// else if (name.startswith("cpu MHz")) {
			// 	uint64_t cpu_mhz;
			// 	value.getAsDouble(cpu_mhz);
			// 	cycles_us = std::max(cycles_us, cpu_mhz);
			// }
			std::istringstream iss(line);
    		std::string name, value;
    		std::getline(iss, name, ':'); // 分割字符串，将结果存入 name
    		std::getline(iss, value);     // 获取冒号后的内容，将结果存入 value
			// 去除 value 字符串的首尾空格（可选，根据需要进行去除空格）
			name.erase(name.begin(), std::find_if(name.begin(), name.end(), [](unsigned char ch) {
        	return !std::isspace(ch);
   			}));
    		name.erase(std::find_if(name.rbegin(), name.rend(), [](unsigned char ch) {
        	return !std::isspace(ch);
    		}).base(), name.end());
    		value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        	return !std::isspace(ch);
   			}));
    		value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        	return !std::isspace(ch);
    		}).base(), value.end());
			if (name=="cpu MHz") {
				uint64_t cpu_mhz;
				cpu_mhz=std::stoull(value);
				cycles_us = std::max(cycles_us, cpu_mhz);
			}
			
		}
		if(ref_cycles_us_!=0){
			metrics.cpu_time_us_ref = metrics.cpu_cycles_ / ref_cycles_us_;
		}
		if(cycles_us!=0){
			metrics.cpu_time_us_ = metrics.cpu_cycles_ / cycles_us;
		}

		
		return;
	}

	void QueryCoster::CPUCycles() {
		fd = syscall(__NR_perf_event_open, &attrs[0], 0, -1, -1, 0);
		if (fd == -1) {
			printf("Failed to create perf event - CPU cycles\n");
			return;
		}
		ioctl(fd, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
		return;
	}
	void QueryCoster::CPUInstructions() {
		fd_instructions = syscall(__NR_perf_event_open, &attrs[1], 0, -1, -1, 0);
		if (fd_instructions == -1) {
			printf("Failed to create perf event - CPU instructions\n");
			return;
		}
		ioctl(fd_instructions, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_instructions, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::CPUCacheReferences() {
		fd_cache_references = syscall(__NR_perf_event_open, &attrs[2], 0, -1, -1, 0);
		if (fd_cache_references == -1) {
			printf("Failed to create perf event - CPU cache references\n");
			return;
		}
		ioctl(fd_cache_references, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_cache_references, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::CPUCacheMisses() {
		fd_cache_misses = syscall(__NR_perf_event_open, &attrs[3], 0, -1, -1, 0);
		if (fd_cache_misses == -1) {
			printf("Failed to create perf event - CPU cache misses\n");
			return;
		}
		ioctl(fd_cache_misses, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_cache_misses, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::DiskBlockReads() {
		fd_disk_reads = syscall(__NR_perf_event_open, &attrs[4], 0, -1, -1, 0);
		if (fd_disk_reads == -1) {
			printf("Failed to create perf event - Disk block reads\n");
			return;
		}
		ioctl(fd_disk_reads, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_disk_reads, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::DiskBlockWrites() {
		fd_disk_writes = syscall(__NR_perf_event_open, &attrs[5], 0, -1, -1, 0);
		if (fd_disk_writes == -1) {
			printf("Failed to create perf event - Disk block writes\n");
			return;
		}
		ioctl(fd_disk_writes, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_disk_writes, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::MemoryConsumption() {
		metrics.memory_b_max = max_host_usage_event - max_host_usage_start;
		metrics.memory_b_ = memory_usage_start - memory_usage_event;
	}
	void QueryCoster::start() {
		CPUCycles();
		CPUInstructions();
		CPUCacheReferences();
		CPUCacheMisses();
		DiskBlockReads();
		DiskBlockWrites();
		auto query_start_time = std::chrono::system_clock::now();
		start_time_microseconds = timeInMicroseconds(query_start_time);
		// MemoryUsage memoryusage_start = ProgressIndication::getMemoryUsage();
		// memory_usage_start = memoryusage_start.total;
		// max_host_usage_start = memoryusage_start.max;
		return;
	}

	void QueryCoster::stop() {
		auto query_finish_time = std::chrono::system_clock::now();
		event_time_microseconds = timeInMicroseconds(query_finish_time);
		query_time_microseconds = event_time_microseconds - start_time_microseconds;
		metrics.elapsed_us_=query_time_microseconds;
		ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_instructions, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_cache_references, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_cache_misses, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_disk_reads, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_disk_writes, PERF_EVENT_IOC_DISABLE, 0);
		read(fd, &metrics.cpu_cycles_, sizeof(metrics.cpu_cycles_));
		read(fd_instructions, &metrics.instructions_, sizeof(metrics.instructions_));
		read(fd_cache_references, &metrics.cache_references_, sizeof(metrics.cache_references_));
		read(fd_cache_misses, &metrics.cache_misses_, sizeof(metrics.cache_misses_));
		read(fd_disk_reads, &metrics.disk_block_reads, sizeof(metrics.disk_block_reads));
		read(fd_disk_writes, &metrics.disk_block_writes, sizeof(metrics.disk_block_writes));
		CPUTime();
		// MemoryUsage memoryusage_event = ProgressIndication::getMemoryUsage();
		// memory_usage_event = memoryusage_event.total;
		// max_host_usage_event = memoryusage_event.max;
		// MemoryConsumption();
		return;
	}

	void QueryCoster::writeCSV(const std::string& filename) {
		std::ofstream file(filename);
		if (file.is_open()) {
			file << "elapsed_us_,cpu_time_us_ref,cpu_time_us_,cpu_cycles_,instructions_,cache_references_,cache_misses_,disk_block_reads,disk_block_writes,memory_b_max,memory_b,isused_avx" << std::endl;
			file << metrics.elapsed_us_ << "," <<metrics.cpu_time_us_ref<<"," << metrics.cpu_time_us_ << "," << metrics.cpu_cycles_ << "," << metrics.instructions_ << "," << metrics.cache_references_ << "," << metrics.cache_misses_ << "," << metrics.disk_block_reads << "," << metrics.disk_block_writes << "," << metrics.memory_b_max <<"," <<metrics.memory_b_<< "," << metrics.isused_avx << std::endl;
			file.close();
		}
		else {
			std::cout << "open failed";
		}
	}

}
