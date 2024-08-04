//      The File Manipulate Test Program of C
//      Author:R6bandito
//      Time:24/8/2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_MOD "rb+"                               // 宏定义文件打开模式，便于对打开文件的权限进行操作
//#define FILE_ADDRESS "X:\\Your Path\\Database.bin" // 宏定义文件打开绝对路径
#define FILE_ADDRESS "Database.bin"   //宏定义文件打开相对路径
char ch[20];

FILE *OpenFile();                    // 声明打开文件函数
void WriteFile(FILE *fp);            // 声明写入数据函数
void StringInput(char *p, size_t m); // 声明输入功能函数
void ReadFile(FILE *fp);    //声明读取文件函数
void FixFile(FILE *fp);     //声明修改数据函数
void DeleteFile(FILE *fp);  //声明删除数据函数

int main(int argc, const char *argv[])
{
    FILE *fp = OpenFile();  //打开文件
    WriteFile(fp);  //写入文件

    fp=OpenFile();  //由于上次操作已经将文件关闭，因此在进行接下来的操作时应重新打开文件使文件指针重新指向该文件   
    ReadFile(fp);   //读取文件

    fp=OpenFile();
    FixFile(fp);    //修改文件

    fp=OpenFile();
    DeleteFile(fp); //删除数据

    fp=OpenFile();
    ReadFile(fp);
    return 0;
}

void StringInput(char *p, size_t m) // 定义输入功能函数
{
    if (fgets(p, m, stdin) != NULL) // 剔除掉通过fgets输入的数据流中换行符的影响
    {
        p[strcspn(p, "\n")] = '\0';
    }
}

/*注:并不建议将文件的打开定义为功能函数，虽然能够缩减代码量，但是不同的文件操作所需打开权限是不同的。而宏定义由于修改的局限性，无法满足需求
全部操作用统一的权限打开可能会出现不可预知的错误。*/

FILE *OpenFile() // 定义文件打开函数
{
    // FILE *fp=fopen("D:\\vsc\\C noob\\Database.bin","FILE_MOD") //按绝对路径打开文件
    FILE *fp = fopen(FILE_ADDRESS, FILE_MOD); // 按相对路径打开文件
    if (fp == NULL)
    {
        printf("Error!Can not open the file(Press any keys to continue)\n");
        getchar(); // getchar函数从键盘读取一个输入，并不显示。放在此处目的是使程序暂停，留时间给用户阅读。
        exit(1);
    }
    return fp; // 文件成功打开，返回文件指针
}

void WriteFile(FILE *fp) // 定义文件写入函数
{
    while (1)
    {
        printf("Please input a string(between 1-18)\n");
        StringInput(ch, sizeof(ch));
        size_t write = fwrite(ch, sizeof(char), sizeof(ch), fp); // 将输入的数据写入文件，一次写一个数据
        if (write != sizeof(ch))                                // 当成功写入时，fwrite会返回写入数据的数量。可据此来检查是否成功写入
        {
            printf("Something wrong with writing the file\n");
            fclose(fp); // 文件不使用时应将其关闭
            getchar();
            exit(1);
        }
        printf("Press E to quit(or other to continue)\n");
        StringInput(ch,sizeof(ch));
        if((ch[0] == 'E' || ch[0] == 'e') && ch[1] == '\0')
        {
            fclose(fp);
            return; //结束函数
        }
    }
}

void ReadFile(FILE *fp) //定义读取数据函数
{
    printf("Ready to Read and Print\n");
    while(fread(ch,sizeof(char),sizeof(ch),fp) == sizeof(ch))  //当读取到相关数据时，fread返回读取到数据的数量。当fread读取错误或读取到末尾时，将会返回一个小于读取数量的值
    {
        printf("%s\t",ch);  //读取之后打印
    }
    fclose(fp); //打印操作结束，关闭文件
}

void FixFile(FILE *fp)  //定义修改数据函数
{
    char _data[20];
    printf("\nInput the data that you want to fix:\t");
    StringInput(_data,sizeof(_data));

    while(fread(ch,sizeof(char),sizeof(ch),fp) == sizeof(ch))
    {
        if(strcmp(ch,_data) == 0)   //比较用户的输入是否在文件中有匹配的数据
        {
            char _temp[20];
            printf("Successfully found.Input new data\n");
            StringInput(_temp,sizeof(_temp));   

//注意！此处文件打开方式不能为ab+！因为ab+模式的写操作只能在文件末尾进行！
            strcpy(ch,_temp);   //用新数据覆盖旧数据
//将文件指针向前移动一个数据，准备接下来的复写操作（复写操作是必须的，修改后的文件必须重新将其写回文件相应位置）
            fseek(fp,-sizeof(ch),SEEK_CUR); //fseek用于移动指针
            size_t write = fwrite(_temp,sizeof(char),sizeof(ch),fp);    //复写修改过后的位置
            if(write != sizeof(ch))  //判断是否复写成功
            {
                printf("Failed\n");
                fclose(fp);
                return; //结束程序
            }
            fflush(fp);
//重新读取并打印文件
            fseek(fp,0,SEEK_SET);   //将文件指针放回开头
            while(fread(ch,sizeof(char),sizeof(ch),fp) == sizeof(ch))   //重新读取并打印
            {
                printf("\n%s\t",ch);
            }
            fclose(fp); //关闭文件
            return;  //终止函数，操作结束
        }
        continue;
    }
    printf("Didn't found the relative data\n");
    fclose(fp);
    return;
}

void DeleteFile(FILE *fp)   //定义文件中删除数据函数
{
//并不能从文件中直接删去数据，因此要达到删除目的，应是将要删除数据之外的数据重新读取写入新文件，后重新命名文件达到删除效果
    char _NAME[20];
    printf("\nPlease input the data you want to delete\n");
    StringInput(_NAME,sizeof(_NAME));
    FILE *fpn=fopen("Temp.bin","wb+");  //新创建一个临时文件用于存储修改数据
    if(fpn == NULL)
    {
        printf("Oops!Something wrong happened\n");
        return;
    }

    while(fread(&ch,sizeof(char),sizeof(ch),fp) == sizeof(ch))  //往数据缓冲区ch中读入文件数据，读取数据类型为字符，一次读取20个，读取成功返回20
    {
        if(strcmp(ch,_NAME) != 0)   //将不吻合用户输入的数据读取写入新文件，符合条件的数据跳过不写入，达到删除效果
        {
            fwrite(&ch,sizeof(char),sizeof(ch),fpn);
            continue;
        }
        else{
            continue;
        }
    }
    fclose(fp); //操作结束关闭文件
    fclose(fpn);
    //删除旧文件且重命名新文件，达到删除效果
    remove("Database.bin");
    rename("Temp.bin","Database.bin");
}