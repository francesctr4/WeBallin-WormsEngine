#pragma once

#include "Globals.h"

class PerfTimer
{
public:

	// Constructor
	PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64 startTime;
	static uint64 frequency;
};