#include<stdio.h>
#include<ctype.h>
#include<string.h>
#define ILNGMAX 30 //标识符长度最大值
#define STRLNGMAX 100 //字符串长度最大值
#define MAX_DIGIT 9 //int最大位数
#define MAX_LINE 200 //最大行长度
#define MAX_TAB 2000//表项最大值
#define SMAX 10000 //字符串常量表最大值
#define MAX_STACK 10000 //栈最大值
#define MAX_LABEL 1000 //标签数量最大值
#define MAX_CODE 20000 //四元式数量最大值
#define MAX_CODE_TYPE 20 //四元式类型最大长度
enum symbol{
    notsy, plus, minus, times, div, becomes,
    eql, neq, gtr, geq, lss, leq,
    lbrace, rbrace, lparent, rparent, lbrack, rbrack, comma, semicolon,
    intcon, charcon, stringcon,
    ident,
    constsy, intsy, charsy, voidsy, mainsy,
    ifsy, elsesy, dosy, whilesy, forsy,
    scanfsy, printfsy, returnsy,
    end
}; //枚举类型
enum obj{
    constant, variable, function
}; //标识符种类
enum type{
    inttype, chartype, arraytype, voidtype
}; //标识符类型
enum identtype{
	array_type, function_other, other, 
	array_int, array_char, function_int, function_char, function_void,
	constant_int, constant_char, variable_int, variable_char, no_type
};
const char symbolstr[][12] = {
    "notsy", "plus", "minus", "times", "div", "becomes",
    "eql", "neq", "gtr", "geq", "lss", "leq",
    "lbrace", "rbrace", "lparent", "rparent", "lbrack", "rbrack", "comma", "semicolon",
    "intcon", "charcon", "stringcon",
    "ident",
    "constsy", "intsy", "charsy", "voidsy", "mainsy",
    "ifsy", "elsesy", "dosy", "whilesy", "forsy",
    "scanfsy", "printfsy", "returnsy",
    "end"
}; //枚举类型
const char symbolvalue[][8] = {
    "", "+", "-", "*", "/", "=",
    "==", "!=", ">", ">=", "<", "<=",
    "{", "}", "(", ")", "[", "]", ",", ";",
    "", "", "",
    "",
    "const", "int", "char", "void", "main",
    "if", "else", "do", "while", "for",
    "scanf", "printf", "return",
    "end"
};
const char errormessage[][50] = {
    "标识符过长", //0
    "不合法字符", //1
    "单引号中只能有一个字符", //2
    "字符串过长", //3
    "缺少\"", //4
    "缺少'", //5
    "整数过大", //6
    "前导0不合法", //7
    "应为int常量", //8
    "应为int或char", //9
    "应为标识符", //10
    "常量说明中int或char后应为赋值号", //11
    "应为','或';'", //12
    "应为字符常量", //13
    "int后的标识符后应为'['、','、';'或'('", //14
    "应为']'", //15
    "void后应为main或函数名", //16
    "应为')'", //17
    "应为'{'", //18
    "int后的标识符后应为'['、','或';'", //19
    "应为'}'", //20
    "应为'='、'['或'('", //21
    "应为'('", //22
    "因子格式错误", //23
    "应为while", //24
    "应为=", //25
    "应为;", //26
    "应为+或-", //27
    "应为[", //28
    "应为,或)", //29
    "应为(或;", //30
    "应为int常量、+或-", //31
    "标识符重复定义", //32
	"符号表溢出", //33
	"数组信息向量表溢出", //34
	"函数表溢出", //35
	"类型不匹配或未定义", //36
	"数组下标必须为int类型", //37
};
//符号表
struct table{
    char name[ILNGMAX+1];
    int link;
    enum obj obj;
    enum type typ;
    int ref;
    int adr;
}tab[MAX_TAB];
int t;
//数组表
struct arraytable{
    enum type eltyp;
    int tref;
    int high;
}atab[MAX_TAB];
int a;
//函数表
struct functiontable{
    int tref;
    int parnum;
    int psize;
    int vsize;
}ftab[MAX_TAB];
int f;
//字符串常量表
char stab[SMAX];
int sx;

const int nkw = 13; //保留字个数
const char word[13][8] = { //保留字数组
    "char", "const", "do", "else", "for", "if", "int",
    "main", "printf", "return", "scanf", "void", "while",
};
enum symbol wsym[13] = {//保留字对应类型
    charsy, constsy, dosy, elsesy, forsy, ifsy, intsy,
    mainsy, printfsy, returnsy, scanfsy, voidsy, whilesy
};
FILE *fin; //输入文件指针
char ch; //当前所读字符
int cc = -1; //当前所取字符在该行的位置
char line[MAX_LINE]; //当前行
int ll; //当前行长度
int l; //当前所取字符所在行数
enum symbol sym; //识别到的单词类型
int inum; //识别到的数字
char c; //识别到的字符
char id[ILNGMAX+1]; //识别到的标识符
char string[STRLNGMAX+1]; //识别到的字符串
enum symbol lastsy;
char lastid[ILNGMAX+1];
int funcvalid;

//四元式存储区域
struct code
{
	char type[MAX_CODE_TYPE+1];
	char arg1[ILNGMAX+1];
	char arg2[ILNGMAX+1];
	char result[ILNGMAX+1];
}codes[MAX_CODE];
int codeindex = 0;

//临时变量栈及其指针
int tvs_top, lasttvs_top;//函数调用结束后返回的临时变量指针位置
//Label表及栈
int labx;//labx为当前用到的Label表索引
int falses[MAX_LABEL], fs_top;//false的Label栈及其索引
int trues[MAX_LABEL], ts_top;//true的Label栈及其索引

char temp_s[ILNGMAX+1];
char temp_s1[ILNGMAX+1];
char temp_s2[ILNGMAX+1];
char temp_s3[ILNGMAX+1];
enum identtype lasttype;
enum type exptype;
int lasttemp;
int ifpar;

