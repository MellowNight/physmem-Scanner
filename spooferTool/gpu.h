#pragma once
#include "utils.h"




void	spoofGPU()
{
	/*	delete nvml.dll and delete 	nvidia-smi here	*/


	std::remove("C:\\Windows\\System32\\nvml.dll");
	std::remove("C:\\Windows\\System32\\nvidia-smi.exe");
	std::remove("C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvml.dll");
	std::remove("C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvidia-smi.exe");

	return;
}