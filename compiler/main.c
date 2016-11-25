#include<stdio.h>
#include<ctype.h>
#include<string.h>
#define ILNGMAX 30 //��ʶ���������ֵ
#define STRLNGMAX 100 //�ַ����������ֵ
#define MAX_DIGIT 9 //int���λ��
#define MAX_LINE 200 //����г���
#define MAX_TAB 2000//�������ֵ
#define SMAX 10000 //�ַ������������ֵ
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
};
//���ű�
struct table{
    char name[ILNGMAX+1];
    int link;
    enum obj obj;
    enum type typ;
    int ref;
    int adr;
}tab[MAX_TAB];
int t;
//�����
struct arraytable{
    enum type eltyp;
    int tref;
    int high;
    int elsize;
    int size;
}atab[MAX_TAB];
int a;
//������
struct functiontable{
    int tref;
    int parnum;
    int psize;
    int vsize;
}ftab[MAX_TAB];
int f;
//�ַ���������
char stab[SMAX];
int sx;

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
int cc; //��ǰ��ȡ�ַ��ڸ��е�λ��
char line[MAX_LINE]; //��ǰ��
int ll; //��ǰ�г���
int l; //��ǰ��ȡ�ַ���������
enum symbol sym; //ʶ�𵽵ĵ�������
int inum; //ʶ�𵽵�����
char c; //ʶ�𵽵��ַ�
char id[ILNGMAX+1]; //ʶ�𵽵ı�ʶ��
char string[STRLNGMAX+1]; //ʶ�𵽵��ַ���
enum symbol lastsy;
char lastid[ILNGMAX+1];

//��������
void getch();//��ȡһ���ַ�
void error(int n);//error��ֱ��return�ˣ�����Ӧ���ٶ�һ���ַ�
void getsym();
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


