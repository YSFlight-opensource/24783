#include <stdio.h>
#include <ysclass.h>


class Parser
{
public:
	class Word
	{
	public:
		YsWString str;
		Word *subWordPtr;
		Word *nextWordPtr;
		Word *prevWordPtr;
	};
	YsSegmentedArray <Word,4> wordArray;
	Word *firstWordPtr,*lastWordPtr;

public:
	Parser();
	~Parser();
	void CleanUp(void);
	void Print(void);

	YSRESULT Parse(const YsWString &wstr);
protected:
	YSRESULT Decompose(const YsWString &wstr);
	Word *CreateWord(void);
};

Parser::Parser()
{
	firstWordPtr=nullptr;
	lastWordPtr=nullptr;
}
Parser::~Parser()
{
	CleanUp();
}
void Parser::CleanUp(void)
{
	wordArray.CleanUp();
	firstWordPtr=nullptr;
	lastWordPtr=nullptr;
}

void Parser::Print(void)
{
	for(auto ptr=firstWordPtr; nullptr!=ptr; ptr=ptr->nextWordPtr)
	{
		printf("%ls\n",ptr->str.Txt());
	}
}

YSRESULT Parser::Parse(const YsWString &wstr)
{
	CleanUp();
	if(YSOK!=Decompose(wstr))
	{
		return YSERR;
	}
	return YSOK;
}

YSRESULT Parser::Decompose(const YsWString &wstr)
{
	YsWString currentWord;
	for(YSSIZE_T idx=0; idx<wstr.Strlen(); ++idx)
	{
		if('+'==wstr[idx] ||
		   '-'==wstr[idx] ||
		   '*'==wstr[idx] ||
		   '/'==wstr[idx] ||
		   '%'==wstr[idx] ||
		   '('==wstr[idx] ||
		   ')'==wstr[idx] ||
		   '['==wstr[idx] ||
		   ']'==wstr[idx] ||
		   '{'==wstr[idx] ||
		   '}'==wstr[idx])
		{
			if(0<currentWord.Strlen())
			{
				auto newWord=CreateWord();
				newWord->str=currentWord;
				currentWord=L"";
			}
			auto newWord=CreateWord();
			newWord->str=L"";
			newWord->str.Append(wstr[idx]);
		}
		else
		{
			currentWord.Append(wstr[idx]);
		}
	}

	if(0<currentWord.Strlen())
	{
		auto newWord=CreateWord();
		newWord->str=currentWord;
	}

	return YSOK;
}

Parser::Word *Parser::CreateWord(void)
{
	auto nextIdx=wordArray.GetN();
	wordArray.Increment();

	auto newWordPtr=&wordArray[nextIdx];

	newWordPtr->prevWordPtr=lastWordPtr;
	if(nullptr!=lastWordPtr)
	{
		lastWordPtr->nextWordPtr=newWordPtr;
	}
	else
	{
		firstWordPtr=newWordPtr;
	}
	lastWordPtr=newWordPtr;
	newWordPtr->subWordPtr=nullptr;
	newWordPtr->nextWordPtr=nullptr;
	return &wordArray[nextIdx];
}


////////////////////////////////////////////////////////////



int main(int ac,char *av[])
{
	if(2<=ac)
	{
		YsWString wstr;
		wstr.SetUTF8String(av[1]);
		Parser parser;
		parser.Parse(wstr);
		parser.Print();
	}
	return 0;
}

