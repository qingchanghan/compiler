#include<stdio.h>
#include<ctype.h>
#include<string.h>
#define ILNGMAX 30 //标识符长度最大值
#define STRINGMAX 200 //字符串长度最大值
#define MAX_DIGIT 9 //int最大位数
#define MAX_LINE 200 //最大行长度
#define MAX_TAB 2000//表项最大值
#define SMAX 10000 //字符串常量表最大值
#define MAX_STACK 10000 //栈最大值
#define MAX_LABEL 1000 //标签数量最大值
#define MAX_CODE 20000 //四元式数量最大值
#define MAX_CODE_TYPE 20 //四元式类型最大长度
#define MAX_PAR_NUM 10 //函数参数最大个数
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
	array_type, function_type, other, 
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
	"函数实参与形参数量或类型不符", //38
	"有返回值函数不能返回空", //39
	"无返回值函数只能返回空", //40
};
//符号表
struct table{
    char name[ILNGMAX+1];
    int link;
    enum obj obj;
    enum type typ;
    int ref;
    int adr;
	int value;
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
	int size;
	int lasttvs;
}ftab[MAX_TAB];
int f;
//字符串常量表
char strtable[SMAX][STRINGMAX];
int stringx;

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
char string[STRINGMAX+1]; //识别到的字符串
enum symbol lastsy;//读到的上一个符号类型
char lastid[ILNGMAX+1];//读到的上一个标识符
int funcvalid;//函数是否有效
int cur_adr;//常量和变量当前的相对地址

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
int tvs_top = 1;
//Label表及栈
int labx;//labx为当前用到的Label表索引

char temp_s[ILNGMAX+1];
char temp_s1[ILNGMAX+1];
char temp_s2[ILNGMAX+1];
char temp_s3[ILNGMAX+1];
enum identtype lasttype;
enum type exptype;
int lasttemp;
int ifpar;
int cur_func;//当前函数在ftab表中的位置
int cur_tabx;//当前查询标识符在tab表中的位置

//函数声明
void getch();//获取一个字符
void error(int n);
void getsym();
char* inttoa(int n);
int atoint(char s[]);
char* tempvar(int n, int flag);//生成临时变量
char* tolabel(int n);
char* totable(int n, int flag);
char* tofunc(int n,int flag);
char* tostring(int n);
int intfromtabx(char s[]);
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
int condition(int n);
void assignment();
void readstatement();
void writestatement();
void returnstatement();
void callfctstatement();
int valueparalist();
void expression();
void term();
void factor();
void print(int n);
void printtable();
void printstring();
void printcode();
void printmipscode();


