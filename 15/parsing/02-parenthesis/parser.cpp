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
	void Print(void) const;
protected:
	void Print(const Word *wordPtr,YsString indent) const;

public:
	YSRESULT Parse(const YsWString &wstr);
protected:
	YSRESULT Decompose(const YsWString &wstr);
	YSRESULT ClampParenthesis(Word *&currentPtr,wchar_t leftSymbol);
	void DropClosingParenthesis(Word *currentPtr);
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

void Parser::Print(void) const
{
	Print(firstWordPtr,"");
}

void Parser::Print(const Word *wordPtr,YsString indent) const
{
	for(auto ptr=wordPtr; nullptr!=ptr; ptr=ptr->nextWordPtr)
	{
		printf("%s",indent.Txt());
		printf("%ls\n",ptr->str.Txt());
		if(nullptr!=ptr->subWordPtr)
		{
			auto newIndent=indent;
			newIndent.Append("  ");
			Print(ptr->subWordPtr,newIndent);
		}
	}
}

YSRESULT Parser::Parse(const YsWString &wstr)
{
	CleanUp();
	if(YSOK!=Decompose(wstr))
	{
		return YSERR;
	}

	auto top=firstWordPtr;
	if(YSOK!=ClampParenthesis(top,0))
	{
		return YSERR;
	}

	DropClosingParenthesis(firstWordPtr);

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

YSRESULT Parser::ClampParenthesis(Word *&currentPtr,wchar_t leftSymbol)
{
	while(nullptr!=currentPtr)
	{
		if(0==currentPtr->str.Strcmp(L"(") ||
		   0==currentPtr->str.Strcmp(L"[") ||
		   0==currentPtr->str.Strcmp(L"{"))
		{
			auto subGroupPtr=currentPtr->nextWordPtr;
			if(YSOK!=ClampParenthesis(subGroupPtr,currentPtr->str[0]))
			{
				printf("Detected an open %ls\n",currentPtr->str.Txt());
				return YSERR;
			}
			// If successful, subGroupPtr is pointing to the closing symbol for the currentPtr.
			subGroupPtr->prevWordPtr->nextWordPtr=nullptr; // Always exists.
			currentPtr->subWordPtr=currentPtr->nextWordPtr;

			currentPtr->nextWordPtr=subGroupPtr;
			subGroupPtr->prevWordPtr=currentPtr;

			currentPtr=subGroupPtr;
		}
		else if(0==currentPtr->str.Strcmp(L")") ||
			    0==currentPtr->str.Strcmp(L"]") ||
			    0==currentPtr->str.Strcmp(L"}"))
		{
			if((leftSymbol!='(' && currentPtr->str[0]==')') ||
			   (leftSymbol!='[' && currentPtr->str[0]==']') ||
			   (leftSymbol!='{' && currentPtr->str[0]=='}'))
			{
				printf("Miss-matching %ls\n",currentPtr->str.Txt());
				return YSERR;
			}
			return YSOK;
		}
		currentPtr=currentPtr->nextWordPtr;
	}

	if(0!=leftSymbol)
	{
		printf("Open %lc error.\n",leftSymbol);
		return YSERR;
	}

	return YSOK;
}

void Parser::DropClosingParenthesis(Word *currentPtr)
{
	while(nullptr!=currentPtr)
	{
		DropClosingParenthesis(currentPtr->subWordPtr);

		if(nullptr!=currentPtr->nextWordPtr &&
		   ((currentPtr->str[0]=='(' && currentPtr->nextWordPtr->str[0]==')') ||
		    (currentPtr->str[0]=='[' && currentPtr->nextWordPtr->str[0]==']') ||
		    (currentPtr->str[0]=='{' && currentPtr->nextWordPtr->str[0]=='}')))
		{
			currentPtr->str.Append(currentPtr->nextWordPtr->str);
			currentPtr->nextWordPtr=currentPtr->nextWordPtr->nextWordPtr;
			if(nullptr!=currentPtr->nextWordPtr)
			{
				currentPtr->nextWordPtr->prevWordPtr=currentPtr;
			}
		}

		currentPtr=currentPtr->nextWordPtr;
	}
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