//函数声明
void getch();//获取一个字符
void error(int n);
void getsym();
char* inttoa(int n);
char* tempvar(int n, int flag);//生成临时变量
void gen();//生成四元式
void ifdefine(char id_temp[], enum identtype typ);
int findduplicate(enum obj object);
void entertable(enum obj object, enum type thetype);//登录符号表
void program();//处理总程序
void constdec();//处理常量声明部分
void variabledec();
void returnfctdec();
void noreturnfctdec();
void parameterlist();
void maindec();
void compoundstatement();
void statementlist();
void statement();
void ifstatement();
void dostatement();
void forstatement();
void condition();
void assignment();
void readstatement();
void writestatement();
void returnstatement();
void returnfctstatement();
void noreturnfctstatement();
void valueparalist();
void expression();
void term();
void factor();
void print(int n);
void printtable();
void printcode();


int main()
{
    int i;
    char sin[FILENAME_MAX] = "14061183_test.txt";
    printf("please input source program file name : \n");
    //scanf("%s", sin);
    fin = fopen(sin, "r");
    if(fin == NULL)
    {
        printf("Failed to open!\n");
        return 1;
    }
    printf("Open successfully!\n");

    i = 1;
    getch();
    /*while(!feof(fin))
    {
        getsym();
        if(ch == -1)
            break;
        print(i++);
    }*/
    program();
    printtable();
	printcode();
    fclose(fin);
}

