#pragma once

class InputInfo
{
public:
	enum ThruMode {
		NO_THRU = 0,
		ON_SELECT, // output track is only 
		MANUAL // output track is only set manually on i/f or in script
	};

	InputInfo(int _pin, bool _stereo, ThruMode _mode)
		: pin(_pin), isStereo(_stereo), thruMode(_mode), thruTrack(-1) {}

	int pin; /** vst pin assigned */
	bool isStereo; /** if this is stereo (will use 2 vst pins) */
	ThruMode thruMode; /** how we select the thru track on this input */
	int thruTrack; /** current track receiving thru, -1 if not echoed */

	static ThruMode findMode(const string & nm);
	static string findMode(const ThruMode nm);
};