#include<iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <ctime>
using namespace std;
bool passwd(); //验证密码
void init();   //初始化，将数据文件读入内存
void menu();   //菜单
void View();   //显示所有停车信息
void Park();   //停车
void Get();    //取车
void searchCarNum();  //通过车牌号搜索
void searchParkNum(); //通过车位号搜索
void Exit();   //将内存数据写入文件，然后退出程序
void disp(int i); //显示park数组第i位的数据
int Parking_duration(int parknum); //获取停车时长，返回值为分钟

struct Park //车位结构体 
{
    bool flag; //车位状态，true是无车，false是有车
    string carnum; //车牌号
    tm starttime; //开始时间
    string type;  //车类型：大小车
}park[100] = {}; //停车场容量100
int main() {
    system("mode con cols=100 lines=30");
    if (passwd()) {
        init(); //如果密码验证成功就初始化数据
        menu(); //菜单
    }
    else
        cout << "密码错误！";
}
bool passwd() {
    try
    {
        string pass,str;
        ifstream  passwordfile("password.txt"); //打开文件
        passwordfile >> pass; //读取正确密码
        cout << "请输入密码：";
        cin >> str;
        if (str.compare(pass)) //判断是否吻合
            return true;
        else
            return false;
        passwordfile.close();//关闭文件
    }
    catch (const std::exception&)//异常处理
    {
        cout << "读取失败";
        exit(1);
    }
}
void init() {
    string line;
    ifstream in("data.txt");
    for (auto & i : park) {//将车位初始状态都设置为无车
        i.flag = true;
    }
    while (getline(in, line)) { //逐行读取
        stringstream ss; //文本流
        string carnum, type;
        int parknum, year, mounth, day, hour, minute;
        ss << line; //将字符串存入流中
        ss >> carnum >> parknum >> year >> mounth >> day >> hour >> minute >> type; //读取数据
        parknum--; //车位号在数组中以0开始
        tm temp{} ; //时间
        temp.tm_year = year-1900;
        temp.tm_mon = mounth-1;
        temp.tm_mday = day;
        temp.tm_hour = hour;
        temp.tm_min = minute;
        park[parknum].carnum = carnum;
        park[parknum].flag = false;
        park[parknum].starttime = temp;
        park[parknum].type = type;
    }
    in.close();
}
void menu() {
    system("cls");
    int choice = 0;
    printf("\t\t欢迎进入泊车管理系统\n\n");
    printf("\t\t1.显示停车信息\n");
    printf("\t\t2.停放车辆\n");
    printf("\t\t3.取走车辆\n");
    printf("\t\t4.车牌号查询\n");
    printf("\t\t5.车位查询\n");
    printf("\t\t0.退出管理系统\n");
    printf("\t\t请选择选项（0-5）：");
    cin >> choice;
    switch (choice)
    {
        case 1:
            View();
            break;
        case 2:
            Park();
            break;
        case 3:
            Get();
            break;
        case 4:
            searchCarNum();
            break;
        case 5:
            searchParkNum();
            break;
        case 0:
            Exit();
            break;
        default:
            cout<<"输入错误！"<<endl;
    }
}
void View() {
    system("cls");
    int num = 0;
    string title[] = { "车牌号：","车位号：","时间：","车类型：" };
    string timeunit[] = { "年","月","日","时","分" };
    for (int i = 0; i < 100; i++) {
        if (!park[i].flag) {
            disp(i);
            num++;
        }
    }
    cout << "共查询到" << num << "条记录！"
         <<"\n\n按回车键返回主菜单"<< endl;
    getchar();
    getchar();
    menu();
}
void Park() {
    system("cls");
    string CarNum;
    int ParkNum=-1;
    for (int i = 0; i < 100; i++) {//选取第一个空车位作为车位号
        if (park[i].flag) {
            ParkNum = i;
            break;
        }
    }
    if (ParkNum == -1) {
        cout << "停车位已满！" <<"\n\n按回车键返回主菜单"<< endl;
        getchar();
        getchar();
        menu();
    }
    struct tm starttime;   //当前时间
    time_t now;  //声明time_t类型变量
    time(&now);      //获取系统日期和时间
    localtime_s(&starttime, &now);   //获取当地日期和时间

    string Type;
    cout << "请输入车牌号：";cin >> CarNum;
    cout << "请输入车类型："; cin >> Type;
    park[ParkNum].flag = false; //将车位状态设置为有车
    park[ParkNum].carnum = CarNum;//存入信息
    park[ParkNum].starttime =starttime;
    park[ParkNum].type = Type;
    cout << "\n\n您的停车信息：\n";
    disp(ParkNum);//回显信息
    cout << "\n\n按回车键返回主菜单";
    getchar();
    getchar();
    menu();
}
void Get() {
    system("cls");
    int parknum;
    cout << "请输入车位号：";
    cin >> parknum;
    parknum--;
    if (park[parknum].flag || parknum < 0 || parknum>=100) {//确保车位号正确且车位上面确实有车
        cout << "\n\n输入错误，按回车键返回主菜单";
        getchar();
        getchar();
        menu();
    }
    else {
        int time=Parking_duration(parknum),price;//根据当前时间和停车开始时间计算停车时长
        double cost;
        if (park[parknum].type.compare("小车"))
            price = 5;
        else price = 7;
        if (time % 30 == 0)
            cost = (time / 30) * price / 2.0;
        else
            cost = (time / 30+1) * price / 2.0;
        cout << "\n\n请支付" << cost << "元停车费" << endl;
        cout << "\n\n按回车键返回主菜单";
        park[parknum].flag = true;//取车完成，车位状态设为无车
        getchar();
        getchar();
        menu();
    }
}
void searchCarNum() {
    system("cls");
    string carnum = "";
    bool flag=false;//查询成功标志
    cout << "请输入要搜索的车牌号：";
    cin >> carnum;
    cout << "\n\n";
    for (int i = 0; i < 100; i++) {//逐个比对
        if (!park[i].flag && park[i].carnum.compare(carnum)) {
            disp(i);
            flag = true;
            break;
        }
    }
    if (!flag)
        cout << "未查询到任何车辆！";
    cout << "\n\n按任意键返回主菜单";
    getchar();
    getchar();
    menu();
}
void searchParkNum() {
    system("cls");
    int parknum;
    bool flag = false;
    cout << "请输入要搜索的车位号：";
    cin >> parknum;
    parknum--;
    cout << "\n\n";
    if (!park[parknum].flag)
        disp(parknum);
    else
        cout << "未查询到任何车辆！";
    cout << "\n\n按任意键返回主菜单";
    getchar();
    getchar();
    menu();
}
void Exit() {
    ofstream data("data.txt");

    for (int i = 0; i < 100; i++) {//逐个将有车的车位数据写入文件
        if (!park[i].flag) {
            data << park[i].carnum << ' ' << i + 1 << ' '
                 << park[i].starttime.tm_year+1900 << ' ' << park[i].starttime.tm_mon+1 << ' '
                 << park[i].starttime.tm_mday << ' ' << park[i].starttime.tm_hour << ' '
                 << park[i].starttime.tm_min<<' '<< park[i].type
                 <<endl;
        }
    }

    exit(0);
}
void disp(int i) {
    string title[] = { "车牌号：","车位号：","时间：","车类型：" };
    string timeunit[] = { "年","月","日","时","分" };
    cout << title[0] << park[i].carnum << endl;
    cout << title[1] << i + 1 << endl;
    cout << title[2] << park[i].starttime.tm_year +1900<< timeunit[0]
         << park[i].starttime.tm_mon+1 << timeunit[1]
         << park[i].starttime.tm_mday << timeunit[2]
         << park[i].starttime.tm_hour << timeunit[3]
         << park[i].starttime.tm_min << timeunit[4] << endl;
    cout << title[3] << park[i].type << endl;
    cout << "--------------------------------------\n";
}
int Parking_duration(int parknum) {
    int min;
    tm start = park[parknum].starttime;
    struct tm stop;   //tm结构指针
    time_t now;  //声明time_t类型变量
    time(&now);      //获取系统日期和时间
    localtime_s(&stop, &now);   //获取当地日期和时间
    min =((stop.tm_mday - start.tm_mday)* 24 + stop.tm_hour - start.tm_hour) * 60+stop.tm_min-start.tm_min;
    return min;
}