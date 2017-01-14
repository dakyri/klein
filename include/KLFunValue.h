#pragma once

#include <stdint.h>
#include <string>
#include <sstream>

using namespace std;

const int sampleRate = 44100;

enum KLFType {
	T_INTEGER,
	T_FLOAT,
	T_BOOLEAN,
	T_STRING, // and garbage collection ??? for this and t_object
	T_OBJECT, 
		// ?? go javascripty for composites, an object as a key-value map, array as a key-value map with integer keys
		// implement as an unordered_map?
	T_FUNCTION,
	T_BLOCK,
	T_KLF,
	T_TIME,
	T_UNDEFINED
		// ???? FIXME TODO is it going to be useful to return undefined. perhaps this is splitting hairs for our klf microlanguage
};

/** ok. kind of reasonable. but will break on sample rate >= 96. we're good up to 48 (64) and that'll be ok for now */
struct ktime_t {
	uint16_t frame;
	uint16_t second;
	int16_t minute;

	void normalize() {
		while (frame >= sampleRate) {
			frame -= sampleRate;
			second++;
		}
		while (frame < 0) {
			frame += sampleRate;
			second--;
		}
		while (second >= 60) {
			second -= 60;
			minute++;
		}
		while (second < 0) {
			second += 60;
			minute--;
		}
	}

	long t2frame() {
		return ((minute * 60) + second) * sampleRate + frame;
	}

	void frame2t(long in) {
		frame = in % sampleRate;
		in = in / sampleRate;
		second = in % 60;
		minute = second / 60;
	}

	void string2t(string in) {
		int i1 = in.find_first_of(':');
		int i2 = in.find_first_of('.');
		if (i2 == in.npos) {
			if (i1 == in.npos) {
				minute = 0;
				second = 0;
				frame = stoi(in);
			} else {
				minute = stoi(in.substr(0, i1));
				second = stoi(in.substr(i1 + 1));
				frame = 0;
			}
		} else if (i1 == in.npos) {
			minute = 0;
			second = stoi(in.substr(0, i2));
			frame = stoi(in.substr(i2 + 1));
		} else {
			minute = stoi(in.substr(0, i1));
			second = stoi(in.substr(i1+1, i2));
			frame = stoi(in.substr(i2+1));
		}

		normalize();
	}
	
	ktime_t operator+(ktime_t t2) {
		ktime_t t1 = *this;
		t1.frame += t2.frame;
		t1.second += t2.second;
		t1.minute += t2.minute;
		t1.normalize();
		return t1;
	}

	ktime_t operator-(ktime_t t2) {
		ktime_t t1 = *this;
		t1.frame -= t2.frame;
		t1.second -= t2.second;
		t1.minute -= t2.minute;
		t1.normalize();
		return t1;
	}

	ktime_t operator*(float t2) {
		ktime_t t1;
		t1.frame2t(t2frame()*t2);
		return t1;
	}

	ktime_t operator/(float t2) {
		ktime_t t1;
		t1.frame2t(t2frame()/t2);
		return t1;
	}

	ktime_t operator%(int t2) {
		ktime_t t1;
		t1.frame2t(t2frame() % t2);
		return t1;
	}

	bool operator== (ktime_t t1) {
		return minute == t1.minute && second == t1.second && frame == t1.frame;
	}

	bool operator!= (ktime_t t1) {
		return minute != t1.minute || second == t1.second || frame == t1.frame;
	}

	bool operator< (ktime_t t1) {
		return minute < t1.minute || (minute == t1.minute && (second < t1.second || (second == t1.second && frame < t1.frame)));
	}

	bool operator> (ktime_t t1) {
		return minute > t1.minute || (minute == t1.minute && (second > t1.second || (second == t1.second && frame > t1.frame)));
	}

	bool operator<= (ktime_t t1) {
		return minute < t1.minute || (minute == t1.minute && (second < t1.second || (second == t1.second && frame <= t1.frame)));
	}

	bool operator>= (ktime_t t1) {
		return minute > t1.minute || (minute == t1.minute && (second > t1.second || (second == t1.second && frame >= t1.frame)));
	}



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
		class KLFun *Function;
		class KBlock *Block;
		class KLFObject *Object;
		string *String;
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

	KLFValue(const char *s) {
		Type = T_STRING;
		value.String = new string(s);
	}

	KLFValue(string &s) {
		Type = T_STRING;
		value.String = new string(s);
	}


	virtual ~KLFValue();

	static KLFValue forType(type_t);

	float FloatValue() const {
		switch (Type) {
		case T_INTEGER: return value.Int;
		case T_FLOAT: return value.Float;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return 60*value.Time.minute + value.Time.second + (value.Time.frame/((float)sampleRate));
		case T_STRING: return stof(*value.String);
		}
		return 0;
	}

	/**
	 * a specific cast for float control parameters (gain, feedback etc), which are valued from [0,1].
	 * integer values are assumed to be in from midi values (integral [0,127]) and are normalized
	 */
	float ControlFloatValue() const {
		switch (Type) {
		case T_INTEGER: return (value.Int/127.0);
		case T_FLOAT: return value.Float;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return 60 * value.Time.minute + value.Time.second + (value.Time.frame / ((float)sampleRate));
		case T_STRING: return stof(*value.String);
		}
		return 0;
	}

	int IntValue() const {
		switch (Type) {
		case T_INTEGER: return value.Int;
		case T_FLOAT: return value.Float;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return  60 * value.Time.minute + value.Time.second;
		case T_STRING: return stoi(*value.String);
		}
		return 0;
	}

