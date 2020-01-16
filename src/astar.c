#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define BUFSIZE 256

/// 二次元情報を持つ点。
typedef struct POINT{
    int x;
    int y;
}Point;

/// 最終経路を表示する。
void printGoalMap(Point startPos, Point goalPos, Point stageSize, Point prev[][BUFSIZE], int stage[][BUFSIZE]){
    char goalMap[stageSize.y][stageSize.x]; // 表示用配列
    Point tmp = goalPos;
    int cost = 0;

    for(int y = 0; y < stageSize.y; y++){
        for(int x = 0; x < stageSize.x; x++){
            if(stage[y][x] == -1){
                goalMap[y][x] = '@';
            }else{
                goalMap[y][x] = ' ';
            }
        }
    }
    goalMap[startPos.y][startPos.x] = 'S';

    while(tmp.x != startPos.x || tmp.y != startPos.y){
        cost += stage[tmp.y][tmp.x];
        goalMap[tmp.y][tmp.x] = '+';
        tmp = prev[tmp.y][tmp.x];
        
    }
    goalMap[goalPos.y][goalPos.x] = 'G';
    
    printf("cost = %d\n", cost);

    for(int y = 0; y < stageSize.y; y++){
        for(int x = 0; x < stageSize.x; x++){
            printf("%c", goalMap[y][x]);
        }
        putchar('\n');
    }

    return;
}

/// ２点間の予測距離を計算する関数。
double distCalc(Point pos1, Point pos2){
    return abs(pos2.x - pos1.x) + abs(pos2.y - pos1.y);
    //return sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2));
}


Point scanPoint(FILE *fp){
    char buf[BUFSIZE];
    Point point;

    if(fgets(buf, sizeof(buf), fp) == NULL){
        printf("ファイルが正しいかどうか確認してください。");
        fclose(fp);
        exit(1);
    }
    sscanf(buf, "%d, %d\n", &point.x, &point.y);
    //printf("%d, %d\n", point.x, point.y);

    return point;
}

int main(void){
    FILE *fp;
    char filepath[BUFSIZE] = "./stage/";
    char filename[] = "stage3.csv";
    char buf[BUFSIZE];
    char *p, *end;
    Point stageSize;
    Point startPos, goalPos;
    int stage[BUFSIZE][BUFSIZE];
    int actualCost[BUFSIZE][BUFSIZE];
    Point prev[BUFSIZE][BUFSIZE];
    Point open[BUFSIZE];
    int size = 0;
    int mindex;
    Point top, nextPos, tmp;
    int dx[] = {0, 1, 0, -1};
    //int dx[] = {0, 1, 1, 1, 0, -1, -1, -1}; // 8近傍距離の場合
    int dy[] = {1, 0, -1, 0};
    //int dy[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int neighbor = 4;
    int rooptime = 0;

    
    fp = fopen(strcat(filepath, filename), "r");

    //* ファイルが開けない場合の処理 
    if(fp == NULL){
        printf("ファイルが開けません\n");
        fclose(fp);
        return -1;
    }

    //* ファイル読み込み 
    stageSize = scanPoint(fp);
    startPos = scanPoint(fp);
    goalPos = scanPoint(fp);
    
    for(int y = 0; y < stageSize.y; y++){
        if(fgets(buf, sizeof(buf), fp) == NULL){
            printf("ファイルに誤りがあります。\n");
            fclose(fp);
            return -1;
        }
        p = buf;
        for(int x = 0; x < stageSize.x; x++){
            long num = strtol(p, &end, 10);
            if(p == end){
                break;
            }
            stage[y][x] = num;
            p = end + 1; // ','を飛ばす
        }
    }

    fclose(fp);
    
    /* 標準入出力で流し込む場合
    scanf("%d, %d", &stageSize.x, &stageSize.y);
    scanf("%d, %d", &startPos.x, &startPos.y);
    scanf("%d, %d", &goalPos.x, &goalPos.y);

    for(int y = 0; y < stageSize.y; y++){
        for(int x = 0; x < stageSize.x; x++){
            scanf(" %d%*c", &stage[y][x]);  
            printf("%d,", stage[y][x]);
        }
        putchar('\n');
    }
    */

    /* actualCost[][]の初期化 */
    for(int y = 0; y < stageSize.y; y++){
        for(int x = 0; x < stageSize.x; x++){
            actualCost[y][x] = -1;
        }
    }
    actualCost[startPos.y][startPos.x] = 0;

    open[size++] = startPos;

    while(size > 0){
        rooptime++;
        /* 並べ替え */
        mindex = 0;
        for(int i = 0; i < size; i++){
            if(distCalc(open[mindex], goalPos) + (double)actualCost[open[mindex].y][open[mindex].x] >
                    distCalc(open[i], goalPos) + (double)actualCost[open[i].y][open[i].x]){
                mindex = i;
            }
        }
        tmp = open[size-1];
        open[size-1] = open[mindex];
        open[mindex] = tmp;

        top = open[--size];
        printf("(%d, %d), ", top.x, top.y);

        if(top.x == goalPos.x && top.y == goalPos.y){
            printf("goal\n");
            printf("rooptime = %d\n", rooptime);
            printGoalMap(startPos, goalPos, stageSize, prev, stage);

            return 0;
        }


        for(int i = 0; i < neighbor; i++){
            nextPos = (Point){top.x + dx[i], top.y + dy[i]};
            if(stage[nextPos.y][nextPos.x] != -1 &&
              actualCost[nextPos.y][nextPos.x] == -1){
                actualCost[nextPos.y][nextPos.x] =
                  actualCost[top.y][top.x] + stage[nextPos.y][nextPos.x];
                
                open[size++] = nextPos; 
                prev[nextPos.y][nextPos.x] = top;
            }
        }
    }

    printf("NG\n");
    return 0;
}