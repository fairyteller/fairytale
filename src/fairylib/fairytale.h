#pragma once

class FairytaleCore;

class Fairytale
{
public:
	Fairytale();
	~Fairytale();
	virtual int interpret(const char* line);
	virtual int interpret_file(const char* filename);
private:
	FairytaleCore* m_pCore;
};