int main()
{
    int i;
    char sin[FILENAME_MAX];
    printf("please input source program file name : \n");
    scanf("%s", sin);
    fin = fopen(sin, "r");
    if(fin == NULL)
    {
        printf("Failed to open!\n");
        return 1;
    }
    printf("Open successfully!\n");

    cc = -1;
    ll = 0;
    l = 0;
    i = 1;
    t = 0;
    a = 0;
    f = 0;
    sx = 0;
    getch();
    /*while(!feof(fin))
    {
        getsym();
        if(ch == -1)
            break;
        print(i++);
    }*/
    program();
    printf("a: %d\n", a);
    printtable();
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
void error(int n)//error��ֱ��return�ˣ�����Ӧ���ٶ�һ���ַ�
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
        strcpy(id, a);
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
                        while(k < STRLNGMAX && ch != '\"' && ch >= 32 && ch <= 126)
                        {
                            string[k++] = ch;
                            getch();
                        }
                        string[k] = '\0';
                        if(k == STRLNGMAX && ch != '\"' && ch >= 32 && ch <= 126) //�ַ�������
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
void entertable(enum obj object, enum type thetype)
{
    if(object == constant)
    {
        strcpy(tab[t].name, lastid);
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
        }
        else if(thetype == chartype)
        {
            tab[t].typ = chartype;
            tab[t].adr = c;
        }
        t++;
    }
    else if(object == variable)
    {
        if(thetype == arraytype)
        {
            //��¼�����
            if(lastsy == intsy)
            {
                atab[a].eltyp = inttype;
                atab[a].elsize = 4;
            }
            else
            {
                atab[a].eltyp = chartype;
                atab[a].elsize = 1;
            }
            atab[a].tref = t;
            atab[a].high = inum;
            atab[a].size = atab[a].elsize * inum;

            //��¼���ű�
            strcpy(tab[t].name, lastid);
            if(t == 0)
                tab[t].link = 0;
            else
                tab[t].link = 1;
            tab[t].obj = variable;
            tab[t].typ = arraytype;
            tab[t].ref = a;
            tab[t].adr = 0;//TODO:����������ջ�з���洢��Ԫ����Ե�ַ
            t++;
            a++;
        }
        else
        {
            //��¼���ű�
            strcpy(tab[t].name, lastid);
            if(t == 0)
                tab[t].link = 0;
            else
                tab[t].link = 1;
            tab[t].obj = variable;
            if(lastsy == intsy)
                tab[t].typ = inttype;
            else
                tab[t].typ = chartype;
            tab[t].ref = 0;
            tab[t].adr = 0;//TODO:����������ջ�з���洢��Ԫ����Ե�ַ
            t++;
        }
    }
    else//����
    {
        ftab[f].tref = t;
        ftab[f].parnum = 0;
        ftab[f].psize = 0;//TODO:�������ú���������ջS�е�������Ϣ����ռ�洢��Ԫ��
        ftab[f].vsize = 0;//TODO:��ռ�洢��Ԫ����

        strcpy(tab[t].name, lastid);
        tab[t].link = 0;
        tab[t].obj = function;
        if(lastsy == intsy)
            tab[t].typ = inttype;
        else if(lastsy == charsy)
            tab[t].typ = chartype;
        else
            tab[t].typ = voidtype;
        tab[t].ref = f;
        tab[t].adr = 0;//TODO:������ӦĿ��������ڵ�ַ
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
            strcpy(lastid, id);
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
            strcpy(lastid, id);
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
            strcpy(lastid, id);
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
            strcpy(lastid, id);
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
                        strcpy(lastid, id);
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
                        strcpy(lastid, id);
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
void variabledec()//��������������� TODO:��û�е�¼���ű�
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
        entertable(variable, lastsy);
        printf("line%d.%d ����������䣺%s %s;\n", l, cc, symbolvalue[lastsy], lastid);
    }
    while(sym == comma)
    {
        getsym();
        strcpy(lastid, id);
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
                entertable(variable, lastsy);
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
    printf("line%d.%d �з���ֵ�������������ʼ\n", l, cc);
    entertable(function, lastsy);
    parameterlist();
    compoundstatement();
    f++;
    if(tab[t-1].link != 0)
        tab[t-1].link = 2;
    printf("line%d.%d �з���ֵ��������������\n", l, cc);
}
void noreturnfctdec()//�����޷���ֵ��������
{
    printf("line%d.%d �޷���ֵ�������������ʼ\n", l, cc);
    entertable(function, lastsy);
    parameterlist();
    compoundstatement();
    f++;
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
                strcpy(lastid, id);
                entertable(variable, lastsy);
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
    entertable(function, lastsy);
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
        strcpy(lastid, id);
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
        strcpy(lastid, id);
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
    printf("line%d.%d if [else]���������\n", l, cc);
}
void dostatement()//Ԥ��һ�������һ��
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
    getsym();
    if(sym != becomes)
    {
        error(25);
    }
    getsym();
    expression();
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
    printf("line%d.%d for���������\n", l, cc);
}
void condition()//û��Ԥ�������һ��
{
    getsym();
    expression();
    if(sym == eql || sym == neq || sym == gtr
    || sym == geq || sym == lss || sym == leq)
    {
        getsym();
        expression();
    }
    printf("line%d.%d �����������\n", l, cc);
}
void assignment()//Ԥ����=��[�����һ��
{
    if(sym == becomes)
    {
        getsym();
        expression();
    }
    else if(sym == lbrack)
    {

        getsym();
        expression();
        if(sym != rbrack)
        {
            error(15);
        }
        getsym();
        if(sym != becomes)
        {
            error(25);
        }
        getsym();
        expression();
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
    printf("line%d.%d return���������\n", l, cc);
}
void returnfctstatement()//Ԥ����(
{
    valueparalist();
    if(sym != rparent)
    {
        error(17);
    }
    getsym();
    printf("line%d.%d �з���ֵ�����������������\n", l, cc);
}
void noreturnfctstatement();
void valueparalist()//û��Ԥ�������һ��
{
    do{
        getsym();
        if(sym == rparent)
            return;
        expression();
    }while(sym == comma);
}
void expression()//Ԥ��һ�������һ��
{
    if(sym == plus || sym == minus)
    {
        getsym();
    }
    term();
    while(sym == plus || sym == minus)
    {
        getsym();
        term();
    }
    printf("line%d.%d ���ʽ�������\n", l, cc);
}
void term()//Ԥ��һ�������һ��
{
    factor();
    while(sym == times || sym == div)
    {
        getsym();
        factor();
    }
    printf("line%d.%d ��������\n", l, cc);
}
void factor()//Ԥ��һ�������һ��
{
    if(sym == ident)
    {
        strcpy(lastid, id);
        getsym();
        if(sym == lbrack)
        {
            getsym();
            expression();
            if(sym == rbrack)
            {
                printf("line%d.%d ����Ϊ<��ʶ��>[<���ʽ>]����ʽ\n", l, cc);
            }
            else
            {
                error(15);
            }
            getsym();
        }
        else if(sym == lparent)
        {
            returnfctstatement();
            printf("line%d.%d ����Ϊ<�з���ֵ�����������>����ʽ\n", l, cc);
        }
        else
        {
            printf("line%d.%d ����Ϊ<��ʶ��>����ʽ\n", l, cc);
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
        getsym();
    }
    else if(sym == intcon)
    {
        printf("line%d.%d ������һ������\n", l, cc);
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
        printf("id: %2d   name: %10s   tref: %3d   eltyp: %1d   high: %5d   elsize: %1d   size: %3d\n",
            i, tab[atab[i].tref].name, atab[i].tref, atab[i].eltyp, atab[i].high, atab[i].elsize, atab[i].size);
    }
}
