#pragma once
#include "parsing.h"
#include "fairy_object.h"
#include "runtime.h"
#include "rules.h"
#include "lexer.h"
#include "abstract_syntax_tree.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <stack>
#include <list>
#include <set>
#include <memory>
#include <assert.h>
#include <sstream>

const int STDIN = 0;
const int STDOUT = 1;
const int STDERR = 2;

struct IFileSystemAbstraction
{
	virtual ~IFileSystemAbstraction() {}
	virtual uint64_t open(const char* filename, const char* mode) = 0;
	virtual void write(uint64_t file, const void* bytes, size_t amount) = 0;
	virtual size_t read(uint64_t file, void* output, size_t buffsize) = 0;
};

class DefaultFileSystemAbstraction : public IFileSystemAbstraction
{
public:
	virtual ~DefaultFileSystemAbstraction() {}
	uint64_t open(const char* filename, const char* mode) override
	{
		static_assert(sizeof(uint64_t) == sizeof(FILE*));
		return reinterpret_cast<uint64_t>(fopen(filename, mode));
	}
	void write(uint64_t file, const void* bytes, size_t amount) override
	{
		FILE* f = getStreamPtr(file);
		fwrite(bytes, 1, amount, f);
		fflush(f);
	}
	size_t read(uint64_t file, void* output, size_t buffsize) override
	{
		return fread(output, 1, buffsize, getStreamPtr(file));
	}
private:
	FILE* getStreamPtr(uint64_t file)
	{
		switch (file)
		{
			case STDIN:
				return stdin;
			case STDOUT:
				return stdout;
			case STDERR:
				return stderr;
			default:
				return reinterpret_cast<FILE*>(file);
		}
	}
};

class FairytaleCore
{
public:
	FairytaleCore() : m_runtime(this), m_ioHandler(new DefaultFileSystemAbstraction()){}
	int interpret(const std::string& filename, const char* line);
	int interpret(const std::string& filename, const std::string& input);
	int interpret_file(const char* filename);
	Runtime* get_runtime()
	{
		return &m_runtime;
	}

	std::unique_ptr<ASTNode> loadAST(const std::string& filename, const std::string& input);
	void set_io_handler(IFileSystemAbstraction* ioAbstraction)
	{
		m_ioHandler.reset(ioAbstraction);
	}
	IFileSystemAbstraction* get_io_handler()
	{
		return m_ioHandler.get();
	}

	std::string resolveFilename(uint64_t fileIndex)
	{
		return "<filename>";
	}
private:
	Lexer m_lexer;
	Runtime m_runtime;
	std::unique_ptr<IFileSystemAbstraction> m_ioHandler;
};