int main()
{
    int i;
	errno_t err;
    char sin[FILENAME_MAX] = "14061183_test.txt";
    printf("please input source program file name : \n");
    //scanf("%s", sin);
    err = fopen_s(&fin, sin, "r");
    if(err != 0)
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
	printstring();
	printmipscode();
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
                        while(k < STRINGMAX && ch != '\"' && ch >= 32 && ch <= 126)
                        {
                            string[k++] = ch;
                            getch();
                        }
                        string[k] = '\0';
                        if(k == STRINGMAX && ch != '\"' && ch >= 32 && ch <= 126) //字符串超长
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
	sprintf_s(temp_s, 10, "%d", n);
	return temp_s;
}
int atoint(char s[])
{
	int i, sum = 0;
	for(i = 0; s[i] != '\0' && isdigit(s[i]); i++)
		sum = sum * 10 + (s[i] - '0');
	return sum;
}
char* tempvar(int n, int flag)
{
	if(flag == 1)
	{
		sprintf_s(temp_s1, 10, "$t%d", n);
		return temp_s1;
	}
	else if(flag == 2)
	{
		sprintf_s(temp_s2, 10, "$t%d", n);
		return temp_s2;
	}
	else
	{
		sprintf_s(temp_s3, 10, "$t%d", n);
		return temp_s3;
	}
}
char* tolabel(int n)
{
	sprintf_s(temp_s3, 10, "label%d", n);
	return temp_s3;
}
char* totable(int n, int flag)
{
	if(flag == 1)
	{
		sprintf_s(temp_s1, 10, "$s%d", n);
		return temp_s1;
	}
	else if(flag == 2)
	{
		sprintf_s(temp_s2, 10, "$s%d", n);
		return temp_s2;
	}
	else
	{
		sprintf_s(temp_s3, 10, "$s%d", n);
		return temp_s3;
	}
}
char* tofunc(int n, int flag)
{
	if(flag == 1)
	{
		sprintf_s(temp_s1, 10, "$f%d", n);
		return temp_s1;
	}
	else if(flag == 2)
	{
		sprintf_s(temp_s2, 10, "$f%d", n);
		return temp_s2;
	}
	else
	{
		sprintf_s(temp_s3, 10, "$f%d", n);
		return temp_s3;
	}
}
char* tostring(int n)
{
	sprintf_s(temp_s3, 10, "string%d", n);
	return temp_s3;
}
int intfromtabx(char s[])
{
	int i, sum = 0;
	if(s[0] != '$')
		return 0;
	for(i = 2; s[i] != '\0' && isdigit(s[i]); i++)
	{
		sum = sum * 10 + (s[i] - '0');
	}
	return sum;
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
				cur_tabx = i;
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
					cur_tabx = i;
					return;
				}
			}
		}
	}
	else if(typ == function_type)
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
				else if(tab[ftab[i].tref].typ == voidtype)
				{
					lasttype = function_void;
				}
				cur_func = i;
				cur_tabx = i;
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
				cur_tabx = i;
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
					cur_tabx = i;
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
            tab[t].value = inum;
			gen("CONST INT", totable(t, 1), "", inttoa(inum));
        }
        else if(thetype == chartype)
        {
            tab[t].typ = chartype;
            tab[t].value = c;
			gen("CONST CHAR", totable(t, 1), "", inttoa(c-'\0'));
        }
		cur_adr += 4;
		tab[t].adr = cur_adr;
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
				gen("INT[]", inttoa(inum), "", totable(t, 3));
            }
            else
            {
                atab[a].eltyp = chartype;
				gen("CHAR[]", inttoa(inum), "", totable(t, 3));
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
            tab[t].value = 0;
			cur_adr += inum * 4;
			tab[t].adr = cur_adr;
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
					gen("PAR INT", "", "", totable(t, 3));
				else
					gen("INT", "", "", totable(t, 3));
			}
			else
			{
                tab[t].typ = chartype;
				if(ifpar == 1)
					gen("PAR CHAR", "", "", totable(t, 3));
				else
					gen("CHAR", "", "", totable(t, 3));
			}
			tab[t].ref = 0;
			tab[t].value = 0;
			cur_adr += 4;
			tab[t].adr = cur_adr;
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
		ftab[f].size = 0;
        ftab[f].tref = t;
        ftab[f].parnum = 0;
		cur_adr = 0;

        strcpy_s(tab[t].name, ILNGMAX+1, lastid);
        tab[t].link = 0;
        tab[t].obj = function;
        if(lastsy == intsy)
		{
            tab[t].typ = inttype;
			gen("INT FUNC", "", "", tofunc(f, 3));
		}
		else if(lastsy == charsy)
		{
            tab[t].typ = chartype;
			gen("CHAR FUNC", "", "", tofunc(f, 3));
		}
		else
        {  
			tab[t].typ = voidtype;
			gen("VOID FUNC", "", "", tofunc(f, 3));
		}
        tab[t].ref = f;
        tab[t].adr = 0;
        tab[t].value = 0;
		t++;
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
	tvs_top = 1;
    parameterlist();
    compoundstatement();
	gen("RETURN VOID", "", "", "");
	ftab[f].size = cur_adr;
	ftab[f].lasttvs = tvs_top - 1;
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
	tvs_top = 1;
    parameterlist();
    compoundstatement();
	gen("RETURN VOID", "", "", "");
	ftab[f].size = cur_adr;
	ftab[f].lasttvs = tvs_top - 1;
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
	tvs_top = 1;
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
	gen("RETURN VOID", "", "", "");
	ftab[f].size = cur_adr;
	ftab[f].lasttvs = tvs_top - 1;
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
            ifdefine(lastid, function_type);
			if(lasttype == no_type)
			{
				error(36);
				return;
			}
			callfctstatement();
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
    int locallabel1, locallabel2;
	getsym();
    if(sym != lparent)
    {
        error(22);
    }
    locallabel1 = condition(-1);
	if(locallabel1 == -1)
		return;
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    statement();
    if(sym == elsesy)
    {
        gen("J", "", "", tolabel(labx));
		locallabel2 = labx++;
		gen("LABEL", "", "", tolabel(locallabel1));
		getsym();
        statement();
		gen("LABEL", "", "", tolabel(locallabel2));
    }
	else
	{
		gen("LABEL", "", "", tolabel(locallabel1));
	}
    printf("line%d.%d if [else]语句分析完成\n", l, cc);
}
void dostatement()//预读一个，多读一个
{
    int locallabel;
	gen("LABEL", "", "", tolabel(labx));
	locallabel = labx++;
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
    if(condition(locallabel) == -1)
		return;
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
	enum symbol localsym;
	int locallabel1, locallabel2, localnum, localtabx, localtabx1;
	getsym();
    if(sym != lparent)
    {
        error(22);
    }
	//第一个赋值语句
    getsym();
    if(sym != ident)
    {
        error(10);
    }
	ifdefine(id, other);
	if(lasttype != variable_int && lasttype != variable_char)
	{
		error(36);
		return;
	}
	localtabx = cur_tabx;
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
	gen("=", tempvar(lasttemp, 1), "", totable(localtabx, 3));
	gen("=", totable(localtabx, 1), "", tempvar(tvs_top, 3));
	lasttemp = tvs_top++;
	gen("LABEL", "", "", tolabel(labx));
	locallabel1 = labx++;
    if(sym != semicolon)
    {
        error(26);
    }
	//条件
    locallabel2 = condition(-1);
	if(locallabel2 == -1)
		return;
    if(sym != semicolon)
    {
        error(26);
    }
	//保存步长信息
    getsym();
    if(sym != ident)
    {
        error(10);
    }
	ifdefine(id, other);
	if(lasttype != variable_int && lasttype != variable_char)
	{
		error(36);
		return;
	}
	localtabx = cur_tabx;
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
	ifdefine(id, other);
	if(lasttype == no_type)
	{
		error(36);
		return;
	}
	localtabx1 = cur_tabx;
    getsym();
    if(sym != plus && sym != minus)
    {
		error(27);
    }
	localsym = sym;
    getsym();
    if(sym != intcon)
    {
        error(8);
    }
	localnum = inum;
    getsym();
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    statement();
	if(localsym == plus)
	{
		gen("+", totable(localtabx1, 1), inttoa(localnum), totable(localtabx, 3));
	}
	else
	{
		gen("-", totable(localtabx1, 1), inttoa(localnum), totable(localtabx, 3));
	}
	gen("J", "", "", tolabel(locallabel1));
	gen("LABEL", "", "", tolabel(locallabel2));
    printf("line%d.%d for语句分析完成\n", l, cc);
}
int condition(int n)//没有预读，多读一个
{
	enum symbol localsym;
	int lefttemp;
	getsym();
    expression();
	if(lasttype == no_type)
	{
		return -1;
	}
	lefttemp = lasttemp;
    if(sym == eql || sym == neq || sym == gtr
    || sym == geq || sym == lss || sym == leq)
    {
        localsym = sym;
		getsym();
        expression();
		if(n == -1)//label未建立，条件为反
		{
			switch(localsym)
			{
			case eql:gen("!=", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(labx++));
				break;
			case neq:gen("==", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(labx++));
				break;
			case gtr:gen("<=", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(labx++));
				break;
			case geq:gen("<", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(labx++));
				break;
			case lss:gen(">=", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(labx++));
				break;
			case leq:gen(">", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(labx++));
				break;
			}
		}
		else//label已建立，条件为正
		{
			switch(localsym)
			{
			case eql:gen("==", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(n));
				break;
			case neq:gen("!=", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(n));
				break;
			case gtr:gen(">", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(n));
				break;
			case geq:gen(">=", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(n));
				break;
			case lss:gen("<", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(n));
				break;
			case leq:gen("<=", tempvar(lefttemp, 1), tempvar(lasttemp, 2), tolabel(n));
				break;
			}
		}
    }
	else
	{
		if(n == -1)
		{
			gen("==", tempvar(lefttemp, 1), "0", tolabel(labx++));
		}
		else
		{
			gen("!=", tempvar(lefttemp, 1), "0", tolabel(n));
		}
	}
    printf("line%d.%d 条件分析完成\n", l, cc);
	return labx - 1;
}
void assignment()//预读到=或[，多读一个
{
    enum type lefttype;
	int indextemp;
	int localtabx;
	if(sym == becomes)
    {
		//判断为变量赋值是否正确
		ifdefine(lastid, other);
		if(lasttype != variable_int && lasttype != variable_char)
		{
			error(36);
			return;
		}
		localtabx = cur_tabx;
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
		gen("=", tempvar(lasttemp, 2), "", totable(localtabx, 3));
		gen("=", totable(localtabx, 1), "", tempvar(tvs_top, 3));
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
		localtabx = cur_tabx;
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
		gen("[]=", totable(localtabx, 1), tempvar(indextemp, 2), tempvar(lasttemp, 3));
		gen("=[]", totable(localtabx, 1), tempvar(indextemp, 2), tempvar(tvs_top, 3));
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
		ifdefine(id, other);
		if(lasttype != variable_int && lasttype != variable_char)
		{
			error(36);
			return;
		}
		if(lasttype == variable_int)
			gen("READ INT", "", "", totable(cur_tabx, 3));
		else
			gen("READ CHAR", "", "", totable(cur_tabx, 3));
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
    int i;
	getsym();
    if(sym != lparent)
    {
        error(22);
    }
    getsym();
    if(sym == stringcon)
    {
        for(i = 0; i < stringx && strcmp(string, strtable[i]) != 0; i++);
		if(i == stringx)
			strcpy_s(strtable[stringx++], STRINGMAX, string);
		gen("WRITE STRING", "", "", tostring(i));
		getsym();
        if(sym == comma)
        {
            getsym();
            expression();
			if(lasttype == no_type)
			{
				return;
			}
			if(exptype == inttype)
				gen("WRITE INT", "", "", tempvar(lasttemp, 3));
			else
				gen("WRITE CHAR", "", "", tempvar(lasttemp, 3));
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
		if(lasttype == no_type)
		{
			return;
		}
		if(exptype == inttype)
			gen("WRITE INT", "", "", tempvar(lasttemp, 3));
		else
			gen("WRITE CHAR", "", "", tempvar(lasttemp, 3));
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
        if(tab[ftab[f].tref].typ == voidtype)
		{
			error(40);
			return;
		}
		getsym();
        expression();
        if(sym != rparent)
        {
            error(17);
        }
		if(tab[ftab[f].tref].typ == inttype)
			gen("RETURN INT", "", "", tempvar(lasttemp, 3));
		else
			gen("RETURN CHAR", "", "", tempvar(lasttemp, 3));
        getsym();
    }
    else if(sym == semicolon)
    {
		if(tab[ftab[f].tref].typ != voidtype)
		{
			error(39);
			return;
		}
		gen("RETURN VOID", "", "", "");
    }
    else
    {
        error(30);
        getsym();
    }
    printf("line%d.%d return语句分析完成\n", l, cc);
}
void callfctstatement()//预读到(
{
	enum identtype localtype;
	int parnum;
	localtype = lasttype;
	parnum = valueparalist();
	if(parnum == -1)
	{
		error(38);
		return;
	}
    if(sym != rparent)
    {
        error(17);
    }
	if(localtype == function_int)
		gen("CALL INT FUNC", inttoa(parnum), "", tofunc(cur_func, 3));
	else if(localtype == function_char)
		gen("CALL CHAR FUNC", inttoa(parnum), "", tofunc(cur_func, 3));
	else
		gen("CALL VOID FUNC", inttoa(parnum), "", tofunc(cur_func, 3));
    getsym();
    printf("line%d.%d 有返回值函数调用语句分析完成\n", l, cc);
}
int valueparalist()//没有预读，多读一个
{
    int cur_tref = ftab[cur_func].tref;
	int parnum = ftab[cur_func].parnum;
	int i = 0, listindex = 0;
	enum type typelist[MAX_PAR_NUM];
	int lasttemplist[MAX_PAR_NUM];
	do{
        getsym();
        if(sym == rparent)
            break;
        expression();
		//由于int和char可以相互转化，所以类型无需检查
		typelist[listindex] = exptype;
		lasttemplist[listindex] = lasttemp;
		listindex++;
    }while(sym == comma);
	if(listindex != parnum)
	{
		return -1;
	}
	else
	{
		for(i = 0; i < listindex; i++)
		{
			if(typelist[i] == inttype)
				gen("REAL PAR INT", "", "", tempvar(lasttemplist[i], 3));
			else
				gen("REAL PAR CHAR", "", "", tempvar(lasttemplist[i], 3));
		}
		return listindex;
	}
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
			gen("+", tempvar(localtemp, 1), tempvar(lasttemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			localtemp = lasttemp;
			exptype = inttype;
			lastexptype = exptype;
		}
		else
		{
			gen("-", tempvar(localtemp, 1), tempvar(lasttemp, 2), tempvar(tvs_top, 3));
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
			gen("*", tempvar(localtemp, 1), tempvar(lasttemp, 2), tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			localtemp = lasttemp;
			exptype = inttype;
			lastexptype = exptype;
		}
		else
		{
			gen("/", tempvar(localtemp, 1), tempvar(lasttemp, 2), tempvar(tvs_top, 3));
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
	int localtabx;
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
			localtabx = cur_tabx;
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
			gen("=[]", totable(localtabx, 1), tempvar(lasttemp, 2), tempvar(tvs_top, 3));
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
			ifdefine(lastid, function_type);
			if(lasttype != function_char && lasttype != function_int)
			{
				error(36);
				return;
			}
			localtype = lasttype;
			callfctstatement();
            printf("line%d.%d 因子为<有返回值函数调用语句>的形式\n", l, cc);
			gen("=CALL", tofunc(cur_func, 1), "", tempvar(tvs_top, 3));
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
			gen("=", totable(cur_tabx, 1), "", tempvar(tvs_top, 3));
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
        printf("id: %2d   name: %10s link: %4d   obj: %1d   typ: %1d   ref: %2d   adr: %5d   value: %5d\n",
            i, tab[i].name, tab[i].link, tab[i].obj, tab[i].typ, tab[i].ref, tab[i].adr, tab[i].value);
    }
    printf("\n\n\nfunction table\n");
    for(i = 0; i < f; i++)
    {
        printf("id: %2d   name: %10s   tref: %3d   parnum: %1d   size: %5d\n",
            i, tab[ftab[i].tref].name, ftab[i].tref, ftab[i].parnum, ftab[i].size);
    }
    printf("\n\n\narray table\n");
    for(i = 0; i < a; i++)
    {
        printf("id: %2d   name: %10s   tref: %3d   eltyp: %1d   high: %5d\n",
            i, tab[atab[i].tref].name, atab[i].tref, atab[i].eltyp, atab[i].high);
    }
}
void printstring()
{
	int i;
	printf("\n\n\n");
	for(i = 0; i < stringx; i++)
	{
		printf("%s\n", strtable[i]);
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
void printmipscode()
{
	int i, index, index1, index2, parno = 0, flag = 0, adr, localf;
	FILE *fout;
	errno_t err;
	char ins[10];
	err = fopen_s(&fout, "mips.txt", "w+");
	if(err != 0)
	{
		printf("open failed!\n");
		return;
	}
	fprintf(fout, ".data\n");
	fprintf(fout, "temp: .word 0x7fff0000\n");
	for(i = 0; i < stringx; i++)
	{
		fprintf(fout, "string%d: .asciiz \"%s\"\n", i, strtable[i]);
	}
	fprintf(fout, ".text\n");
	fprintf(fout, "lw $t8, temp\n");//$t8存储临时变量基地址
	fprintf(fout, "move $t9, $sp\n");//$t9存储全局变量地址
	fprintf(fout, "subi $fp, $sp, 12\n\n");

	for(i = 0; i < codeindex; i++)
	{
		if(strcmp(codes[i].type, "CONST INT") == 0 || strcmp(codes[i].type, "CONST CHAR") == 0)
		{
			fprintf(fout, "# CONST INT || CONST CHAR\n");
			index = tab[intfromtabx(codes[i].arg1)].adr;
			fprintf(fout, "li $t0, %s\n", codes[i].result);
			fprintf(fout, "sw $t0, -%d($sp)\n", 8 + index);
			fprintf(fout, "subi $fp, $sp, %d\n", 12 + index);
		}
		else if(strcmp(codes[i].type, "INT") == 0 || strcmp(codes[i].type, "INT[]") == 0
			|| strcmp(codes[i].type, "CHAR") == 0 || strcmp(codes[i].type, "CHAR[]") == 0)
		{
			fprintf(fout, "# INT || CHAR\n");
			index = tab[intfromtabx(codes[i].result)].adr;
			fprintf(fout, "subi $fp, $sp, %d\n", 12 + index);
		}
		else if(strcmp(codes[i].type, "VOID FUNC") == 0 || strcmp(codes[i].type, "INT FUNC") == 0
			|| strcmp(codes[i].type, "CHAR FUNC") == 0)
		{
			if(flag == 0)
			{
				flag = 1;
				fprintf(fout, "jal main\n\n");
				fprintf(fout, "#program end\n");
				fprintf(fout, "li $v0, 10\n");
				fprintf(fout, "syscall\n\n");
			}
			fprintf(fout, "# FUNC\n");
			index = intfromtabx(codes[i].result);
			localf = index;
			fprintf(fout, "%s:\n", tab[ftab[index].tref].name);
			fprintf(fout, "sw $ra, 0($fp)\n");
			fprintf(fout, "sw $sp, -4($fp)\n");
			fprintf(fout, "move $sp, $fp\n");
			fprintf(fout, "subi $fp, $fp, 12\n");
			if(localf == f - 1)
			{
				fprintf(fout, "subi $s7, $t8, %d\n", ftab[localf].lasttvs * 4 + 4);
			}
			else
			{
				fprintf(fout, "sw $t8, 0($s7)\n");
				fprintf(fout, "move $t8, $s7\n");
				fprintf(fout, "subi $s7, $t8, %d\n", ftab[localf].lasttvs * 4 + 4);
			}
		}
		else if(strcmp(codes[i].type, "PAR INT") == 0 || strcmp(codes[i].type, "PAR CHAR") == 0)
		{
			fprintf(fout, "# PAR INT || PAR CHAR\n");
			index = tab[intfromtabx(codes[i].result)].adr;
			fprintf(fout, "subi $fp, $sp, %d\n", 12 + index);
		}
		else if(strcmp(codes[i].type, "=") == 0)
		{
			fprintf(fout, "# = \n");
			index = intfromtabx(codes[i].arg1);
			index1 = intfromtabx(codes[i].result);
			if(codes[i].arg1[0] != '$')
			{
				fprintf(fout, "li $t0, %s\n", codes[i].arg1);
			}
			else if(codes[i].arg1[1] == 's')
			{
				if(index < ftab[0].tref)
				{
					fprintf(fout, "lw $t0, -%d($t9)\n", 8 + tab[index].adr);
				}
				else
				{
					fprintf(fout, "lw $t0, -%d($sp)\n", 8 + tab[index].adr);
				}
			}
			else if(codes[i].arg1[1] == 't')
			{
				fprintf(fout, "lw $t0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
			}
			if(codes[i].result[1] == 's')
			{
				if(index1 < ftab[0].tref)
				{
					fprintf(fout, "sw $t0, -%d($t9)\n", 8 + tab[index1].adr);
				}
				else
				{
					fprintf(fout, "sw $t0, -%d($sp)\n", 8 + tab[index1].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "sw $t0, %d($t8)\n", index1 * 4 > 0 ? -4 * index1 : 0);
			}
		}
		else if(strcmp(codes[i].type, "[]=") == 0)//a[n] = t
		{
			fprintf(fout, "# []=\n");
			index = intfromtabx(codes[i].arg1);
			adr = tab[index].adr - (atab[tab[index].ref].high - 1) * 4;
			index1 = intfromtabx(codes[i].arg2);
			index2 = intfromtabx(codes[i].result);
			//读取t
			if(codes[i].result[1] == 's')
			{
				if(index2 < ftab[0].tref)
				{
					fprintf(fout, "lw $t0, -%d($t9)\n", 8 + tab[index2].adr);
				}
				else
				{
					fprintf(fout, "lw $t0, -%d($sp)\n", 8 + tab[index2].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "lw $t0, %d($t8)\n", index2 * 4 > 0 ? -4 * index2 : 0);
			}
			//读取n
			if(codes[i].arg2[1] == 's')
			{
				if(index1 < ftab[0].tref)
				{
					fprintf(fout, "lw $t1, -%d($t9)\n", 8 + tab[index1].adr);
				}
				else
				{
					fprintf(fout, "lw $t1, -%d($sp)\n", 8 + tab[index1].adr);
				}
			}
			else if(codes[i].arg2[1] == 't')
			{
				fprintf(fout, "lw $t1, %d($t8)\n", index1 * 4 > 0 ? -4 * index1 : 0);
			}
			fprintf(fout, "sll $t1,$t1,2\n");//n * 4
			//$t2存数组a的地址
			if(index < ftab[0].tref)
			{
				fprintf(fout, "subi $t2, $t9, %d\n", 8 + adr);
			}
			else
			{
				fprintf(fout, "subi $t2, $sp, %d\n", 8 + adr);
			}
			fprintf(fout, "sub $t2, $t2, $t1\n");//此时$t2存有a[n]地址
			fprintf(fout, "sw $t0, 0($t2)\n");
		}
		else if(strcmp(codes[i].type, "=[]") == 0)//t = a[n]
		{
			fprintf(fout, "# =[]\n");
			index = intfromtabx(codes[i].arg1);
			adr = tab[index].adr - (atab[tab[index].ref].high - 1) * 4;
			index1 = intfromtabx(codes[i].arg2);
			index2 = intfromtabx(codes[i].result);
			//读取n
			if(codes[i].arg2[1] == 's')
			{
				if(index1 < ftab[0].tref)
				{
					fprintf(fout, "lw $t1, -%d($t9)\n", 8 + tab[index1].adr);
				}
				else
				{
					fprintf(fout, "lw $t1, -%d($sp)\n", 8 + tab[index1].adr);
				}
			}
			else if(codes[i].arg2[1] == 't')
			{
				fprintf(fout, "lw $t1, %d($t8)\n", index1 * 4 > 0 ? -4 * index1 : 0);
			}
			fprintf(fout, "sll $t1,$t1,2\n");//n * 4
			//$t2存数组a的地址
			if(index < ftab[0].tref)
			{
				fprintf(fout, "subi $t2, $t9, %d\n", 8 + adr);
			}
			else
			{
				fprintf(fout, "subi $t2, $sp, %d\n", 8 + adr);
			}
			fprintf(fout, "sub $t2, $t2, $t1\n");//此时$t2存有a[n]地址
			fprintf(fout, "lw $t0, 0($t2)\n");
			//给t赋值
			if(codes[i].result[1] == 's')
			{
				if(index2 < ftab[0].tref)
				{
					fprintf(fout, "sw $t0, -%d($t9)\n", 8 + tab[index2].adr);
				}
				else
				{
					fprintf(fout, "sw $t0, -%d($sp)\n", 8 + tab[index2].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "sw $t0, %d($t8)\n", index2 * 4 > 0 ? -4 * index2 : 0);
			}
		}
		else if(strcmp(codes[i].type, "+") == 0 || strcmp(codes[i].type, "-") == 0
			|| strcmp(codes[i].type, "*") == 0 || strcmp(codes[i].type, "/") == 0)
		{
			fprintf(fout, "# + - * /\n");
			index = intfromtabx(codes[i].arg1);
			index1 = intfromtabx(codes[i].arg2);
			index2 = intfromtabx(codes[i].result);
			if(codes[i].arg1[1] == 's')
			{
				if(index < ftab[0].tref)
				{
					fprintf(fout, "lw $t0, -%d($t9)\n", 8 + tab[index].adr);
				}
				else
				{
					fprintf(fout, "lw $t0, -%d($sp)\n", 8 + tab[index].adr);
				}
			}
			else if(codes[i].arg1[1] == 't')
			{
				fprintf(fout, "lw $t0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
			}
			if(codes[i].arg2[0] == '$')
			{
				if(codes[i].arg2[1] == 's')
				{
					if(index1 < ftab[0].tref)
					{
						fprintf(fout, "lw $t1, -%d($t9)\n", 8 + tab[index1].adr);
					}
					else
					{
						fprintf(fout, "lw $t1, -%d($sp)\n", 8 + tab[index1].adr);
					}

				}
				else if(codes[i].arg2[1] == 't')
				{
					fprintf(fout, "lw $t1, %d($t8)\n", index1 * 4 > 0 ? -4 * index1 : 0);
				}
			}
			if(strcmp(codes[i].type, "+") == 0)
			{
				if(codes[i].arg2[0] == '$')
					fprintf(fout, "add $t2, $t0, $t1\n");
				else
					fprintf(fout, "addi $t2, $t0, %s\n", codes[i].arg2);
			}
			else if(strcmp(codes[i].type, "-") == 0)
			{
				if(codes[i].arg2[0] == '$' && strcmp(codes[i].arg1, "0") != 0)
					fprintf(fout, "sub $t2, $t0, $t1\n");
				else if(strcmp(codes[i].arg1, "0") != 0)
					fprintf(fout, "subi $t2, $t0, %s\n", codes[i].arg2);
				else if(codes[i].arg2[0] == '$' && strcmp(codes[i].arg1, "0") == 0)
					fprintf(fout, "sub $t2, $0, $t1\n");
				else
					fprintf(fout, "subi $t2, $0, %s\n", codes[i].arg2);
			}
			else if(strcmp(codes[i].type, "*") == 0)
			{
				fprintf(fout, "mul $t2, $t0, $t1\n");
			}
			else if(strcmp(codes[i].type, "/") == 0)
			{
				fprintf(fout, "div $t2, $t0, $t1\n");
			}
			if(codes[i].result[1] == 's')
			{
				if(index2 < ftab[0].tref)
				{
					fprintf(fout, "sw $t2, -%d($t9)\n", 8 + tab[index2].adr);
				}
				else
				{
					fprintf(fout, "sw $t2, -%d($sp)\n", 8 + tab[index2].adr);
				}

			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "sw $t2, %d($t8)\n", index2 * 4 > 0 ? -4 * index2 : 0);
			}
		}
		else if(strcmp(codes[i].type, "<") == 0 || strcmp(codes[i].type, "<=") == 0
			|| strcmp(codes[i].type, ">") == 0 || strcmp(codes[i].type, ">=") == 0
			|| strcmp(codes[i].type, "==") == 0 || strcmp(codes[i].type, "!=") == 0)
		{
			fprintf(fout, "# compare\n");
			index = intfromtabx(codes[i].arg1);
			index1 = intfromtabx(codes[i].arg2);
			if(codes[i].arg1[0] == '$')
			{
				if(codes[i].arg1[1] == 's')
				{
					if(index < ftab[0].tref)
					{
						fprintf(fout, "lw $t0, -%d($t9)\n", 8 + tab[index].adr);
					}
					else
					{
						fprintf(fout, "lw $t0, -%d($sp)\n", 8 + tab[index].adr);
					}
				}
				else if(codes[i].arg1[1] == 't')
				{
					fprintf(fout, "lw $t0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
				}
			}
			if(codes[i].arg2[0] == '$')
			{
				if(codes[i].arg2[1] == 's')
				{
					if(index1 < ftab[0].tref)
					{
						fprintf(fout, "lw $t1, -%d($t9)\n", 8 + tab[index1].adr);
					}
					else
					{
						fprintf(fout, "lw $t1, -%d($sp)\n", 8 + tab[index1].adr);
					}
				}
				else if(codes[i].arg2[1] == 't')
				{
					fprintf(fout, "lw $t1, %d($t8)\n", index1 * 4 > 0 ? -4 * index1 : 0);
				}
			}
			if(strcmp(codes[i].type, "<") == 0)
			{
				strcpy_s(ins, 10, "blt");
			}
			else if(strcmp(codes[i].type, "<=") == 0)
			{
				strcpy_s(ins, 10, "ble");
			}
			else if(strcmp(codes[i].type, ">") == 0)
			{
				strcpy_s(ins, 10, "bgt");
			}
			else if(strcmp(codes[i].type, ">=") == 0)
			{
				strcpy_s(ins, 10, "bge");
			}
			else if(strcmp(codes[i].type, "==") == 0)
			{
				strcpy_s(ins, 10, "beq");
			}
			else if(strcmp(codes[i].type, "!=") == 0)
			{
				strcpy_s(ins, 10, "bne");
			}
			if(codes[i].arg1[0] == '$' && codes[i].arg2[0] == '$')
				fprintf(fout, "%s $t0, $t1, %s\n", ins, codes[i].result);
			else if(codes[i].arg2[0] == '$')
				fprintf(fout, "%s %s, $t1, %s\n", ins, codes[i].arg1, codes[i].result);
			else if(codes[i].arg1[0] == '$')
				fprintf(fout, "%s $t0, %s, %s\n", ins, codes[i].arg2, codes[i].result);
			else
				fprintf(fout, "%s %s, %s, %s\n", ins, codes[i].arg1, codes[i].arg2, codes[i].result);
		}
		else if(strcmp(codes[i].type, "J") == 0)
		{
			fprintf(fout, "# j\n");
			fprintf(fout, "j %s\n", codes[i].result);
		}
		else if(strcmp(codes[i].type, "LABEL") == 0)
		{
			fprintf(fout, "# label\n");
			fprintf(fout, "%s:\n", codes[i].result);
		}
		else if(strcmp(codes[i].type, "REAL PAR INT") == 0
			|| strcmp(codes[i].type, "REAL PAR CHAR") == 0)
		{
			fprintf(fout, "# real par\n");
			index = intfromtabx(codes[i].result);
			if(codes[i].result[1] == 's')
			{
				if(index < ftab[0].tref)
				{
					fprintf(fout, "lw $t0, -%d($t9)\n", 8 + tab[index].adr);
				}
				else
				{
					fprintf(fout, "lw $t0, -%d($sp)\n", 8 + tab[index].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "lw $t0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
			}
			fprintf(fout, "sw $t0, -%d($fp)\n", 12 + parno * 4);
			parno++;
		}
		else if(strcmp(codes[i].type, "CALL VOID FUNC") == 0
			|| strcmp(codes[i].type, "CALL INT FUNC") == 0
			|| strcmp(codes[i].type, "CALL CHAR FUNC") == 0)
		{
			fprintf(fout, "# call function\n");
			parno = 0;
			index = intfromtabx(codes[i].result);
			fprintf(fout, "jal %s\n", tab[ftab[index].tref].name);
		}
		else if(strcmp(codes[i].type, "=CALL") == 0)
		{
			fprintf(fout, "# =call\n");
			index = intfromtabx(codes[i].result);
			if(codes[i].result[1] == 's')
			{
				if(index < ftab[0].tref)
				{
					fprintf(fout, "sw $v0, -%d($t9)\n", 8 + tab[index].adr);
				}
				else
				{
					fprintf(fout, "sw $v0, -%d($sp)\n", 8 + tab[index].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "sw $v0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
			}
		}
		else if(strcmp(codes[i].type, "RETURN INT") == 0
			|| strcmp(codes[i].type, "RETURN CHAR") == 0
			|| strcmp(codes[i].type, "RETURN VOID") == 0)
		{
			fprintf(fout, "\n");
			fprintf(fout, "# RETURN\n");
			if(strcmp(codes[i].type, "RETURN VOID") != 0)
			{
				index = intfromtabx(codes[i].result);
				if(codes[i].result[1] == 's')
				{
					if(index < ftab[0].tref)
					{
						fprintf(fout, "lw $t0, -%d($t9)\n", 8 + tab[index].adr);
					}
					else
					{
						fprintf(fout, "lw $t0, -%d($sp)\n", 8 + tab[index].adr);
					}
				}
				else if(codes[i].result[1] == 't')
				{
					fprintf(fout, "lw $t0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
				}
				fprintf(fout, "sw $t0, -8($sp)\n");
			}
			if(localf != f - 1)
			{
				fprintf(fout, "lw $t8, 0($t8)\n");
			}
			fprintf(fout, "move $fp, $sp\n");
			fprintf(fout, "lw $v0, -8($sp)\n");
			fprintf(fout, "lw $ra, 0($sp)\n");
			fprintf(fout, "lw $sp, -4($sp)\n");
			fprintf(fout, "jr $ra\n");
		}
		else if(strcmp(codes[i].type, "WRITE INT") == 0
			|| strcmp(codes[i].type, "WRITE CHAR") == 0)
		{
			fprintf(fout, "# WRITE\n");
			index = intfromtabx(codes[i].result);
			if(codes[i].result[1] == 's')
			{
				if(index < ftab[0].tref)
				{
					fprintf(fout, "lw $a0, -%d($t9)\n", 8 + tab[index].adr);
				}
				else
				{
					fprintf(fout, "lw $a0, -%d($sp)\n", 8 + tab[index].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "lw $a0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
			}
			if(strcmp(codes[i].type, "WRITE INT") == 0)
				fprintf(fout, "li $v0, 1\n");
			else
				fprintf(fout, "li $v0, 11\n");
			fprintf(fout, "syscall\n");
		}
		else if(strcmp(codes[i].type, "READ INT") == 0
			|| strcmp(codes[i].type, "READ CHAR") == 0)
		{
			fprintf(fout, "# READ\n");
			if(strcmp(codes[i].type, "READ INT") == 0)
			{
				fprintf(fout, "li $v0, 5\n");
			}
			else
			{
				fprintf(fout, "li $v0, 12\n");
			}
			fprintf(fout, "syscall\n");
			index = intfromtabx(codes[i].result);
			if(codes[i].result[1] == 's')
			{
				if(index < ftab[0].tref)
				{
					fprintf(fout, "sw $v0, -%d($t9)\n", 8 + tab[index].adr);
				}
				else
				{
					fprintf(fout, "sw $v0, -%d($sp)\n", 8 + tab[index].adr);
				}
			}
			else if(codes[i].result[1] == 't')
			{
				fprintf(fout, "sw $v0, %d($t8)\n", index * 4 > 0 ? -4 * index : 0);
			}
		}
		else if(strcmp(codes[i].type, "WRITE STRING") == 0)
		{
			fprintf(fout, "# WRITE STRING\n");
			fprintf(fout, "la $a0, %s\n", codes[i].result);
			fprintf(fout, "li $v0, 4\n");
			fprintf(fout, "syscall\n");
		}
		fprintf(fout, "\n");
	}
	fclose(fout);
}