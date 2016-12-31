#pragma once

#include <stdint.h>

enum KLFType {
	T_INTEGER,
	T_FLOAT,
	T_BOOLEAN,
	T_TIME
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
		Type = T_INTEGER;
		value.Int = v;
	}

	KLFValue(float v) {
		Type = T_FLOAT;
		value.Float = v;
	}

	KLFValue(bool v) {
		Type = T_BOOLEAN;
		value.Bool = v;
	}

	KLFValue(ktime_t v) {
		Type = T_TIME;
		value.Time = v;
	}

	float FloatValue() {
		switch (Type) {
		case T_INTEGER: return value.Int;
		case T_FLOAT: return value.Float;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return 0;
		}
	}

	int IntValue() {
		switch (Type) {
		case T_INTEGER: return value.Int;
		case T_FLOAT: return value.Float;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return 0;
		}
	}

	bool BoolValue() {
		switch (Type) {
		case T_INTEGER: return value.Int != 0;
		case T_FLOAT: return value.Float != 0;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return 0;
		}
	}


	ktime_t TimeValue() {
		ktime_t t;
		switch (Type) {
		case T_INTEGER: t.frame = value.Int; break;
		case T_FLOAT: t.frame = value.Float; break;
		case T_BOOLEAN: t.frame = value.Bool; break;
		case T_TIME: return value.Time;
		}
		return t;
	}

	KLFValue operator+ (KLFValue &v) {
		if (Type == T_TIME) {
//			return KLFValue(TimeValue() + v.IntValue());
		}
		if (v.Type == T_TIME) {
//			return KLFValue(v.TimeValue() + IntValue());
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() + v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() + v.IntValue());
		}
		return KLFValue();
	}

};