void getch()//获取一个字符
{
    if(cc == ll - 1)
    {
        if(feof(fin))
        {
            ch = -1;
            return;
        }
        ll = 0;
        cc = -1;
        memset(line, 0, MAX_LINE);
        if(fgets(line, MAX_LINE, fin))
        {
            l++;
            ll = strlen(line);
            if(line[ll] == '\n')
                line[ll--] = '\0';
        }
        else
        {
            printf("read error\n");
            return;
        }
    }
    cc++;
    ch = line[cc];
}
void error(int n)
{
    printf("Error%d in line%d.%d 错误信息：%s\n", n, l, cc, errormessage[n]);
}
void getsym()
{
    int k, i, j;
    char a[ILNGMAX+1];
    while(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
        getch();
    if(ch == -1)
    {
        sym = end;
        return;
    }
    if(isalpha(ch) || ch == '_')
    {
        for(k = 0; k < ILNGMAX && (isalpha(ch) || isdigit(ch) || ch == '_'); k++)
        {
            a[k] = ch;
            getch();
        }
        a[k] = '\0';
        strcpy_s(id, ILNGMAX+1, a);
        if(k == ILNGMAX && (isalpha(ch) || isdigit(ch) || ch == '_'))//标识符超长
        {
            error(0);
            while(isalpha(ch) || isdigit(ch) || ch == '_')
            {
                getch();
            }
        }
        i = 0;
        j = nkw - 1;
        while(i <= j)
        {
            k = (i + j) / 2;
            if(strcmp(id, word[k]) <= 0)
                j = k - 1;
            if(strcmp(id, word[k]) >= 0)
                i = k + 1;
        }
        if(i - 1 > j)
            sym = wsym[k];
        else
            sym = ident;
    }
    else if(ch == '0')
    {
        getch();
        sym = intcon;
        if(isdigit(ch))
        {
            error(7);
            while(isdigit(ch))
            {
                getch();
            }
        }
        else
        {
            inum = 0;
        }
    }
    else if(isdigit(ch))
    {
        sym = intcon;
        k = 0;
        inum = 0;
        while(k < MAX_DIGIT && isdigit(ch))
        {
            inum = inum * 10 + (ch - '0');
            k++;
            getch();
        }
        if(k == MAX_DIGIT && isdigit(ch))
        {
            error(6);
            while(isdigit(ch))
            {
                getch();
            }
        }
    }
    else
    {
        switch(ch)
        {
            case '\'':  sym = charcon;
                        getch();
                        if(isalpha(ch) || isdigit(ch) || ch == '_' ||
                           ch == '+' || ch == '-' || ch == '*' || ch == '/')
                        {
                            c = ch;
                            getch();
                            if(ch != '\'')
                            {
                                //若ch不为' 再读五个字符
                                //如果读到了' 报错：单引号中只能有一个字符
                                //如果没读到，报错：缺少单引号
                                getch();
                                for(i = 0; i < 5 && ch != '\''; i++)
                                {
                                    getch();
                                }
                                if(i < 5)
                                {
                                    error(2);
                                    getch();
                                }
                                else
                                {
                                    error(5);//缺少单引号
                                    while(isalpha(ch) || isdigit(ch) || ch == '_' || ch == '+' ||
                                        ch == '-' || ch == '*' || ch == '/' || ch == '\'')//略去其他字符
                                    {
                                        getch();
                                    }
                                }
                            }
                            else
                            {
                                getch();
                            }
                        }
                        else
                        {
                            error(1);
                            while(isalpha(ch) || isdigit(ch) || ch == '_' || ch == '+' ||
                                ch == '-' || ch == '*' || ch == '/' || ch == '\'')//略去其他字符
                            {
                                getch();
                            }
                        }
                break;
            case '\"':  sym = stringcon;
                        getch();
                        k = 0;
                        while(k < STRLNGMAX && ch != '\"' && ch >= 32 && ch <= 126)
                        {
                            string[k++] = ch;
                            getch();
                        }
                        string[k] = '\0';
                        if(k == STRLNGMAX && ch != '\"' && ch >= 32 && ch <= 126) //字符串超长
                        {
                            error(3);
                            getch();
                            while(ch >= 32 && ch <= 126)
                                getch();
                        }
                        else
                        {
                            if(ch != '\"')
                            {
                                error(4);
                                getch();
                                while(ch >= 32 && ch <= 126)
                                    getch();
                            }
                            else//ch为"
                            {
                                getch();
                            }
                        }
                break;
            case '=' :  getch();
                        if(ch == '=')
                        {
                            sym = eql;
                            getch();
                        }
                        else
                        {
                            sym = becomes;
                        }
                break;
            case '+' :  sym = plus;
                        getch();
                break;
            case '-' :  sym = minus;
                        getch();
                break;
            case '*' :  sym = times;
                        getch();
                break;
            case '/' :  sym = div;
                        getch();
                break;
            case '<' :  getch();
                        if(ch == '=')
                        {
                            sym = leq;
                            getch();
                        }
                        else
                        {
                            sym = lss;
                        }
                break;
            case '>' :  getch();
                        if(ch == '=')
                        {
                            sym = geq;
                            getch();
                        }
                        else
                        {
                            sym = gtr;
                        }
                break;
            case '!' :  getch();
                        if(ch == '=')
                        {
                            sym = neq;
                            getch();
                        }
                        else
                        {
                            sym = notsy;
                            error(1);
                        }
                break;
            case '{' :  sym = lbrace;
                        getch();
                break;
            case '}' :  sym = rbrace;
                        getch();
                break;
            case '(' :  sym = lparent;
                        getch();
                break;
            case ')' :  sym = rparent;
                        getch();
                break;
            case '[' :  sym = lbrack;
                        getch();
                break;
            case ']' :  sym = rbrack;
                        getch();
                break;
            case ';' :  sym = semicolon;
                        getch();
                break;
            case ',' :  sym = comma;
                        getch();
                break;
            case -1:
                break;
            default:    sym = notsy;
                        error(1);
        }
    }
    return ;
}
char* inttoa(int n)
{
	sprintf(temp_s, "%d", n);
	return temp_s;
}
char* tempvar(int n, int flag)
{
	if(flag == 1)
	{
		sprintf(temp_s1, "$t%d", n);
		return temp_s1;
	}
	else if(flag == 2)
	{
		sprintf(temp_s2, "$t%d", n);
		return temp_s2;
	}
	else
	{
		sprintf(temp_s3, "$t%d", n);
		return temp_s3;
	}
}
void gen(char type_temp[], char arg1_temp[], char arg2_temp[], char result_temp[])
{
	strcpy_s(codes[codeindex].type, MAX_CODE_TYPE+1, type_temp);
	strcpy_s(codes[codeindex].arg1, ILNGMAX+1, arg1_temp);
	strcpy_s(codes[codeindex].arg2, ILNGMAX+1, arg2_temp);
	strcpy_s(codes[codeindex].result, ILNGMAX+1, result_temp);
	codeindex++;
}
void ifdefine(char id_temp[], enum identtype typ)//只检查是否定义过以及类型是否匹配
{
	int i;
	if(typ == array_type)
	{
		for(i = t - 1; i >= 0; i--)
		{
			if(tab[i].link == 0 && tab[i].obj == function)
			{
				break;
			}
			if(tab[i].typ == arraytype && strcmp(id_temp, tab[i].name) == 0)
			{
				if(atab[tab[i].ref].eltyp == inttype)
					lasttype = array_int;
				else if(atab[tab[i].ref].eltyp == chartype)
					lasttype = array_char;
				return;
			}
		}
		if(i >= 0)
		{
			for(i = 0; i < t; i++)
			{
				if(tab[i].link == 0 && tab[i].obj == function)
				{
					break;
				}
				if(tab[i].typ == arraytype && strcmp(id_temp, tab[i].name) == 0)
				{
					if(atab[tab[i].ref].eltyp == inttype)
						lasttype = array_int;
					else if(atab[tab[i].ref].eltyp == chartype)
						lasttype = array_char;
					return;
				}
			}
		}
	}
	else if(typ == function_other)
	{
		for(i = 0; i <= f; i++)
		{
			if(strcmp(tab[ftab[i].tref].name, id_temp) == 0)
			{
				if(tab[ftab[i].tref].typ == chartype)
				{
					lasttype = function_char;
				}
				else if(tab[ftab[i].tref].typ == inttype)
				{
					lasttype = function_int;
				}
				else
				{
					lasttype = no_type;
				}
				return;
			}
		}
	}
	else if(typ == function_void)
	{
		for(i = 0; i <= f; i++)
		{
			if(strcmp(tab[ftab[i].tref].name, id_temp) == 0)
			{
				if(tab[ftab[i].tref].typ == voidtype)
				{
					lasttype = function_void;
				}
				else
				{
					lasttype = no_type;
				}
				return;
			}
		}
	}
	else if(typ == other)
	{
		for(i = t - 1; i >= 0; i--)
		{
			if(tab[i].link == 0 && tab[i].obj == function)
			{
				break;
			}
			if(strcmp(id_temp, tab[i].name) == 0)
			{
				if(tab[i].obj == constant && tab[i].typ == inttype)
					lasttype = constant_int;
				else if(tab[i].obj == constant && tab[i].typ == chartype)
					lasttype = constant_char;
				else if(tab[i].obj == variable && tab[i].typ == inttype)
					lasttype = variable_int;
				else if(tab[i].obj == variable && tab[i].typ == chartype)
					lasttype = variable_char;
				else
					lasttype = no_type;
				return;
			}
		}
		if(i >= 0)
		{
			for(i = 0; i < t; i++)
			{
				if(tab[i].link == 0 && tab[i].obj == function)
				{
					break;
				}
				if(strcmp(id_temp, tab[i].name) == 0)
				{
					if(tab[i].obj == constant && tab[i].typ == inttype)
						lasttype = constant_int;
					else if(tab[i].obj == constant && tab[i].typ == chartype)
						lasttype = constant_char;
					else if(tab[i].obj == variable && tab[i].typ == inttype)
						lasttype = variable_int;
					else if(tab[i].obj == variable && tab[i].typ == chartype)
						lasttype = variable_char;
					else
						lasttype = no_type;
					return;
				}
			}
		}
	}
	lasttype = no_type;
}
int findduplicate(enum obj object)//查找标识符是否重复定义
//规则如下：
//1. 常量或变量不能与当前函数内的常量或变量同名，但可以与函数名同名，可以与全局常量或变量同名。
//2. 函数可以和当前函数内的常变量同名，可以和其他函数内的常变量同名，不可以和其他函数同名，不可以和全局变量同名。
{
    int i;
    if(object == constant || object == variable)
    {
        //常量或者变量 直接往前找 分两种情况
        //1. 找到link为0，也就是函数名
        //2. 找到符号表开头
        for(i = t - 1; i >= 0; i--)
        {
            if(tab[i].link == 2)
            {
                return 0;
            }
            if(tab[i].link == 0)
            {
                if(strcmp(tab[i].name, lastid) != 0)
                {
                    return 0;
                }
                else
                {
                    if(tab[i].obj == function)
                        return 0;
                    else
                        return 1;
                }
            }
            if(strcmp(tab[i].name, lastid) == 0)
                return 1;
        }
        return 0;
    }
    else if(object == function)
    {
        //函数 函数不能和别的函数同名 不能和全局变量同名
        if(t >= 1)
        {
            if(tab[0].obj == constant || tab[0].obj == variable)
            {
                if(strcmp(tab[0].name, lastid) == 0)//查看第一项是否同名
                    return 1;
                else
                {
                    for(i = 1; i < t; i++)
                    {
                        if(tab[i].link == 0)
                            break;
                        if(strcmp(tab[i].name, lastid) == 0)//查看所有全局变量是否与函数同名
                        {
                            return 1;
                        }
                    }
                }
            }
        }
        //查看所有函数是否与当前函数同名
        if(f >= 1)
        {
            for(i = 0; i < f; i++)
            {
                if(strcmp(tab[ftab[i].tref].name, lastid) == 0)
                    return 1;
            }
        }
        return 0;
    }
	return 0;
}
void entertable(enum obj object, enum type thetype)
{
	if(object == constant)
    {
		if(t >= MAX_TAB)
		{
			error(33);
			return;
		}
		if(findduplicate(constant) == 1)
        {
            error(32);
            return;
        }
        strcpy_s(tab[t].name, ILNGMAX+1, lastid);
        if(t == 0)
            tab[t].link = 0;
        else
            tab[t].link = 1;
        tab[t].obj = constant;
        tab[t].ref = 0;
        if(thetype == inttype)
        {
            tab[t].typ = inttype;
            tab[t].adr = inum;
			gen("CONST INT", lastid, "", inttoa(inum));
        }
        else if(thetype == chartype)
        {
            tab[t].typ = chartype;
            tab[t].adr = c;
			gen("CONST CHAR", lastid, "", inttoa(c-'\0'));
        }
		//TODO:生成常量声明四元式
        t++;
    }
    else if(object == variable)
    {
		if(t >= MAX_TAB)
		{
			error(33);
			return;
		}
		if(findduplicate(variable) == 1)
        {
            error(32);
            return;
        }
        if(thetype == arraytype)
        {
            if(a >= MAX_TAB)
			{
				error(34);
				return;
			}
			//登录数组表
            if(lastsy == intsy)
            {
                atab[a].eltyp = inttype;
				gen("INT[]", inttoa(inum), "", lastid);
            }
            else
            {
                atab[a].eltyp = chartype;
				gen("CHAR[]", inttoa(inum), "", lastid);
            }
            atab[a].tref = t;
            atab[a].high = inum;

            //登录符号表
            strcpy_s(tab[t].name, ILNGMAX+1, lastid);
            if(t == 0)
                tab[t].link = 0;
            else
                tab[t].link = 1;
            tab[t].obj = variable;
            tab[t].typ = arraytype;
            tab[t].ref = a;
            tab[t].adr = 0;//TODO:变量在运行栈中分配存储单元的相对地址
			//TODO：生成数组变量声明四元式
            t++;
            a++;
			
        }
        else
        {
            //登录符号表
            strcpy_s(tab[t].name, ILNGMAX+1, lastid);
            if(t == 0)
                tab[t].link = 0;
            else
                tab[t].link = 1;
            tab[t].obj = variable;
            if(lastsy == intsy)
			{
                tab[t].typ = inttype;
				if(ifpar == 1)
					gen("PAR INT", "", "", lastid);
				else
					gen("INT", "", "", lastid);
			}
			else
			{
                tab[t].typ = chartype;
				if(ifpar == 1)
					gen("PAR CHAR", "", "", lastid);
				else
					gen("CHAR", "", "", lastid);
			}
			tab[t].ref = 0;
            tab[t].adr = 0;//TODO:变量在运行栈中分配存储单元的相对地址
            //TODO：生成变量声明四元式
			t++;
        }
    }
    else//函数
    {
        if(f >= MAX_TAB)
		{
			error(35);
			funcvalid = -1;
			return;
		}
		if(t >= MAX_TAB)
		{
			error(32);
			funcvalid = -1;
			return;
		}
		if(findduplicate(function) == 1)
        {
			error(32);
			funcvalid = -1;
            return;
        }
        ftab[f].tref = t;
        ftab[f].parnum = 0;
        ftab[f].psize = 0;//TODO:参数及该函数在运行栈S中的内务信息区所占存储单元数
        ftab[f].vsize = 0;//TODO:所占存储单元总数

        strcpy_s(tab[t].name, ILNGMAX+1, lastid);
        tab[t].link = 0;
        tab[t].obj = function;
        if(lastsy == intsy)
		{
            tab[t].typ = inttype;
			gen("INT FUNC", "", "", lastid);
		}
		else if(lastsy == charsy)
		{
            tab[t].typ = chartype;
			gen("CHAR FUNC", "", "", lastid);
		}
		else
        {  
			tab[t].typ = voidtype;
			gen("VOID FUNC", "", "", lastid);
		}
        tab[t].ref = f;
        tab[t].adr = 0;//TODO:函数相应目标代码的入口地址
        //TODO：生成函数声明四元式
		t++;
		//codes[codeindex].type[0] = 'f';
		//codes[codeindex].type[1] = 'u';
		//codes[codeindex].type[2] = 'n';
		//codes[codeindex].type[3] = 'c';
		//codes[codeindex].type[4] = '\0';
		//strcpy_s(codes[codeindex].result, lastid);//生成函数标签四元式
		//codeindex++;
    }
}
void program()//处理总程序
{
    getsym();
    if(sym == constsy)
    {
        constdec();
    }
    printf("line%d.%d 全局常量声明部分处理完毕\n", l, cc);
    while(sym == intsy || sym == charsy || sym == voidsy)
    {
        lastsy = sym;
        if(sym == intsy || sym == charsy)
        {
            getsym();
            while(sym != ident)
            {
                error(10);
                while(sym != semicolon && sym != comma && sym != end)
                    getsym();
                if(sym == semicolon)
                {
                    break;
                }
                else if(sym == comma)
                {
                    getsym();
                }
                else if(sym == end)
                    break;
            }
            if(sym == semicolon)
            {
                getsym();
                continue;
            }
            else if(sym == end)
            {
                break;
            }
            strcpy_s(lastid, ILNGMAX+1, id);
            getsym();
            if(sym == comma || sym == semicolon || sym == lbrack)
            {
                //变量声明
                variabledec();
            }
            else if(sym == lparent)
            {
                returnfctdec();
                getsym();
                break;
            }
            else
            {
                //这里的错误处理暂时没有思路，暂时直接结束
                error(14);
                printf("line%d.%d 遇到无法合理跳读的错误，程序结束\n", l, cc);
                return;
            }
        }
        else if(sym == voidsy)
        {
            getsym();
            strcpy_s(lastid, ILNGMAX+1, id);
            if(sym == mainsy)//主函数
            {
                maindec();
                printf("line%d.%d 语法分析完成\n", l, cc);
                return;
            }
            else if(sym == ident)//无返回值函数
            {
                getsym();
                if(sym != lparent)
                {
                    error(22);
                }
                noreturnfctdec();
                getsym();
                break;
            }
            else
            {
                error(16);
                printf("line%d.%d 遇到无法合理跳读的错误，程序结束\n", l, cc);
                return;
            }
        }
    }
    while(sym == intsy || sym == charsy || sym == voidsy)
    {
        lastsy = sym;
        if(sym == intsy || sym == charsy)
        {
            getsym();
            if(sym != ident)
            {
                error(10);
            }
            strcpy_s(lastid, ILNGMAX+1, id);
            getsym();
            if(sym != lparent)
            {
                error(22);
            }
            returnfctdec();
            getsym();
        }
        else
        {
            getsym();
            strcpy_s(lastid, ILNGMAX+1, id);
            if(sym == mainsy)//主函数
            {
                maindec();
                printf("line%d.%d 语法分析完成\n", l, cc);
                return;
            }
            else if(sym == ident)//无返回值函数
            {
                getsym();
                if(sym != lparent)
                {
                    error(22);
                }
                noreturnfctdec();
                getsym();
            }
            else
            {
                error(16);
                printf("line%d.%d 遇到无法合理跳读的错误，程序结束\n", l, cc);
                return;
            }
        }
    }
    if(sym == end)
    {
        printf("line%d.%d 到达文件结尾\n", l, cc);
    }
    else
    {
        printf("line%d.%d 未处理完，发生错误\n", l, cc);
    }
}
void constdec()//处理常量声明部分
{
    do{
        getsym();
        if(sym != intsy && sym != charsy)
        {
            error(9);
            while(sym != semicolon && sym != end)
                getsym();
            getsym();
        }
        else
        {
            if(sym == intsy)
            {
                do{
                    getsym();
                    if(sym != ident)
                    {
                        error(10);
                        while(sym != semicolon && sym != comma && sym != end)
                            getsym();
                    }
                    else
                    {
                        strcpy_s(lastid, ILNGMAX+1, id);
                        getsym();
                        if(sym != becomes)
                        {
                            error(11);
                            while(sym != semicolon && sym != comma && sym != end)
                                getsym();
                        }
                        else
                        {
                            getsym();
                            if(sym != intcon && sym != plus && sym != minus)
                            {
                                error(31);
                                getsym();
                            }
                            else if(sym == plus || sym == minus)
                            {
                                lastsy = sym;
                                getsym();
                                if(sym != intcon)
                                {
                                    error(8);
                                }
                                if(lastsy == minus)
                                    inum = -1 * inum;
                                entertable(constant, inttype);
                                printf("line%d.%d 常量声明语句：const int %s = %d;\n", l, cc, lastid, inum);
                                getsym();
                            }
                            else
                            {
                                entertable(constant, inttype);
                                printf("line%d.%d 常量声明语句：const int %s = %d;\n", l, cc, lastid, inum);
                                getsym();
                            }
                        }
                    }
                    if(sym == semicolon)
                    {
                        break;
                    }
                    else if(sym != comma)
                    {
                        error(12);
                        while(sym != semicolon && sym != end)
                            getsym();
                        break;
                    }
                }while(1);
                getsym();
            }
            else if(sym == charsy)
            {
                do{
                    getsym();
                    if(sym != ident)
                    {
                        error(10);
                        while(sym != semicolon && sym != comma && sym != end)
                            getsym();
                    }
                    else
                    {
                        strcpy_s(lastid, ILNGMAX+1, id);
                        getsym();
                        if(sym != becomes)
                        {
                            error(11);
                            while(sym != semicolon && sym != comma && sym != end)
                                getsym();
                        }
                        else
                        {
                            getsym();
                            if(sym != charcon)
                            {
                                error(13);
                                getsym();
                            }
                            else
                            {
                                entertable(constant, chartype);
                                printf("line%d.%d 常量声明语句：const char %s = '%c';\n", l, cc, lastid, c);
                                getsym();
                            }
                        }
                    }
                    if(sym == semicolon)
                    {
                        break;
                    }
                    else if(sym != comma)
                    {
                        error(12);
                        while(sym != semicolon && sym != end)
                            getsym();
                        break;
                    }
                }while(1);
                getsym();
            }
        }
        if(sym != constsy)
        {
            break;
        }
    }while(1);
}
void variabledec()//处理变量声明部分
{
    if(sym == lbrack)
    {
        getsym();
        if(sym != intcon)
        {
            error(8);
            while(sym != comma && sym != semicolon && sym != end)
                getsym();
        }
        else
        {
            getsym();
            if(sym != rbrack)
            {
                error(15);
            }
            entertable(variable, arraytype);
            printf("line%d.%d 变量声明语句：%s %s[%d];\n", l, cc, symbolvalue[lastsy], lastid, inum);
            getsym();
        }
    }
    else
    {
        if(lastsy == intsy)
			entertable(variable, inttype);
		else
			entertable(variable, chartype);
        printf("line%d.%d 变量声明语句：%s %s;\n", l, cc, symbolvalue[lastsy], lastid);
    }
    while(sym == comma)
    {
        getsym();
        strcpy_s(lastid, ILNGMAX+1, id);
        if(sym == ident)
        {
            getsym();
            if(sym == lbrack)
            {
                getsym();
                if(sym != intcon)
                {
                    error(8);
                    while(sym != comma && sym != semicolon && sym != end)
                        getsym();
                }
                else
                {
                    getsym();
                    if(sym != rbrack)
                    {
                        error(15);
                    }
                    entertable(variable, arraytype);
                    printf("line%d.%d 变量声明语句：%s %s[%d];\n", l, cc, symbolvalue[lastsy], id, inum);
                    getsym();
                }
            }
            else
            {
				if(lastsy == intsy)
					entertable(variable, inttype);
				else
					entertable(variable, chartype);
                printf("line%d.%d 变量声明语句：%s %s;\n", l, cc, symbolvalue[lastsy], id);
            }
        }
        else
        {
            error(10);
            while(sym != comma && sym != semicolon && sym != end)
                getsym();
        }
    }
    if(sym == semicolon)
    {
        getsym();
        return;
    }
    else
    {
        error(16);
        while(sym != comma && sym != semicolon && sym != end)
            getsym();
        getsym();
    }
}
void returnfctdec()//处理有返回值函数定义
{
    printf("line%d.%d 有返回值函数%s定义分析开始\n", l, cc, lastid);
	if(lastsy == intsy)
		entertable(function, inttype);
	else
		entertable(function, chartype);
    parameterlist();
    compoundstatement();
    f++;
	if(funcvalid == -1)
	{
		f--;
		funcvalid = 0;
	}
    if(tab[t-1].link != 0)
        tab[t-1].link = 2;
    printf("line%d.%d 有返回值函数%s定义分析完成\n", l, cc, tab[ftab[f - 1].tref].name);
}
void noreturnfctdec()//处理无返回值函数定义
{
    printf("line%d.%d 无返回值函数定义分析开始\n", l, cc);
	entertable(function, voidtype);
    parameterlist();
    compoundstatement();
    f++;
	if(funcvalid == -1)
	{
		f--;
		funcvalid = 0;
	}
    if(tab[t-1].link != 0)
        tab[t-1].link = 2;
    printf("line%d.%d 无返回值函数定义分析完成\n", l, cc);
}
void parameterlist()//处理函数参数，将形参及其有关信息登录到符号表中
{
    int i = 0;
    do{
        getsym();
        if(i == 0 && sym == rparent)
            break;
        if(sym == charsy || sym == intsy)
        {
            lastsy = sym;
            getsym();
            if(sym == ident)
            {
                ifpar = 1;
				strcpy_s(lastid, ILNGMAX+1, id);
				if(lastsy == intsy)
					entertable(variable, inttype);
				else
					entertable(variable, chartype);
				ifpar = 0;
                printf("line%d.%d 函数参数：%s %s\n", l, cc, symbolvalue[lastsy], id);
                getsym();
            }
            else
            {
                error(10);
                while(sym != comma && sym != rparent && sym != end)
                    getsym();
            }
        }
        else
        {
            error(9);
            while(sym != comma && sym != rparent && sym != end)
                getsym();
        }
        i++;
    }while(sym == comma);
    if(sym == rparent)
    {
        printf("line%d.%d 参数表处理完成，共有%d个参数\n", l, cc, i);
        ftab[f].parnum = i;
    }
    else
    {
        error(17);
    }
}
void maindec()
{
	entertable(function, voidtype);
    getsym();
    if(sym != lparent)
    {
        error(22);
    }
    getsym();
    if(sym != rparent)
    {
        error(17);
    }
    compoundstatement();
    f++;
	if(funcvalid == -1)
	{
		f--;
		funcvalid = 0;
	}
    if(tab[t-1].link != 0)
        tab[t-1].link = 2;
    printf("line%d.%d main函数分析完成\n", l, cc);
}
void compoundstatement()
{
    getsym();
    if(sym != lbrace)
    {
        error(18);
    }
    getsym();
    if(sym == constsy)
    {
        constdec();
    }
    printf("line%d.%d 函数中常量声明部分处理完毕\n", l, cc);
    while(sym == intsy || sym == charsy)
    {
        lastsy = sym;
        getsym();
        while(sym != ident)
        {
            error(10);
            while(sym != semicolon && sym != comma && sym != end)
                getsym();
            if(sym == semicolon)
            {
                break;
            }
            else if(sym == comma)
            {
                getsym();
            }
            else if(sym == end)
                break;
        }
        if(sym == semicolon)
        {
            getsym();
            continue;
        }
        else if(sym == end)
        {
            break;
        }
        strcpy_s(lastid, ILNGMAX+1, id);
        getsym();
        if(sym == comma || sym == semicolon || sym == lbrack)
        {
            variabledec();
        }
        else
        {
            error(19);
            while(sym != semicolon && sym != comma && sym != end)
                getsym();
            getsym();
        }
    }
    printf("line%d.%d 函数内变量声明处理完毕\n", l, cc);
    statementlist();//已经读了第一个符号
    if(sym == rbrace)
    {
        printf("line%d.%d 复合语句处理完毕\n", l, cc);
    }
    else
    {
        error(20);
    }
}
void statementlist()//预读一个
{
    printf("line%d.%d 语句列识别开始\n", l, cc);
    while(sym == ident || sym == ifsy || sym == dosy || sym == forsy || sym == scanfsy
        || sym == printfsy || sym == returnsy || sym == lbrace || sym == semicolon)
    {
        statement();
        if(sym == rbrace)
        {
            break;
        }
    }
    printf("line%d.%d 语句列识别结束\n", l, cc);
}
void statement()//预读一个，多读一个
{
    if(sym == ident)
    {
        strcpy_s(lastid, ILNGMAX+1, id);
        getsym();
        if(sym == becomes || sym == lbrack)
        {
            assignment();
            if(sym != semicolon)
            {
                error(26);
            }
        }
        else if(sym == lparent)
        {
            returnfctstatement();
            if(sym != semicolon)
            {
                error(26);
            }
        }
        else
        {
            error(21);
            while(sym != semicolon && sym != end)
                getsym();
        }
        getsym();
    }
    else if(sym == ifsy)
    {
        ifstatement();
    }
    else if(sym == dosy)
    {
        dostatement();
    }
    else if(sym == forsy)
    {
        forstatement();
    }
    else if(sym == scanfsy)
    {
        readstatement();
        if(sym != semicolon)
        {
            error(26);
        }
        getsym();
    }
    else if(sym == printfsy)
    {
        writestatement();
        if(sym != semicolon)
        {
            error(26);
        }
        getsym();
    }
    else if(sym == returnsy)
    {
        returnstatement();
        if(sym != semicolon)
        {
            error(26);
        }
        getsym();
    }
    else if(sym == lbrace)
    {
        getsym();
        statementlist();
        if(sym == rbrace)
        {
            printf("line%d.%d 语句中的语句列处理完毕\n", l, cc);
        }
        else
        {
            error(20);
        }
        getsym();
    }
    else if(sym == semicolon)
    {
        getsym();
        return;
    }
}
void ifstatement()//预读一个，多读一个
{
    getsym();
    if(sym != lparent)
    {
        error(22);
    }
    condition();
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    statement();
    if(sym == elsesy)
    {
        getsym();
        statement();
    }
    printf("line%d.%d if [else]语句分析完成\n", l, cc);
}
void dostatement()//预读一个，多读一个
{
    getsym();
    statement();
    if(sym != whilesy)
    {
        error(24);
    }
    getsym();
    if(sym != lparent)
    {
        error(22);
    }
    condition();
    if(sym == rparent)
    {
        printf("line%d.%d do while语句分析完成\n", l, cc);
    }
    else
    {
        error(17);
    }
    getsym();
}
void forstatement()//预读一个，多读一个
{
	enum type lefttype;
	int lefttemp;
	char localid[ILNGMAX+1];
	getsym();
    if(sym != lparent)
    {
        error(22);
    }
    getsym();
    if(sym != ident)
    {
        error(10);
    }
	strcpy_s(lastid, ILNGMAX+1, id);
    getsym();
    if(sym != becomes)
    {
        error(25);
    }
	//生成赋值语句四元式
	ifdefine(lastid, other);
	if(lasttype != variable_int && lasttype != variable_char)
	{
		error(36);
		return;
	}
	strcpy_s(localid, ILNGMAX+1, lastid);
	if(lasttype == variable_int)
		lefttype = inttype;
	else
		lefttype = chartype;
    getsym();
    expression();
	if(lasttype == no_type)
	{
		return;
	}
	gen("=", tempvar(lasttemp, 2), "", localid);
	gen("=", localid, "", tempvar(tvs_top, 3));
	lasttemp = tvs_top++;
	exptype = lefttype;
	//赋值语句四元式生成完毕
    if(sym != semicolon)
    {
        error(26);
    }
    condition();
    if(sym != semicolon)
    {
        error(26);
    }
    getsym();
    if(sym != ident)
    {
        error(10);
    }
    getsym();
    if(sym != becomes)
    {
        error(25);
    }
    getsym();
    if(sym != ident)
    {
        error(10);
    }
    getsym();
    if(sym != plus && sym != minus)
    {
        error(27);
    }
    getsym();
    if(sym != intcon)
    {
        error(8);
    }
    getsym();
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    statement();
    printf("line%d.%d for语句分析完成\n", l, cc);
}
void condition()//没有预读，多读一个
{
    getsym();
    expression();
    if(sym == eql || sym == neq || sym == gtr
    || sym == geq || sym == lss || sym == leq)
    {
        getsym();
        expression();
    }
    printf("line%d.%d 条件分析完成\n", l, cc);
}
void assignment()//预读到=或[，多读一个
{
    enum type lefttype;
	int lefttemp, indextemp;
	char localid[ILNGMAX+1];
	if(sym == becomes)
    {
		//判断为变量赋值是否正确
		ifdefine(lastid, other);
		if(lasttype != variable_int && lasttype != variable_char)
		{
			error(36);
			return;
		}
		strcpy_s(localid, ILNGMAX+1, lastid);
		if(lasttype == variable_int)
			lefttype = inttype;
		else
			lefttype = chartype;
		getsym();
        expression();
		if(lasttype == no_type)
		{
			return;
		}
		gen("=", tempvar(lasttemp, 2), "", localid);
		gen("=", localid, "", tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = lefttype;
    }
    else if(sym == lbrack)
    {
		//判断数组调用是否正确
		ifdefine(lastid, array_type);
		if(lasttype == no_type)
		{
			error(36);
			return;
		}
		if(lasttype == array_int)
			lefttype = inttype;
		else
			lefttype = chartype;
		strcpy_s(localid, ILNGMAX+1, lastid);
        getsym();
        expression();
		//这里只需要检查表达式类型是否是int，而不需要判断是否超出数组上界
		if(exptype != inttype)
		{
			error(37);
			return;
		}
        if(sym != rbrack)
        {
            error(15);
        }
		indextemp = lasttemp;
        getsym();
        if(sym != becomes)
        {
            error(25);
        }
        getsym();
        expression();
		if(lasttype == no_type)
		{
			return;
		}
		gen("[]=", localid, tempvar(indextemp, 2), tempvar(lasttemp, 3));
		gen("=[]", localid, tempvar(indextemp, 2), tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = lefttype;
    }
    printf("line%d.%d 赋值语句分析完成\n", l, cc);
}
void readstatement()//预读一个，多读一个
{
    getsym();
    if(sym != lparent)
    {
        error(22);
    }
    do{
        getsym();
        if(sym != ident)
        {
            error(10);
        }
        getsym();
    }while(sym == comma);
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    printf("line%d.%d scanf语句分析完成\n", l, cc);
}
void writestatement()//预读一个，多读一个
{
    getsym();
    if(sym != lparent)
    {
        error(22);
    }
    getsym();
    if(sym == stringcon)
    {
        getsym();
        if(sym == comma)
        {
            getsym();
            expression();
            if(sym != rparent)
            {
                error(17);
            }
            printf("line%d.%d printf语句分析完成\n", l, cc);
        }
        else if(sym == rparent)
        {
            printf("line%d.%d printf语句分析完成\n", l, cc);
        }
        else
        {
            error(29);
        }
    }
    else
    {
        expression();
        if(sym != rparent)
        {
            error(17);
        }
        printf("line%d.%d printf语句分析完成\n", l, cc);
    }
    getsym();
}
void returnstatement()//预读一个，多读一个
{
    getsym();
    if(sym == lparent)
    {
        getsym();
        expression();
        if(sym != rparent)
        {
            error(17);
        }
        getsym();
    }
    else if(sym == semicolon)
    {
    }
    else
    {
        error(30);
        getsym();
    }
    printf("line%d.%d return语句分析完成\n", l, cc);
}
void returnfctstatement()//预读到(
{
    valueparalist();
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    printf("line%d.%d 有返回值函数调用语句分析完成\n", l, cc);
}
void noreturnfctstatement();
void valueparalist()//没有预读，多读一个
{
    do{
        getsym();
        if(sym == rparent)
            return;
        expression();
    }while(sym == comma);
}
void expression()//预读一个，多读一个
{
    enum symbol localsy = notsy;
	int localtemp;
	enum type lastexptype;
	if(sym == plus || sym == minus)
    {
        localsy = sym;
		getsym();
    }
    term();
	if(lasttype == no_type)
	{
		return;
	}
	if(localsy == minus)
	{
		gen("-", "0", tempvar(lasttemp, 2), tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = inttype;
	}
	localtemp = lasttemp;
	lastexptype = exptype;
    while(sym == plus || sym == minus)
    {
        localsy = sym;
		getsym();
        term();
		if(lasttype == no_type)
		{
			return;
		}
		if(localsy == plus)
		{
			gen("+", tempvar(lasttemp, 1), tempvar(localtemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			localtemp = lasttemp;
			exptype = inttype;
			lastexptype = exptype;
		}
		else
		{
			gen("-", tempvar(lasttemp, 1), tempvar(localtemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			localtemp = lasttemp;
			exptype = inttype;
			lastexptype = exptype;
		}
    }
    printf("line%d.%d 表达式分析完成\n", l, cc);
}
void term()//预读一个，多读一个
{
    enum symbol localsy;
	int localtemp;
	enum type lastexptype;
	factor();
	if(lasttype == no_type)
	{
		return;
	}
	localtemp = lasttemp;
	lastexptype = exptype;
    while(sym == times || sym == div)
    {
        localsy = sym;
		getsym();
        factor();
		if(lasttype == no_type)
		{
			return;
		}
		if(localsy == times)
		{
			gen("*", tempvar(lasttemp, 1), tempvar(localtemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			localtemp = lasttemp;
			exptype = inttype;
			lastexptype = exptype;
		}
		else
		{
			gen("/", tempvar(lasttemp, 1), tempvar(localtemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			localtemp = lasttemp;
			exptype = inttype;
			lastexptype = exptype;
		}
    }
    printf("line%d.%d 项分析完成\n", l, cc);
}
void factor()//预读一个，多读一个
{
	enum identtype localtype;
	char localid[ILNGMAX+1];
	if(sym == ident)
    {
		strcpy_s(lastid, ILNGMAX+1, id);
        getsym();
        if(sym == lbrack)
        {
            //判断数组调用是否正确
			ifdefine(lastid, array_type);
			if(lasttype == no_type)
			{
				error(36);
				return;
			}
			localtype = lasttype;
			strcpy_s(localid, ILNGMAX+1, lastid);
			getsym();
            expression();
			//这里只需要检查表达式类型是否是int，而不需要判断是否超出数组上界
			if(exptype != inttype)
			{
				error(37);
				return;
			}
            if(sym == rbrack)
            {
                printf("line%d.%d 因子为<标识符>[<表达式>]的形式\n", l, cc);
            }
            else
            {
                error(15);
            }
			gen("=[]", localid, tempvar(lasttemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			if(localtype == array_int)
				exptype = inttype;
			else
				exptype = chartype;
            getsym();
        }
        else if(sym == lparent)
        {
			//判断有返回值函数调用是否正确
			ifdefine(lastid, function_other);
			if(lasttype == no_type)
			{
				error(36);
				return;
			}
			localtype = lasttype;
			strcpy_s(localid, ILNGMAX+1, lastid);
			returnfctstatement();
            printf("line%d.%d 因子为<有返回值函数调用语句>的形式\n", l, cc);
			gen("call", localid, "", tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			if(localtype == function_int)
				exptype = inttype;
			else
				exptype = chartype;
        }
        else
        {
			//判断常量或变量调用是否正确
			ifdefine(lastid, other);
			if(lasttype == no_type)
			{
				error(36);
				return;
			}
			printf("line%d.%d 因子为<标识符>的形式\n", l, cc);
			gen("=", lastid, "", tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			if(lasttype == constant_int || lasttype == variable_int)
				exptype = inttype;
			else
				exptype = chartype;
        }
    }
    else if(sym == lparent)
    {
        getsym();
        expression();
        if(sym == rparent)
        {
            printf("line%d.%d 因子是(<表达式>)的形式\n", l, cc);
        }
        else
        {
            error(17);
        }
        getsym();
    }
    else if(sym == charcon)
    {
        printf("line%d.%d 因子是一个字符\n", l, cc);
		gen("=", inttoa(c-'\0'), "", tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = chartype;
        getsym();
    }
    else if(sym == plus || sym == minus)
    {
        lastsy = sym;
        getsym();
        if(sym == intcon)
        {
            printf("line%d.%d 因子是一个整数\n", l, cc);
        }
        else
        {
            error(8);
        }
		if(lastsy == minus)
			inum *= -1;
		gen("=", inttoa(inum), "", tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = inttype;
        getsym();
    }
    else if(sym == intcon)
    {
        printf("line%d.%d 因子是一个整数\n", l, cc);
		gen("=", inttoa(inum), "", tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = inttype;
        getsym();
    }
    else
    {
        error(23);
        //TODO:跳到哪里？？？
        getsym();
    }
}

void print(int n)
{
    printf("%d %s ", n, symbolstr[sym]);
    switch(sym)
    {
        case intcon:printf("%d", inum);
            break;
        case charcon:printf("%c", c);
            break;
        case stringcon:printf("\"%s\"", string);
            break;
        case ident: printf("%s", id);
            break;
        default: printf("%s", symbolvalue[sym]);
    }
    putchar('\n');
}
void printtable()
{
    int i;
    printf("symbol table\n");
    for(i = 0; i < t; i++)
    {
        printf("id: %2d   name: %10s link: %4d   obj: %1d   typ: %1d   ref: %2d   adr: %5d\n",
            i, tab[i].name, tab[i].link, tab[i].obj, tab[i].typ, tab[i].ref, tab[i].adr);
    }
    printf("\n\n\nfunction table\n");
    for(i = 0; i < f; i++)
    {
        printf("id: %2d   name: %10s   tref: %3d   parnum: %1d\n",
            i, tab[ftab[i].tref].name, ftab[i].tref, ftab[i].parnum);
    }
    printf("\n\n\narray table\n");
    for(i = 0; i < a; i++)
    {
        printf("id: %2d   name: %10s   tref: %3d   eltyp: %1d   high: %5d\n",
            i, tab[atab[i].tref].name, atab[i].tref, atab[i].eltyp, atab[i].high);
    }
}
void printcode()
{
	int i;
	printf("\n\n\nmidcode\n");
	for(i = 0; i < codeindex; i++)
	{
		printf("index:%4d  (%20s,%10s,%10s,%10s)\n", i, codes[i].type, 
			codes[i].arg1, codes[i].arg2, codes[i].result);
	}
}