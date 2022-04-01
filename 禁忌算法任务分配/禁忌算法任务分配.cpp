#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

int GENERATION = 5000;//迭代次数
const int MANS = 9;//人数
const int NEIGHBOUR_SIZE = 32;//邻域范围,穷尽所有邻域应为9选2的组合数，为36。但要考虑禁忌长度，固选择32
const int LISTLENTH = 11;//禁忌表长度
const int TABULENTH = 6;//禁忌长度，采用邻域组合数开根号
int jump = 500;//历史最低连续jump代不变，跳出
int Plan[MANS]={1,2,3,4,5,6,7,8,9};//用数组存储任务序列，下标0，1，2，3对应工人1234...，数组元素对应任务编号1234.....
int curbestPlan[MANS+1];//当代除初始解外最优解，最后一位存储时间
int allbestPlan[MANS+2];//历史最优解，最后两位位存储时间，还有迭代不变的次数
clock_t start;
clock_t end0;//计时变量

struct tabuList//定义禁忌表结构体
{
	int tabuplan[MANS+1] = { 0 };//储存禁忌的任务序列与所需时间
	int tabulenth = TABULENTH;//禁忌长度
};
tabuList List[LISTLENTH];//定义禁忌表List

int getTime(int Plan[]);//计算任务序列时间
void initPlan(int* Plan);//随机生成初始任务序列
int tabuSearch(int Plan[]);//禁忌搜索
int* neighbourPlan(int Plan[]);//邻域生成
int findBest(int temparr[][MANS+1]);//找寻最优解
void updatetabuList(int* curbestPlan);//更新禁忌表


int main()
{
	initPlan(Plan);
	cout << "	初始任务分配情况如下：\n" << endl;
	cout << "	|  1号  |  2号  |  3号  |  4号  |  5号  |  6号  |  7号  |  8号  |  9号  | 时 间" << endl;
	cout << "	--------------------------------------------------------------------------------" << endl;
	cout << "	| " ;
	for (int i = 0; i < MANS; i++)
	{
		cout <<"任务" << Plan[i] << " | ";
	}
	cout << getTime(Plan) << "分钟\n"<<endl;
	cout << "	按任意键开始计算\n" << endl;
	system("pause");
	tabuSearch(Plan);

	cout << "\n	最佳任务分配情况如下：\n" << endl;
	cout << "	|  1号  |  2号  |  3号  |  4号  |  5号  |  6号  |  7号  |  8号  |  9号  | 时 间" << endl;
	cout << "	--------------------------------------------------------------------------------" << endl;
	cout << "	| ";
	for (int i = 0; i < MANS; i++)
	{
		cout << "任务" << allbestPlan[i] << " | ";
	}
	cout << allbestPlan[MANS] << "分钟\n" << endl;

	return 0;
}

