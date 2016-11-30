#include<stdio.h>
#include<ctype.h>
#include<string.h>
#define ILNGMAX 30 //��ʶ���������ֵ
#define STRINGMAX 200 //�ַ����������ֵ
#define MAX_DIGIT 9 //int���λ��
#define MAX_LINE 200 //����г���
#define MAX_TAB 2000//�������ֵ
#define SMAX 10000 //�ַ������������ֵ
#define MAX_STACK 10000 //ջ���ֵ
#define MAX_LABEL 1000 //��ǩ�������ֵ
#define MAX_CODE 20000 //��Ԫʽ�������ֵ
#define MAX_CODE_TYPE 20 //��Ԫʽ������󳤶�
#define MAX_PAR_NUM 10 //��������������
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
}; //ö������
enum obj{
    constant, variable, function
}; //��ʶ������
enum type{
    inttype, chartype, arraytype, voidtype
}; //��ʶ������
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
}; //ö������
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
    "��ʶ������", //0
    "���Ϸ��ַ�", //1
    "��������ֻ����һ���ַ�", //2
    "�ַ�������", //3
    "ȱ��\"", //4
    "ȱ��'", //5
    "��������", //6
    "ǰ��0���Ϸ�", //7
    "ӦΪint����", //8
    "ӦΪint��char", //9
    "ӦΪ��ʶ��", //10
    "����˵����int��char��ӦΪ��ֵ��", //11
    "ӦΪ','��';'", //12
    "ӦΪ�ַ�����", //13
    "int��ı�ʶ����ӦΪ'['��','��';'��'('", //14
    "ӦΪ']'", //15
    "void��ӦΪmain������", //16
    "ӦΪ')'", //17
    "ӦΪ'{'", //18
    "int��ı�ʶ����ӦΪ'['��','��';'", //19
    "ӦΪ'}'", //20
    "ӦΪ'='��'['��'('", //21
    "ӦΪ'('", //22
    "���Ӹ�ʽ����", //23
    "ӦΪwhile", //24
    "ӦΪ=", //25
    "ӦΪ;", //26
    "ӦΪ+��-", //27
    "ӦΪ[", //28
    "ӦΪ,��)", //29
    "ӦΪ(��;", //30
    "ӦΪint������+��-", //31
    "��ʶ���ظ�����", //32
	"���ű����", //33
	"������Ϣ���������", //34
	"���������", //35
	"���Ͳ�ƥ���δ����", //36
	"�����±����Ϊint����", //37
	"����ʵ�����β����������Ͳ���", //38
	"�з���ֵ�������ܷ��ؿ�", //39
	"�޷���ֵ����ֻ�ܷ��ؿ�", //40
};
//���ű�
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
//�����
struct arraytable{
    enum type eltyp;
    int tref;
    int high;
}atab[MAX_TAB];
int a;
//������
struct functiontable{
    int tref;
    int parnum;
	int size;
	int lasttvs;
}ftab[MAX_TAB];
int f;
//�ַ���������
char strtable[SMAX][STRINGMAX];
int stringx;

const int nkw = 13; //�����ָ���
const char word[13][8] = { //����������
    "char", "const", "do", "else", "for", "if", "int",
    "main", "printf", "return", "scanf", "void", "while",
};
enum symbol wsym[13] = {//�����ֶ�Ӧ����
    charsy, constsy, dosy, elsesy, forsy, ifsy, intsy,
    mainsy, printfsy, returnsy, scanfsy, voidsy, whilesy
};
FILE *fin; //�����ļ�ָ��
char ch; //��ǰ�����ַ�
int cc = -1; //��ǰ��ȡ�ַ��ڸ��е�λ��
char line[MAX_LINE]; //��ǰ��
int ll; //��ǰ�г���
int l; //��ǰ��ȡ�ַ���������
enum symbol sym; //ʶ�𵽵ĵ�������
int inum; //ʶ�𵽵�����
char c; //ʶ�𵽵��ַ�
char id[ILNGMAX+1]; //ʶ�𵽵ı�ʶ��
char string[STRINGMAX+1]; //ʶ�𵽵��ַ���
enum symbol lastsy;//��������һ����������
char lastid[ILNGMAX+1];//��������һ����ʶ��
int funcvalid;//�����Ƿ���Ч
int cur_adr;//�����ͱ�����ǰ����Ե�ַ

//��Ԫʽ�洢����
struct code
{
	char type[MAX_CODE_TYPE+1];
	char arg1[ILNGMAX+1];
	char arg2[ILNGMAX+1];
	char result[ILNGMAX+1];
}codes[MAX_CODE];
int codeindex = 0;

//��ʱ����ջ����ָ��
int tvs_top = 1;
//Label��ջ
int labx;//labxΪ��ǰ�õ���Label������

