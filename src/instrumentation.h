#include <chrono>
#define TimeNow() std::chrono::high_resolution_clock::now()
#define Timelapse(diff) std::chrono::duration_cast<std::chrono::microseconds>(end - start)
