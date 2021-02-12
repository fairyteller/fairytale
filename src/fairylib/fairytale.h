#pragma once

class FairytaleCore;

class Fairytale
{
public:
	Fairytale();
	~Fairytale();
	virtual int interpret(const char* filename, const char* line);
	virtual int interpret_file(const char* filename);
	class Runtime* get_runtime();
	void set_io_handler(class IFileSystemAbstraction*);
private:
	FairytaleCore* m_pCore;
};