int getTime(int Plan[])//查表获取某一方案的时间和
{
	int timesum = 0;
	for (int i = 0; i < MANS; i++)
	{
		int time[9][9] = //任务时间表
		{
			{5,8,2,10,3,4,7,5,9},
			{10,3,6,1,4,5,7,9,8},
			{3,7,10,6,1,2,5,4,8},
			{1,9,5,3,10,2,4,7,6},
			{7,3,6,4,5,8,9,2,1},
			{9,3,6,5,4,8,3,2,10},
			{1,4,7,8,6,9,5,3,8},
			{4,5,3,2,6,7,8,10,4},
			{3,2,5,9,6,10,4,7,1}
		};
		timesum += time[i][Plan[i] - 1];
	}
	return timesum;
}
void initPlan(int* Plan)
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < MANS; i++)
	{
		Plan[i] = (rand() % MANS)+1;
		for (int j = 0; j < i; j++)
		{
			if (Plan[i] == Plan[j]) 
			{
				i--;
				break;
			}
		}
	}
}
int* neighbourPlan(int Plan[])//获取某一方案的一个邻域解，这里的邻域操作是某两人互换任务。
{
	int* tempPlan1=new int [MANS];
	for(int i=0;i<MANS;i++)
	tempPlan1[i] = Plan[i];

	int a = (rand() % MANS) ;
	int b = (rand() % MANS) ;
	while (b == a)
	{
		b = (rand() % MANS) ;
	}
	int temp = tempPlan1[a];
	tempPlan1[a] = tempPlan1[b];
	tempPlan1[b] = temp;
	return tempPlan1;

}
int tabuSearch(int Plan[])//禁忌搜索
{
	for (int i = 0; i< MANS; i++)//初始化历史最优解为初始解
	{
		allbestPlan[i] = Plan[i];
	}
	allbestPlan[MANS] = getTime(Plan);


	while (GENERATION>0)
	{
		start = clock();
		int temparr[NEIGHBOUR_SIZE][MANS+1]={0};//用来储存找到的邻域
		
		for (int i = 0; i < NEIGHBOUR_SIZE; i++)//找到所有符合要求的邻域
		{
			int* tempPlan = NULL;
			tempPlan = neighbourPlan(Plan);
			int temptime = NULL;
			temptime = getTime(tempPlan);
			
			for(int j=0;j<MANS;j++)
			temparr[i][j] = tempPlan[j];
			temparr[i][MANS] = temptime;
			int n = i;
				
			for (int j = 0; j < n; j++)//检查找到的邻域解是否重复
			{
				int breakflag = 0;
				for (int k = 0; k < MANS; k++)
				{
					
					if (tempPlan[k] == temparr[j][k])
					{
						breakflag++;
					}
				
				}
				if (breakflag == MANS)
				{
					i--;
					break;
				}
			}


			for (int j = 0; j < LISTLENTH; j++)//查找邻域解是否被禁忌
			{
				int breakflag = 0;
				for (int k = 0; k < MANS; k++)
				{
					
					if (tempPlan[k] == List[j].tabuplan[k])
					{
						breakflag ++;
					}
					
				}
				if (breakflag == MANS && (curbestPlan[MANS]-temptime)<=2)//若比上一代最佳时间下降大于2可特赦，小于等于2就正常禁忌
				{
					i--;
					break;
				}
			}			
		}



		for (int i = 0; i < MANS+1; i++)//查找和储存当代邻域中的最优解
		{
			curbestPlan[i] = temparr[findBest(temparr)][i];
		}
		if (curbestPlan[MANS] < allbestPlan[MANS])//判断是否更新历史最优解
		{
			memcpy(allbestPlan,curbestPlan,sizeof(curbestPlan));
			allbestPlan[MANS+1] = jump;//更新跳出代数
		}

		memcpy(Plan, curbestPlan, sizeof(curbestPlan));//更新初始解为当代最佳解
		updatetabuList(curbestPlan);//将当代最佳解放入禁忌表，并更新禁忌表

		end0 = clock();
		double endtime = (double)(end0 - start) / CLOCKS_PER_SEC;//迭代计时


		cout << "邻域最佳方案: ";
		for (int i = 0; i < MANS; i++)
			cout << curbestPlan[i]<<" ";
		cout << "	";
		cout << "邻域最低时间: "<< curbestPlan[MANS];
		cout << "	";
		cout << "历史最低时间: " << allbestPlan[MANS];
		cout << "	";
		cout << "代数: " << 5001-GENERATION;
		cout << "	";
		cout << "迭代时间:" << endtime *1000<< "ms";
		cout << endl;


		GENERATION--;
		allbestPlan[MANS+ 1]--;//迭代
		if (allbestPlan[MANS + 1] < 1)
			break;//历史最优如果jump代没变，跳出循换

	}
	return 0;
}
int findBest(int temparr[][MANS+1])
{
	int min=temparr[0][MANS];
	int flag = 0;
	for (int i = 1; i < NEIGHBOUR_SIZE; i++)
	{
		if (min > temparr[i][MANS])
		{
			min = temparr[i][MANS];
			flag = i;
		}
		
	}
	return flag;
}

void updatetabuList(int* curbestPlan)
{
	int empty_line = -1;
	for (int i = 0; i < LISTLENTH; i++)//更新每行禁忌长度
	{
		if (List[i].tabuplan[0] != 0)
		List[i].tabulenth--;
		if (List[i].tabulenth == 0)//清空禁忌长度为0的行
		{
			for (int j = 0; j < MANS + 1; j++)
				List[i].tabuplan[j] = 0;
		}
	}
	for (int i = 0; i < LISTLENTH; i++)//找到禁忌表空行
	{
		if (List[i].tabuplan[0] == 0)
		{
			empty_line = i;
			break;
		}
	}
	for (int i = 0; i < MANS + 1; i++)//将当代最优解写入禁忌表空行
	{
		if (empty_line >= 0)//防止List写入溢出警告的判断，实际上empty_line一定大于等于0
		List[empty_line].tabuplan[i] = curbestPlan[i];
	}
	if (empty_line >= 0)
	List[empty_line].tabulenth = TABULENTH;//更新写入数据禁忌长度
}