char temp_s[ILNGMAX+1];
char temp_s1[ILNGMAX+1];
char temp_s2[ILNGMAX+1];
char temp_s3[ILNGMAX+1];
enum identtype lasttype;
enum type exptype;
int lasttemp;
int ifpar;
int cur_func;//��ǰ������ftab���е�λ��
int cur_tabx;//��ǰ��ѯ��ʶ����tab���е�λ��

//��������
void getch();//��ȡһ���ַ�
void error(int n);
void getsym();
char* inttoa(int n);
int atoint(char s[]);
char* tempvar(int n, int flag);//������ʱ����
char* tolabel(int n);
char* totable(int n, int flag);
char* tofunc(int n,int flag);
char* tostring(int n);
int intfromtabx(char s[]);
void gen();//������Ԫʽ
void ifdefine(char id_temp[], enum identtype typ);
int findduplicate(enum obj object);
void entertable(enum obj object, enum type thetype);//��¼���ű�
void program();//�����ܳ���
void constdec();//��������������
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
void getch()//��ȡһ���ַ�
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
    printf("Error%d in line%d.%d ������Ϣ��%s\n", n, l, cc, errormessage[n]);
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
        if(k == ILNGMAX && (isalpha(ch) || isdigit(ch) || ch == '_'))//��ʶ������
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
                                //��ch��Ϊ' �ٶ�����ַ�
                                //���������' ������������ֻ����һ���ַ�
                                //���û����������ȱ�ٵ�����
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
                                    error(5);//ȱ�ٵ�����
                                    while(isalpha(ch) || isdigit(ch) || ch == '_' || ch == '+' ||
                                        ch == '-' || ch == '*' || ch == '/' || ch == '\'')//��ȥ�����ַ�
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
                                ch == '-' || ch == '*' || ch == '/' || ch == '\'')//��ȥ�����ַ�
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
                        if(k == STRINGMAX && ch != '\"' && ch >= 32 && ch <= 126) //�ַ�������
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
                            else//chΪ"
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
void ifdefine(char id_temp[], enum identtype typ)//ֻ����Ƿ�����Լ������Ƿ�ƥ��
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
int findduplicate(enum obj object)//���ұ�ʶ���Ƿ��ظ�����
//�������£�
//1. ��������������뵱ǰ�����ڵĳ��������ͬ�����������뺯����ͬ����������ȫ�ֳ��������ͬ����
//2. �������Ժ͵�ǰ�����ڵĳ�����ͬ�������Ժ����������ڵĳ�����ͬ���������Ժ���������ͬ���������Ժ�ȫ�ֱ���ͬ����
{
    int i;
    if(object == constant || object == variable)
    {
        //�������߱��� ֱ����ǰ�� ���������
        //1. �ҵ�linkΪ0��Ҳ���Ǻ�����
        //2. �ҵ����ű�ͷ
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
        //���� �������ܺͱ�ĺ���ͬ�� ���ܺ�ȫ�ֱ���ͬ��
        if(t >= 1)
        {
            if(tab[0].obj == constant || tab[0].obj == variable)
            {
                if(strcmp(tab[0].name, lastid) == 0)//�鿴��һ���Ƿ�ͬ��
                    return 1;
                else
                {
                    for(i = 1; i < t; i++)
                    {
                        if(tab[i].link == 0)
                            break;
                        if(strcmp(tab[i].name, lastid) == 0)//�鿴����ȫ�ֱ����Ƿ��뺯��ͬ��
                        {
                            return 1;
                        }
                    }
                }
            }
        }
        //�鿴���к����Ƿ��뵱ǰ����ͬ��
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
			//��¼�����
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

            //��¼���ű�
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
            //��¼���ű�
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
    else//����
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
void program()//�����ܳ���
{
    getsym();
    if(sym == constsy)
    {
        constdec();
    }
    printf("line%d.%d ȫ�ֳ����������ִ������\n", l, cc);
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
                //��������
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
                //����Ĵ�������ʱû��˼·����ʱֱ�ӽ���
                error(14);
                printf("line%d.%d �����޷����������Ĵ��󣬳������\n", l, cc);
                return;
            }
        }
        else if(sym == voidsy)
        {
            getsym();
            strcpy_s(lastid, ILNGMAX+1, id);
            if(sym == mainsy)//������
            {
                maindec();
                printf("line%d.%d �﷨�������\n", l, cc);
                return;
            }
            else if(sym == ident)//�޷���ֵ����
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
                printf("line%d.%d �����޷����������Ĵ��󣬳������\n", l, cc);
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
            if(sym == mainsy)//������
            {
                maindec();
                printf("line%d.%d �﷨�������\n", l, cc);
                return;
            }
            else if(sym == ident)//�޷���ֵ����
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
                printf("line%d.%d �����޷����������Ĵ��󣬳������\n", l, cc);
                return;
            }
        }
    }
    if(sym == end)
    {
        printf("line%d.%d �����ļ���β\n", l, cc);
    }
    else
    {
        printf("line%d.%d δ�����꣬��������\n", l, cc);
    }
}
void constdec()//��������������
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
                                printf("line%d.%d ����������䣺const int %s = %d;\n", l, cc, lastid, inum);
                                getsym();
                            }
                            else
                            {
                                entertable(constant, inttype);
                                printf("line%d.%d ����������䣺const int %s = %d;\n", l, cc, lastid, inum);
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
                                printf("line%d.%d ����������䣺const char %s = '%c';\n", l, cc, lastid, c);
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
void variabledec()//���������������
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
            printf("line%d.%d ����������䣺%s %s[%d];\n", l, cc, symbolvalue[lastsy], lastid, inum);
            getsym();
        }
    }
    else
    {
        if(lastsy == intsy)
			entertable(variable, inttype);
		else
			entertable(variable, chartype);
        printf("line%d.%d ����������䣺%s %s;\n", l, cc, symbolvalue[lastsy], lastid);
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
                    printf("line%d.%d ����������䣺%s %s[%d];\n", l, cc, symbolvalue[lastsy], id, inum);
                    getsym();
                }
            }
            else
            {
				if(lastsy == intsy)
					entertable(variable, inttype);
				else
					entertable(variable, chartype);
                printf("line%d.%d ����������䣺%s %s;\n", l, cc, symbolvalue[lastsy], id);
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
void returnfctdec()//�����з���ֵ��������
{
    printf("line%d.%d �з���ֵ����%s���������ʼ\n", l, cc, lastid);
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
    printf("line%d.%d �з���ֵ����%s����������\n", l, cc, tab[ftab[f - 1].tref].name);
}
void noreturnfctdec()//�����޷���ֵ��������
{
    printf("line%d.%d �޷���ֵ�������������ʼ\n", l, cc);
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
    printf("line%d.%d �޷���ֵ��������������\n", l, cc);
}
void parameterlist()//���������������βμ����й���Ϣ��¼�����ű���
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
                printf("line%d.%d ����������%s %s\n", l, cc, symbolvalue[lastsy], id);
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
        printf("line%d.%d ����������ɣ�����%d������\n", l, cc, i);
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
    printf("line%d.%d main�����������\n", l, cc);
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
    printf("line%d.%d �����г����������ִ������\n", l, cc);
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
    printf("line%d.%d �����ڱ��������������\n", l, cc);
    statementlist();//�Ѿ����˵�һ������
    if(sym == rbrace)
    {
        printf("line%d.%d ������䴦�����\n", l, cc);
    }
    else
    {
        error(20);
    }
}
void statementlist()//Ԥ��һ��
{
    printf("line%d.%d �����ʶ��ʼ\n", l, cc);
    while(sym == ident || sym == ifsy || sym == dosy || sym == forsy || sym == scanfsy
        || sym == printfsy || sym == returnsy || sym == lbrace || sym == semicolon)
    {
        statement();
        if(sym == rbrace)
        {
            break;
        }
    }
    printf("line%d.%d �����ʶ�����\n", l, cc);
}
void statement()//Ԥ��һ�������һ��
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
            printf("line%d.%d ����е�����д������\n", l, cc);
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
void ifstatement()//Ԥ��һ�������һ��
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
    printf("line%d.%d if [else]���������\n", l, cc);
}
void dostatement()//Ԥ��һ�������һ��
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
        printf("line%d.%d do while���������\n", l, cc);
    }
    else
    {
        error(17);
    }
    getsym();
}
void forstatement()//Ԥ��һ�������һ��
{
	enum symbol localsym;
	int locallabel1, locallabel2, localnum, localtabx, localtabx1;
	getsym();
    if(sym != lparent)
    {
        error(22);
    }
	//��һ����ֵ���
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
	//����
    locallabel2 = condition(-1);
	if(locallabel2 == -1)
		return;
    if(sym != semicolon)
    {
        error(26);
    }
	//���沽����Ϣ
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
    printf("line%d.%d for���������\n", l, cc);
}
int condition(int n)//û��Ԥ�������һ��
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
		if(n == -1)//labelδ����������Ϊ��
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
		else//label�ѽ���������Ϊ��
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
    printf("line%d.%d �����������\n", l, cc);
	return labx - 1;
}
void assignment()//Ԥ����=��[�����һ��
{
    enum type lefttype;
	int indextemp;
	int localtabx;
	if(sym == becomes)
    {
		//�ж�Ϊ������ֵ�Ƿ���ȷ
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
		//�ж���������Ƿ���ȷ
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
		//����ֻ��Ҫ�����ʽ�����Ƿ���int��������Ҫ�ж��Ƿ񳬳������Ͻ�
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
    printf("line%d.%d ��ֵ���������\n", l, cc);
}
void readstatement()//Ԥ��һ�������һ��
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
    printf("line%d.%d scanf���������\n", l, cc);
}
void writestatement()//Ԥ��һ�������һ��
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
            printf("line%d.%d printf���������\n", l, cc);
        }
        else if(sym == rparent)
        {
            printf("line%d.%d printf���������\n", l, cc);
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
        printf("line%d.%d printf���������\n", l, cc);
    }
    getsym();
}
void returnstatement()//Ԥ��һ�������һ��
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
    printf("line%d.%d return���������\n", l, cc);
}
void callfctstatement()//Ԥ����(
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
    printf("line%d.%d �з���ֵ�����������������\n", l, cc);
}
int valueparalist()//û��Ԥ�������һ��
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
		//����int��char�����໥ת������������������
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
void expression()//Ԥ��һ�������һ��
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
    printf("line%d.%d ���ʽ�������\n", l, cc);
}
void term()//Ԥ��һ�������һ��
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
    printf("line%d.%d ��������\n", l, cc);
}
void factor()//Ԥ��һ�������һ��
{
	enum identtype localtype;
	int localtabx;
	if(sym == ident)
    {
		strcpy_s(lastid, ILNGMAX+1, id);
        getsym();
        if(sym == lbrack)
        {
            //�ж���������Ƿ���ȷ
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
			//����ֻ��Ҫ�����ʽ�����Ƿ���int��������Ҫ�ж��Ƿ񳬳������Ͻ�
			if(exptype != inttype)
			{
				error(37);
				return;
			}
            if(sym == rbrack)
            {
                printf("line%d.%d ����Ϊ<��ʶ��>[<���ʽ>]����ʽ\n", l, cc);
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
			//�ж��з���ֵ���������Ƿ���ȷ
			ifdefine(lastid, function_type);
			if(lasttype != function_char && lasttype != function_int)
			{
				error(36);
				return;
			}
			localtype = lasttype;
			callfctstatement();
            printf("line%d.%d ����Ϊ<�з���ֵ�����������>����ʽ\n", l, cc);
			gen("=CALL", tofunc(cur_func, 1), "", tempvar(tvs_top, 3));
			lasttemp = tvs_top++;
			if(localtype == function_int)
				exptype = inttype;
			else
				exptype = chartype;
        }
        else
        {
			//�жϳ�������������Ƿ���ȷ
			ifdefine(lastid, other);
			if(lasttype == no_type)
			{
				error(36);
				return;
			}
			printf("line%d.%d ����Ϊ<��ʶ��>����ʽ\n", l, cc);
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
            printf("line%d.%d ������(<���ʽ>)����ʽ\n", l, cc);
        }
        else
        {
            error(17);
        }
        getsym();
    }
    else if(sym == charcon)
    {
        printf("line%d.%d ������һ���ַ�\n", l, cc);
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
            printf("line%d.%d ������һ������\n", l, cc);
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
        printf("line%d.%d ������һ������\n", l, cc);
		gen("=", inttoa(inum), "", tempvar(tvs_top, 3));
		lasttemp = tvs_top++;
		exptype = inttype;
        getsym();
    }
    else
    {
        error(23);
        //TODO:�����������
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
	fprintf(fout, "lw $t8, temp\n");//$t8�洢��ʱ��������ַ
	fprintf(fout, "move $t9, $sp\n");//$t9�洢ȫ�ֱ�����ַ
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
			//��ȡt
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
			//��ȡn
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
			//$t2������a�ĵ�ַ
			if(index < ftab[0].tref)
			{
				fprintf(fout, "subi $t2, $t9, %d\n", 8 + adr);
			}
			else
			{
				fprintf(fout, "subi $t2, $sp, %d\n", 8 + adr);
			}
			fprintf(fout, "sub $t2, $t2, $t1\n");//��ʱ$t2����a[n]��ַ
			fprintf(fout, "sw $t0, 0($t2)\n");
		}
		else if(strcmp(codes[i].type, "=[]") == 0)//t = a[n]
		{
			fprintf(fout, "# =[]\n");
			index = intfromtabx(codes[i].arg1);
			adr = tab[index].adr - (atab[tab[index].ref].high - 1) * 4;
			index1 = intfromtabx(codes[i].arg2);
			index2 = intfromtabx(codes[i].result);
			//��ȡn
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
			//$t2������a�ĵ�ַ
			if(index < ftab[0].tref)
			{
				fprintf(fout, "subi $t2, $t9, %d\n", 8 + adr);
			}
			else
			{
				fprintf(fout, "subi $t2, $sp, %d\n", 8 + adr);
			}
			fprintf(fout, "sub $t2, $t2, $t1\n");//��ʱ$t2����a[n]��ַ
			fprintf(fout, "lw $t0, 0($t2)\n");
			//��t��ֵ
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