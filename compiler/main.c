#include<stdio.h>
#include<ctype.h>
#include<string.h>
#define ILNGMAX 30 //标识符长度最大值
#define STRLNGMAX 100 //字符串长度最大值
#define MAX_DIGIT 9 //int最大位数
#define MAX_LINE 200 //最大行长度
#define MAX_TAB 2000//表项最大值
#define SMAX 10000 //字符串常量表最大值
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
    int elsize;
    int size;
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
int cc; //当前所取字符在该行的位置
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

//函数声明
void getch();//获取一个字符
void error(int n);//error都直接return了，这里应该再读一个字符
void getsym();
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
void error(int n)//error都直接return了，这里应该再读一个字符
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
        strcpy(id, a);
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
            //登录数组表
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

            //登录符号表
            strcpy(tab[t].name, lastid);
            if(t == 0)
                tab[t].link = 0;
            else
                tab[t].link = 1;
            tab[t].obj = variable;
            tab[t].typ = arraytype;
            tab[t].ref = a;
            tab[t].adr = 0;//TODO:变量在运行栈中分配存储单元的相对地址
            t++;
            a++;
        }
        else
        {
            //登录符号表
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
            tab[t].adr = 0;//TODO:变量在运行栈中分配存储单元的相对地址
            t++;
        }
    }
    else//函数
    {
        ftab[f].tref = t;
        ftab[f].parnum = 0;
        ftab[f].psize = 0;//TODO:参数及该函数在运行栈S中的内务信息区所占存储单元数
        ftab[f].vsize = 0;//TODO:所占存储单元总数

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
        tab[t].adr = 0;//TODO:函数相应目标代码的入口地址
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
            strcpy(lastid, id);
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
            strcpy(lastid, id);
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
void variabledec()//处理变量声明部分 TODO:还没有登录符号表
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
        entertable(variable, lastsy);
        printf("line%d.%d 变量声明语句：%s %s;\n", l, cc, symbolvalue[lastsy], lastid);
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
                    printf("line%d.%d 变量声明语句：%s %s[%d];\n", l, cc, symbolvalue[lastsy], id, inum);
                    getsym();
                }
            }
            else
            {
                entertable(variable, lastsy);
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
    printf("line%d.%d 有返回值函数定义分析开始\n", l, cc);
    entertable(function, lastsy);
    parameterlist();
    compoundstatement();
    f++;
    if(tab[t-1].link != 0)
        tab[t-1].link = 2;
    printf("line%d.%d 有返回值函数定义分析完成\n", l, cc);
}
void noreturnfctdec()//处理无返回值函数定义
{
    printf("line%d.%d 无返回值函数定义分析开始\n", l, cc);
    entertable(function, lastsy);
    parameterlist();
    compoundstatement();
    f++;
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
                strcpy(lastid, id);
                entertable(variable, lastsy);
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
    printf("line%d.%d 表达式分析完成\n", l, cc);
}
void term()//预读一个，多读一个
{
    factor();
    while(sym == times || sym == div)
    {
        getsym();
        factor();
    }
    printf("line%d.%d 项分析完成\n", l, cc);
}
void factor()//预读一个，多读一个
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
                printf("line%d.%d 因子为<标识符>[<表达式>]的形式\n", l, cc);
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
            printf("line%d.%d 因子为<有返回值函数调用语句>的形式\n", l, cc);
        }
        else
        {
            printf("line%d.%d 因子为<标识符>的形式\n", l, cc);
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
        getsym();
    }
    else if(sym == intcon)
    {
        printf("line%d.%d 因子是一个整数\n", l, cc);
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
        printf("id: %2d   name: %10s   tref: %3d   eltyp: %1d   high: %5d   elsize: %1d   size: %3d\n",
            i, tab[atab[i].tref].name, atab[i].tref, atab[i].eltyp, atab[i].high, atab[i].elsize, atab[i].size);
    }
}
