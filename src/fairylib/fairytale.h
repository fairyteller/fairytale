#pragma once

class FairytaleCore;

class Fairytale
{
public:
	Fairytale();
	~Fairytale();
	virtual int interpret(const char* line);
	virtual int interpret_file(const char* filename);
	class Runtime* get_runtime();
private:
	FairytaleCore* m_pCore;
};