	bool BoolValue() const {
		switch (Type) {
		case T_INTEGER: return value.Int != 0;
		case T_FLOAT: return value.Float != 0;
		case T_BOOLEAN: return value.Bool;
		case T_TIME: return value.Time.minute != 0 || value.Time.second != 0 || value.Time.frame != 0;;
		case T_STRING: return !value.String->empty();
		}
		return false;
	}


	ktime_t TimeValue() const {
		ktime_t t;
		switch (Type) {
		case T_INTEGER: t.frame = value.Int; break;
		case T_FLOAT: t.frame = value.Float; break;
		case T_BOOLEAN: t.frame = value.Bool; break;
		case T_TIME: return value.Time;
		case T_STRING: t.frame = stoi(*value.String);
		}
		return t;
	}

	string StringValue() const {
		stringstream ss;
		switch (Type) {
		case T_INTEGER: ss << value.Int; break;
		case T_FLOAT: ss << value.Float; break;
		case T_BOOLEAN: ss << (value.Bool? "true": "false");
		case T_TIME: ss << value.Time.minute << ":" << value.Time.second << "." << value.Time.frame;  break;
		case T_STRING: return *value.String; 
		}
		return string(ss.str());
	}

	bool operator() () {
		return BoolValue();
	}

	KLFValue operator= (const KLFValue &v) {
		switch (Type) {
		case T_INTEGER: value.Int = v.IntValue(); break;
		case T_FLOAT: value.Float = v.FloatValue(); break;
		case T_BOOLEAN: value.Bool = v.BoolValue(); break;
		case T_TIME: value.Time = v.TimeValue(); break;
		case T_STRING: *value.String = v.StringValue(); break;
		}
		return *this;
	}

	KLFValue operator+ (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() + v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 + t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() + v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() + v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() + v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator- (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue()/* - v.StringValue()*/);
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 - t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() - v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() - v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() - v.IntValue());
		}

		return KLFValue();
	}
	KLFValue operator* (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue()/* - v.StringValue()*/);
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			float t2 = v.FloatValue();
			return KLFValue(t1 * t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() * v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() * v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() * v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator/ (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue()/* - v.StringValue()*/);
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			float t2 = v.FloatValue();
			if (t2 == 0) return KLFValue();
			return KLFValue(t1 / t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			float vv = v.FloatValue();
			if (vv == 0) return KLFValue();
			return KLFValue(FloatValue() / vv);
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			int vv = v.FloatValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() / vv);
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			int vv = v.IntValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() / vv);
		}

		return KLFValue();
	}

	KLFValue operator% (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue()/* - v.StringValue()*/);
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			int t2 = v.IntValue();
			if (t2 == 0) return KLFValue();
			return KLFValue(t1 % t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			int vv = v.FloatValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() % vv);
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			int vv = v.FloatValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() % vv);
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			int vv = v.IntValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() % vv);
		}

		return KLFValue();
	}

	/* do we want a power operator or a bitwise operator. lets play around first
	KLFValue operator^ (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() - v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			int t2 = v.IntValue();
			if (t2 == 0) return KLFValue();
			return KLFValue(t1 % t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			int vv = v.FloatValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() % vv);
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			int vv = v.FloatValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() % vv);
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			int vv = v.IntValue();
			if (vv == 0) return KLFValue();
			return KLFValue(IntValue() % vv);
		}

		return KLFValue();
	}
	*/
	KLFValue operator< (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() < v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 < t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() < v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() < v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() < v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator> (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() > v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 > t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() > v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() > v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() > v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator<= (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() <= v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 <= t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() <= v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() <= v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() <= v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator>= (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() >= v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 >= t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() >= v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() >= v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() >= v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator== (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() == v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 == t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() == v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() == v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() == v.IntValue());
		}

		return KLFValue();
	}

	KLFValue operator!= (const KLFValue &v) {
		if (Type == T_STRING || v.Type == T_STRING) {
			return KLFValue(StringValue() != v.StringValue());
		}
		if (Type == T_TIME || v.Type == T_TIME) {
			ktime_t t1 = TimeValue();
			ktime_t t2 = v.TimeValue();
			return KLFValue(t1 != t2);
		}
		if (Type == T_FLOAT || v.Type == T_FLOAT) {
			return KLFValue(FloatValue() != v.FloatValue());
		}
		if (Type == T_INTEGER || v.Type == T_INTEGER) {
			return KLFValue(IntValue() != v.IntValue());
		}
		if (Type == T_BOOLEAN || v.Type == T_BOOLEAN) {
			return KLFValue(IntValue() != v.IntValue());
		}

		return KLFValue();
	}


	KLFValue operator- () {
		if (Type == T_STRING) {
			return KLFValue(StringValue()/* - v.StringValue()*/);
		}
		if (Type == T_TIME) {
			ktime_t t1 = TimeValue();
			t1.frame2t(-t1.t2frame());
			return KLFValue(t1);
		}
		if (Type == T_FLOAT) {
			return KLFValue(-FloatValue());
		}
		if (Type == T_INTEGER) {
			return KLFValue(-IntValue());
		}
		if (Type == T_BOOLEAN) {
			return KLFValue(-IntValue());
		}

		return KLFValue();
	}

	KLFValue operator! () {
		if (Type == T_STRING) {
			return KLFValue(StringValue()/* - v.StringValue()*/);
		}
		if (Type == T_TIME) {
			ktime_t t1 = TimeValue();
			return KLFValue(t1.t2frame() == 0);
		}
		if (Type == T_FLOAT) {
			return KLFValue(FloatValue() == 0);
		}
		if (Type == T_INTEGER) {
			return KLFValue(IntValue() == 0);
		}
		if (Type == T_BOOLEAN) {
			return KLFValue(!BoolValue());
		}

		return KLFValue();
	}
};