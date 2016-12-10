#pragma once

#include <stdint.h>

enum KLFType {
	INTEGER,
	FLOAT,
	BOOLEAN,
	TIME
};

struct ktime_t {
	uint16_t frame;
	uint16_t second;
	uint16_t minute;
};
typedef uint16_t type_t;

class KLFValue {
public:
	union {
		bool Bool;
		int Int;
		float Float;
//		double Double;
		ktime_t Time;
	} value;
	type_t Type;

	KLFValue(int v=0) {
		Type = INTEGER;
		value.Int = v;
	}

	KLFValue(float v) {
		Type = FLOAT;
		value.Float = v;
	}

	KLFValue(bool v) {
		Type = BOOLEAN;
		value.Bool = v;
	}

	KLFValue(ktime_t v) {
		Type = TIME;
		value.Time = v;
	}

	float FloatValue() {
		switch (Type) {
		case INTEGER: return value.Int;
		case FLOAT: return value.Float;
		case BOOLEAN: return value.Bool;
		case TIME: return 0;
		}
	}

	int IntValue() {
		switch (Type) {
		case INTEGER: return value.Int;
		case FLOAT: return value.Float;
		case BOOLEAN: return value.Bool;
		case TIME: return 0;
		}
	}

	bool BoolValue() {
		switch (Type) {
		case INTEGER: return value.Int != 0;
		case FLOAT: return value.Float != 0;
		case BOOLEAN: return value.Bool;
		case TIME: return 0;
		}
	}


	ktime_t TimeValue() {
		ktime_t t;
		switch (Type) {
		case INTEGER: t.frame = value.Int; break;
		case FLOAT: t.frame = value.Float; break;
		case BOOLEAN: t.frame = value.Bool; break;
		case TIME: return value.Time;
		}
		return t;
	}

	KLFValue operator+ (KLFValue &v) {
		if (Type == TIME) {
//			return KLFValue(TimeValue() + v.IntValue());
		}
		if (v.Type == TIME) {
//			return KLFValue(v.TimeValue() + IntValue());
		}
		if (Type == FLOAT || v.Type == FLOAT) {
			return KLFValue(FloatValue() + v.FloatValue());
		}
		if (Type == INTEGER || v.Type == INTEGER) {
			return KLFValue(IntValue() + v.IntValue());
		}
		return KLFValue();
	}

};