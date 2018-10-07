#pragma once
#include <iostream>
const unsigned int secondsPerMinute = 60;
const unsigned int minutesPerHour = 60;
const unsigned int HoursPerDay = 24;
inline void writeTime(unsigned int seconds)
{
	auto days = seconds / (secondsPerMinute*minutesPerHour*HoursPerDay);
	seconds = seconds % (secondsPerMinute*minutesPerHour*HoursPerDay);

	auto hours = seconds / (secondsPerMinute*minutesPerHour);
	seconds = seconds % (secondsPerMinute*minutesPerHour);

	auto minutes = seconds / (secondsPerMinute);
	seconds = seconds % (secondsPerMinute);

	if (days != 0)
	{
		std::cout << days << " days, ";
	}
	if (hours != 0)
	{
		std::cout << hours << " hours, ";
	}
	if (minutes != 0)
	{
		std::cout << minutes << " minutes and ";
	}
	std::cout << seconds << " seconds" << std::endl;
}